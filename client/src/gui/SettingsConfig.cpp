/**
 * @file SettingsConfig.cpp
 * @brief Implementation of settings configuration manager
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/SettingsConfig.h"
#include <iostream>
#include <algorithm>

namespace rtype::client::gui {

SettingsConfig::SettingsConfig(const std::string& configPath)
    : configPath(configPath)
{
    initDefaults();
}

void SettingsConfig::initDefaults() {
    // Default keybinds
    keybinds["up"] = sf::Keyboard::W;
    keybinds["down"] = sf::Keyboard::S;
    keybinds["left"] = sf::Keyboard::A;
    keybinds["right"] = sf::Keyboard::D;
    keybinds["shoot"] = sf::Keyboard::Space;

    // Default network settings
    ip = "127.0.0.1";
    port = "4242";
}

bool SettingsConfig::load() {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cout << "[SettingsConfig] Config file not found, using defaults." << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    parseJSON(content);
    std::cout << "[SettingsConfig] Settings loaded from " << configPath << std::endl;
    return true;
}

bool SettingsConfig::save() {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "[SettingsConfig] Failed to open config file for writing." << std::endl;
        return false;
    }

    std::string jsonContent = generateJSON();
    file << jsonContent;
    file.close();

    std::cout << "[SettingsConfig] Settings saved to " << configPath << std::endl;
    return true;
}

sf::Keyboard::Key SettingsConfig::getKeybind(const std::string& action) const {
    auto it = keybinds.find(action);
    if (it != keybinds.end()) {
        return it->second;
    }
    return sf::Keyboard::Unknown;
}

void SettingsConfig::setKeybind(const std::string& action, sf::Keyboard::Key key) {
    keybinds[action] = key;
}

void SettingsConfig::resetToDefaults() {
    initDefaults();
}

void SettingsConfig::parseJSON(const std::string& content) {
    // Simple JSON parser for our specific structure
    // This is a basic implementation that works for our format
    std::istringstream stream(content);
    std::string line;
    bool inKeybinds = false;
    bool inNetwork = false;

    while (std::getline(stream, line)) {
        line = trim(line);

        if (line.find("\"keybinds\"") != std::string::npos) {
            inKeybinds = true;
            inNetwork = false;
        } else if (line.find("\"network\"") != std::string::npos) {
            inNetwork = true;
            inKeybinds = false;
        } else if (line == "}" || line == "},") {
            inKeybinds = false;
            inNetwork = false;
        } else if (inKeybinds) {
            // Parse keybind line: "up": 22,
            if (line.find("\"up\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) keybinds["up"] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            } else if (line.find("\"down\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) keybinds["down"] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            } else if (line.find("\"left\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) keybinds["left"] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            } else if (line.find("\"right\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) keybinds["right"] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            } else if (line.find("\"shoot\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) keybinds["shoot"] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
        } else if (inNetwork) {
            // Parse network line: "ip": "127.0.0.1",
            if (line.find("\"ip\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) ip = value;
            } else if (line.find("\"port\"") != std::string::npos) {
                std::string value = extractValue(line);
                if (!value.empty()) port = value;
            }
        }
    }
}

std::string SettingsConfig::generateJSON() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"keybinds\": {\n";
    json << "    \"up\": " << static_cast<int>(keybinds.at("up")) << ",\n";
    json << "    \"down\": " << static_cast<int>(keybinds.at("down")) << ",\n";
    json << "    \"left\": " << static_cast<int>(keybinds.at("left")) << ",\n";
    json << "    \"right\": " << static_cast<int>(keybinds.at("right")) << ",\n";
    json << "    \"shoot\": " << static_cast<int>(keybinds.at("shoot")) << "\n";
    json << "  },\n";
    json << "  \"network\": {\n";
    json << "    \"ip\": \"" << ip << "\",\n";
    json << "    \"port\": \"" << port << "\"\n";
    json << "  }\n";
    json << "}\n";
    return json.str();
}

std::string SettingsConfig::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string SettingsConfig::extractValue(const std::string& line) const {
    // Extract value after ':' and before ','
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) return "";

    size_t start = colonPos + 1;
    size_t end = line.find(',', start);
    if (end == std::string::npos) end = line.length();

    std::string value = line.substr(start, end - start);
    value = trim(value);

    // Remove quotes if present
    if (value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.length() - 2);
    }

    return value;
}

} // namespace rtype::client::gui
