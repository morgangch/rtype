/**
 * @file GUIHelper.cpp
 * @brief Implementation of GUI utility functions and constants
 * 
 * This file implements the static utility functions and constants defined in
 * GUIHelper.hpp. It provides common GUI operations, standardized colors and
 * sizes, and helper functions used throughout the R-TYPE client interface.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GUIHelper.hpp"
#include "gui/FontManager.hpp"
#include <cstdlib>

namespace rtype::client::gui {
    
    // Color constant definitions
    const sf::Color GUIHelper::Colors::BACKGROUND = sf::Color(20, 20, 30);
    const sf::Color GUIHelper::Colors::TEXT = sf::Color::White;
    const sf::Color GUIHelper::Colors::BUTTON_NORMAL = sf::Color(70, 70, 70, 200);
    const sf::Color GUIHelper::Colors::BUTTON_HOVER = sf::Color(100, 100, 100, 200);
    const sf::Color GUIHelper::Colors::BUTTON_READY = sf::Color(50, 150, 50, 200);
    const sf::Color GUIHelper::Colors::RETURN_BUTTON = sf::Color(100, 50, 50, 200);
    const sf::Color GUIHelper::Colors::INPUT_BOX = sf::Color(50, 50, 50, 200);
    const sf::Color GUIHelper::Colors::HINT_TEXT = sf::Color(150, 150, 150);
    
    const sf::Font& GUIHelper::getFont() {
        return FontManager::getInstance().getDefaultFont();
    }
    
    void GUIHelper::centerText(sf::Text& text, float x, float y) {
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x - textBounds.width / 2, y - textBounds.height / 2);
    }
    
    bool GUIHelper::isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect) {
        sf::FloatRect bounds = rect.getGlobalBounds();
        return bounds.contains(point);
    }
    
    void GUIHelper::setupButton(sf::Text& text, sf::RectangleShape& rect, 
                               const std::string& label, float fontSize) {
        // Setup text
        text.setFont(getFont());
        text.setString(label);
        text.setCharacterSize(static_cast<unsigned int>(fontSize));
        text.setFillColor(Colors::TEXT);
        
        // Setup rectangle
        rect.setFillColor(Colors::BUTTON_NORMAL);
        rect.setOutlineColor(Colors::TEXT);
        rect.setOutlineThickness(2.0f);
    }
    
    void GUIHelper::setupReturnButton(sf::Text& text, sf::RectangleShape& rect) {
        setupButton(text, rect, "Return", Sizes::RETURN_BUTTON_FONT_SIZE);
        rect.setFillColor(Colors::RETURN_BUTTON);
    }
    
    void GUIHelper::applyButtonHover(sf::RectangleShape& rect, sf::Text& text, bool isHovered,
                                   const sf::Color& normalColor, const sf::Color& hoverColor) {
        if (isHovered) {
            rect.setFillColor(hoverColor);
            text.setFillColor(sf::Color::Cyan);
        } else {
            rect.setFillColor(normalColor);
            text.setFillColor(Colors::TEXT);
        }
    }
    
    std::string GUIHelper::generateRandomUsername() {
        // Generate random 4-digit number between 1000-9999
        int randomNum = 1000 + (rand() % 9000);
        return "USERNAME" + std::to_string(randomNum);
    }
    
    bool GUIHelper::isValidServerCode(const std::string& code) {
        if (code.length() != 4) return false;
        
        try {
            int num = std::stoi(code);
            return num >= 1000 && num <= 9999;
        } catch (...) {
            return false;
        }
    }
}
