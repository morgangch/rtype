/**
 * @file SettingsConfig.h
 * @brief Settings configuration manager for R-TYPE client
 *
 * This file contains the SettingsConfig class which manages loading and saving
 * user settings to a JSON configuration file. Settings include keybinds, IP, and port.
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_GUI_SETTINGSCONFIG_H
#define CLIENT_GUI_SETTINGSCONFIG_H

#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

namespace rtype::client::gui {

/**
 * @class SettingsConfig
 * @brief Manages user settings persistence in JSON format
 *
 * Features:
 * - Load settings from config.json
 * - Save settings to config.json
 * - Default values if config file doesn't exist
 * - Keybinds mapping (Up, Down, Left, Right, Shoot)
 * - Network settings (IP and Port)
 *
 * JSON Structure:
 * {
 *   "keybinds": {
 *     "up": 22,      // W key
 *     "down": 18,    // S key
 *     "left": 0,     // A key
 *     "right": 3,    // D key
 *     "shoot": 57    // Space key
 *   },
 *   "network": {
 *     "ip": "127.0.0.1",
 *     "port": "4242"
 *   }
 * }
 */
class SettingsConfig {
public:
    /**
     * @brief Construct a new SettingsConfig object
     * @param configPath Path to the configuration file (default: "config.json")
     */
    SettingsConfig(const std::string& configPath = "config.json");

    /**
     * @brief Load settings from the configuration file
     * @return true if loaded successfully, false if using defaults
     */
    bool load();

    /**
     * @brief Save current settings to the configuration file
     * @return true if saved successfully, false otherwise
     */
    bool save();

    // Keybinds getters/setters
    sf::Keyboard::Key getKeybind(const std::string& action) const;
    void setKeybind(const std::string& action, sf::Keyboard::Key key);

    // Network getters/setters
    std::string getIP() const { return ip; }
    void setIP(const std::string& newIp) { ip = newIp; }

    std::string getPort() const { return port; }
    void setPort(const std::string& newPort) { port = newPort; }

    /**
     * @brief Reset all settings to default values
     */
    void resetToDefaults();

private:
    std::string configPath;  ///< Path to the config file
    std::map<std::string, sf::Keyboard::Key> keybinds;  ///< Keybind mappings
    std::string ip;   ///< Server IP address
    std::string port; ///< Server port

    /**
     * @brief Initialize default settings
     */
    void initDefaults();

    /**
     * @brief Parse a simple JSON-like string manually
     * @param content The file content to parse
     */
    void parseJSON(const std::string& content);

    /**
     * @brief Generate JSON string from current settings
     * @return JSON formatted string
     */
    std::string generateJSON() const;

    /**
     * @brief Trim whitespace from a string
     */
    std::string trim(const std::string& str) const;

    /**
     * @brief Extract value from "key": value pattern
     */
    std::string extractValue(const std::string& line) const;
};

} // namespace rtype::client::gui

#endif // CLIENT_GUI_SETTINGSCONFIG_H
