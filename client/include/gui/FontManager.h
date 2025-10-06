/**
 * @file @file FontManager.h
 * @brief Centralized font management system for the R-TYPE client GUI
 * 
 * This file contains the FontManager class which implements a singleton pattern
 * to provide centralized font loading and management across the entire GUI system.
 * It handles platform-specific font fallbacks and ensures consistent font usage.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_FONT_MANAGER_HPP
#define CLIENT_FONT_MANAGER_HPP

#include <SFML/Graphics.hpp>

namespace rtype::client::gui {
    
    /**
     * @class FontManager
     * @brief Singleton class for centralized font management
     * 
     * The FontManager class provides a centralized way to load and access fonts
     * throughout the GUI system. It implements the singleton pattern to ensure
     * only one instance exists and fonts are loaded only once.
     * 
     * Features:
     * - Platform-specific font fallback system (Linux, macOS, Windows)
     * - Automatic font loading with error handling
     * - Single point of access for all GUI components
     * - Memory efficient (fonts loaded once and reused)
     * 
     * Usage:
     * @code
     * const sf::Font& font = FontManager::getInstance().getDefaultFont();
     * text.setFont(font);
     * @endcode
     */
    class FontManager {
    public:
        /**
         * @brief Get the singleton instance of FontManager
         * @return Reference to the singleton FontManager instance
         */
        static FontManager& getInstance();
        
        /**
         * @brief Get the default font for GUI elements
         * @return Const reference to the loaded font
         * @note If no font could be loaded, returns SFML's default font
         */
        const sf::Font& getDefaultFont();
        /**
         * @brief Check if a font was successfully loaded
         * @return True if font loading was successful, false otherwise
         */
        bool isLoaded() const { return fontLoaded; }
        
    private:
        /**
         * @brief Private constructor for singleton pattern
         * Initializes the FontManager and attempts to load a font
         */
        FontManager();
        
        /**
         * @brief Default destructor
         */
        ~FontManager() = default;
        
        /**
         * @brief Deleted copy constructor to prevent copying
         */
        FontManager(const FontManager&) = delete;
        
        /**
         * @brief Deleted assignment operator to prevent assignment
         */
        FontManager& operator=(const FontManager&) = delete;
        
        sf::Font font;          ///< The loaded font object
        bool fontLoaded;        ///< Flag indicating if font loading was successful
        
        /**
         * @brief Attempts to load a font from various system locations
         * @return True if font loading was successful, false otherwise
         * 
         * Tries to load fonts in the following order:
         * 1. Custom font: "assets/fonts/arial.ttf"
         * 2. Linux: "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
         * 3. Linux: "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
         * 4. macOS: "/System/Library/Fonts/Arial.ttf"
         * 5. Windows: "C:/Windows/Fonts/arial.ttf"
         */
        bool loadFont();
    };
}

#endif // CLIENT_FONT_MANAGER_HPP
