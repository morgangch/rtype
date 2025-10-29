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

    /**
     * @brief Get the keybind for a specific action
     * @param action The action name (e.g., "up", "down", "left", "right", "shoot")
     * @return The keyboard key assigned to this action, or sf::Keyboard::Unknown if not found
     */
    sf::Keyboard::Key getKeybind(const std::string& action) const;
    
    /**
     * @brief Set the keybind for a specific action
     * @param action The action name (e.g., "up", "down", "left", "right", "shoot")
     * @param key The keyboard key to assign to this action
     */
    void setKeybind(const std::string& action, sf::Keyboard::Key key);

    /**
     * @brief Get the secondary binding for an action (opaque int).
     *
     * Secondary bindings are an opaque integer that can encode other input
     * devices (joystick buttons, mouse buttons, etc.). The primary API
     * remains keyboard-oriented for backward compatibility.
     */
    int getSecondaryKeybind(const std::string& action) const;

    /**
     * @brief Set the secondary binding for an action (opaque int).
     */
    void setSecondaryKeybind(const std::string& action, int code);

    /**
     * @brief Get the server IP address
     * @return The configured server IP address as a string
     */
    std::string getIP() const { return ip; }
    
    /**
     * @brief Set the server IP address
     * @param newIp The new IP address to set
     */
    void setIP(const std::string& newIp) { ip = newIp; }

    /**
     * @brief Get the server port
     * @return The configured server port as a string
     */
    std::string getPort() const { return port; }
    
    /**
     * @brief Set the server port
     * @param newPort The new port number to set (as string)
     */
    void setPort(const std::string& newPort) { port = newPort; }

    /**
     * @brief Reset all settings to default values
     */
    void resetToDefaults();

private:
    std::string configPath;  ///< Path to the configuration file
    std::map<std::string, sf::Keyboard::Key> keybinds;  ///< Map of action names to keyboard keys
    std::map<std::string, int> secondaryKeybinds;       ///< Secondary, opaque bindings (joystick/mouse encoded)
    std::string ip;   ///< Server IP address for network connection
    std::string port; ///< Server port number for network connection

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
