/**
 * @file FontManager.cpp
 * @brief Implementation of the FontManager singleton class
 * 
 * This file implements the FontManager class methods for centralized font
 * management in the R-TYPE client GUI system. It handles font loading with
 * platform-specific fallbacks and provides thread-safe singleton access.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/FontManager.hpp"
#include <iostream>

namespace rtype::client::gui {
    
    FontManager& FontManager::getInstance() {
        static FontManager instance;
        return instance;
    }
    
    FontManager::FontManager() : fontLoaded(false) {
        fontLoaded = loadFont();
        if (!fontLoaded) {
            std::cerr << "Warning: Could not load any font file, using SFML default" << std::endl;
        }
    }
    
    const sf::Font& FontManager::getDefaultFont() {
        return font;
    }
    
    bool FontManager::loadFont() {
        // Try to load fonts in order of preference
        if (font.loadFromFile("assets/fonts/arial.ttf")) {
            return true;
        }
        // Try common system fonts
        else if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            return true;
        }
        else if (font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            return true;
        }
        else if (font.loadFromFile("/System/Library/Fonts/Arial.ttf")) { // macOS
            return true;
        }
        else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) { // Windows
            return true;
        }
        
        return false;
    }
}
