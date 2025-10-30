/**
 * @file HighscoreManager.h
 * @brief Minimal JSON-backed highscores persistence utility.
 *
 * This header declares a lightweight, header-only-friendly interface to
 * persist and query player highscores. Data is stored on disk as a small
 * JSON file (default: "scores.json" in the working directory) and kept in
 * memory as a vector of entries while the program runs.
 *
 * Responsibilities:
 * - Load previously saved scores from disk into memory.
 * - Append new entries produced during gameplay.
 * - Save the in-memory list back to disk.
 * - Provide helpers to query the top-N entries by score.
 *
 * JSON file format (example):
 * {
 *   "scores": [
 *     {"name":"Alice","player":1,"score":120,"ts":1700000000},
 *     {"name":"Bob","player":2,"score":95,"ts":1700000100}
 *   ]
 * }
 *
 * Notes and guarantees:
 * - The API does not throw on I/O failures; instead, load() and save()
 *   return false to indicate errors while keeping current state intact.
 * - Names are sanitized on insertion to avoid control characters and to
 *   keep a reasonable length suitable for UI rendering.
 * - Timestamps (ts) are stored as seconds since Unix epoch (UTC).
 * - This component is not thread-safe; serialize access if used across
 *   threads.
 */

#ifndef CLIENT_HIGHSCORE_MANAGER_HPP
#define CLIENT_HIGHSCORE_MANAGER_HPP

#include <string>
#include <vector>

namespace rtype::client::gui {

/**
 * @brief One persisted highscore record.
 *
 * Fields:
 * - name: Player display name (sanitized on insert).
 * - playerIndex: Local player slot or controller index (1-based by default).
 * - score: The numeric score achieved in the run.
 * - ts: Unix timestamp (seconds) when the score was recorded.
 */
struct HighscoreEntry {
    std::string name;
    int playerIndex{1};
    int score{0};
    long ts{0};
};

/**
 * @brief Minimal highscores manager reading/writing a compact JSON file.
 *
 * Typical usage:
 * @code{.cpp}
 * HighscoreManager hs{"scores.json"};
 * hs.load();
 * hs.add({"Player1", 1, 12345, std::time(nullptr)});
 * hs.save();
 * auto top5 = hs.topN(5);
 * @endcode
 */
class HighscoreManager {
public:
    /**
     * @brief Construct a manager bound to a JSON file path.
     * @param path Path to the JSON file to read/write. Defaults to "scores.json".
     */
    explicit HighscoreManager(const std::string& path = "scores.json");

    /**
     * @brief Load scores from disk into memory.
     * @return true on success; false on I/O or parse error (keeps current entries).
     * @note On failure, no in-memory entries are discarded or modified.
     */
    bool load();
    /**
     * @brief Save current in-memory entries to disk.
     * @return true on success; false on I/O error.
     */
    bool save() const;

    /**
     * @brief Add a single entry to the in-memory list.
     * @param e Entry to insert; its name will be sanitized before storage.
     * @note Call save() to persist to disk.
     */
    void add(const HighscoreEntry& e);

    /**
     * @brief Return the top N entries by score (descending order).
     * @param n Maximum number of entries to return.
     * @return A vector containing up to n best-scoring entries.
     * @note Stable ordering for ties is not guaranteed.
     */
    std::vector<HighscoreEntry> topN(std::size_t n) const;

    /**
     * @brief Access all entries currently loaded (read-only).
     */
    const std::vector<HighscoreEntry>& entries() const { return m_entries; }

private:
    std::string m_path;
    std::vector<HighscoreEntry> m_entries;

    /**
     * @brief Sanitize a player name for safe storage and UI display.
     *
     * Rules of thumb (implementation-defined):
     * - Trim surrounding whitespace;
     * - Remove control characters and non-printable glyphs;
     * - Limit length to a reasonable maximum;
     * - Optionally collapse repeated spaces.
     *
     * @param in Raw input name provided by the user.
     * @return A sanitized name string.
     */
    static std::string sanitizeName(const std::string& in);
};

} // namespace rtype::client::gui

#endif // CLIENT_HIGHSCORE_MANAGER_HPP
