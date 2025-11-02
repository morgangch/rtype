/**
 * @file HighscoreManager.cpp
 * @brief Implementation of a minimal JSON-backed highscores persistence.
 *
 * Overview:
 * - Persists a vector of HighscoreEntry to a compact JSON file
 *   (default path provided by the header's constructor).
 * - Tolerant reader based on regex: accepts flexible ordering/whitespace and
 *   ignores unknown fields.
 * - I/O failures are reported via boolean return values (no exceptions).
 * - Timestamps use Unix epoch seconds. Missing/zero timestamps are filled at
 *   write-time or when adding a new entry.
 *
 * JSON format example:
 * {
 *   "scores": [
 *     {"name":"Alice","player":1,"score":120,"ts":1700000000},
 *     {"name":"Bob","player":2,"score":95,"ts":1700000100}
 *   ]
 * }
 */

#include "gui/HighscoreManager.h"
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <ctime>

namespace rtype::client::gui {

/**
 * @brief Bind the manager to a given JSON file path.
 */
HighscoreManager::HighscoreManager(const std::string& path)
    : m_path(path) {}

/**
 * @brief Read the entire file into a string buffer.
 * @param path File system path to read.
 * @return File contents as a string; empty string if the file cannot be opened.
 */
static std::string readFileAll(const std::string& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

/**
 * @brief Write the provided string to a file (truncate/overwrite).
 * @param path Destination file path.
 * @param content Data to write.
 * @return true on success; false if the file cannot be created or written.
 */
static bool writeFileAll(const std::string& path, const std::string& content) {
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs) return false;
    ofs << content;
    return static_cast<bool>(ofs);
}

/**
 * @brief Load and parse scores from the bound JSON file.
 *
 * Behavior:
 * - Clears current in-memory entries and attempts to parse the file.
 * - Returns true if reading/parsing completes (including the case where
 *   the file is absent/empty), false only on unrecoverable I/O issues.
 * - Parsing is tolerant: it scans objects and extracts name, player, score,
 *   and optional ts in any order, ignoring unknown fields.
 */
bool HighscoreManager::load() {
    m_entries.clear();
    const std::string data = readFileAll(m_path);
    if (data.empty()) {
        return true; // No file yet is OK
    }

    // Regex to capture entries: name (string), player (int), score (int), optional ts (int)
    // This is a tolerant parse; it ignores ordering and whitespace variations.
    std::regex entryRegex(R"(\{[^\}]*?\"name\"\s*:\s*\"([^\"]*)\"[^\}]*?\"player\"\s*:\s*(\d+)\s*[^\}]*?\"score\"\s*:\s*(\d+)(?:[^\}]*?\"ts\"\s*:\s*(\d+))?[^\}]*?\})");
    std::smatch m;
    std::string::const_iterator searchStart(data.cbegin());

    while (std::regex_search(searchStart, data.cend(), m, entryRegex)) {
        HighscoreEntry e;
        e.name = sanitizeName(m[1].str());
        try {
            e.playerIndex = std::stoi(m[2].str());
            e.score = std::stoi(m[3].str());
            e.ts = m[4].matched ? std::stol(m[4].str()) : 0;
        } catch (...) {
            // Skip malformed entry
            e.playerIndex = 1; e.score = 0; e.ts = 0;
        }
        if (!e.name.empty()) m_entries.push_back(e);
        searchStart = m.suffix().first;
    }
    return true;
}

/**
 * @brief Serialize current entries to the bound JSON file.
 *
 * Notes:
 * - Entries with ts==0 are populated with the current time at write.
 * - The output format is compact and stable enough for our tolerant reader.
 */
bool HighscoreManager::save() const {
    std::ostringstream ss;
    ss << "{\n  \"scores\": [\n";
    for (std::size_t i = 0; i < m_entries.size(); ++i) {
        const auto& e = m_entries[i];
        ss << "    {\"name\":\"" << e.name << "\",\"player\":" << e.playerIndex
           << ",\"score\":" << e.score << ",\"ts\":" << (e.ts == 0 ? static_cast<long>(std::time(nullptr)) : e.ts) << "}";
        if (i + 1 < m_entries.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n}";
    return writeFileAll(m_path, ss.str());
}

/**
 * @brief Add an entry to the in-memory list with basic normalization.
 *
 * Normalization rules:
 * - Sanitize the name to remove control/unsafe characters and limit length.
 * - Clamp player index to [1, 4].
 * - Clamp score to [0, +inf).
 * - If ts==0, fill with current time.
 */
void HighscoreManager::add(const HighscoreEntry& e) {
    HighscoreEntry cpy = e;
    cpy.name = sanitizeName(cpy.name);
    if (cpy.playerIndex < 1) cpy.playerIndex = 1;
    if (cpy.playerIndex > 4) cpy.playerIndex = 4;
    if (cpy.score < 0) cpy.score = 0;
    if (cpy.ts == 0) cpy.ts = static_cast<long>(std::time(nullptr));
    m_entries.push_back(cpy);
}

/**
 * @brief Compute the top-N entries by score; break ties by older timestamp.
 * @param n Maximum number of entries to return.
 * @return Vector of up to n entries (score desc, then ts asc).
 */
std::vector<HighscoreEntry> HighscoreManager::topN(std::size_t n) const {
    std::vector<HighscoreEntry> v = m_entries;
    std::stable_sort(v.begin(), v.end(), [](const auto& a, const auto& b){
        if (a.score != b.score) return a.score > b.score; // Descending score
        return a.ts < b.ts; // Older first if tie
    });
    if (v.size() > n) v.resize(n);
    return v;
}

/**
 * @brief Sanitize a player name for safe storage and UI display.
 *
 * Implementation details:
 * - Strips control characters (< 0x20) and JSON-unsafe quotes/backslashes.
 * - Truncates to a max length (24 chars) to ensure UI constraints.
 */
std::string HighscoreManager::sanitizeName(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    for (char c : in) {
        if (static_cast<unsigned char>(c) < 32) continue; // remove control chars
        if (c == '"' || c == '\\') continue; // avoid breaking JSON
        out.push_back(c);
    }
    if (out.size() > 24) out.resize(24);
    return out;
}

} // namespace rtype::client::gui
