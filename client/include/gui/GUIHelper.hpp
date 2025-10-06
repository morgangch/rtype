/**
 * @file GUIHelper.hpp
 * @brief Centralized GUI utilities and constants for the R-TYPE client
 * 
 * This file contains the GUIHelper class which provides static utility functions
 * and constants for consistent GUI behavior across all interface elements.
 * It centralizes common operations like text centering, button setup, collision
 * detection, and provides standardized colors and sizes.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_GUI_HELPER_HPP
#define CLIENT_GUI_HELPER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace rtype::client::gui {
    
    /**
     * @class GUIHelper
     * @brief Static utility class for common GUI operations and constants
     * 
     * The GUIHelper class provides a collection of static utility functions
     * and constants that are commonly used throughout the GUI system.
     * This centralizes common functionality and ensures consistency.
     * 
     * Features:
     * - Standardized color palette for UI elements
     * - Consistent font sizes for different UI components
     * - Common geometric operations (text centering, collision detection)
     * - Button setup and styling utilities
     * - Input validation functions
     * 
     * All methods are static, so no instantiation is required.
     * 
     * Usage:
     * @code
     * GUIHelper::centerText(titleText, 400, 200);
     * bool clicked = GUIHelper::isPointInRect(mousePos, buttonRect);
     * @endcode
     */
    class GUIHelper {
    public:
        // Font utilities
        /**
         * @brief Get the default font from FontManager
         * @return Const reference to the default font
         */
        static const sf::Font& getFont();
        
        // Layout utilities
        /**
         * @brief Center text at the specified coordinates
         * @param text The text object to center
         * @param x The x-coordinate for the center point
         * @param y The y-coordinate for the center point
         */
        static void centerText(sf::Text& text, float x, float y);
        
        /**
         * @brief Check if a point is inside a rectangle
         * @param point The point to test (usually mouse position)
         * @param rect The rectangle to test against
         * @return True if the point is inside the rectangle, false otherwise
         */
        static bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        
        // Button utilities
        /**
         * @brief Setup a standard button with text and rectangle
         * @param text The text object to configure
         * @param rect The rectangle object to configure
         * @param label The text to display on the button
         * @param fontSize The font size for the button text (default: 28.0f)
         */
        static void setupButton(sf::Text& text, sf::RectangleShape& rect, 
                               const std::string& label, float fontSize = 28.0f);
        
        /**
         * @brief Setup a standardized return button
         * @param text The text object to configure
         * @param rect The rectangle object to configure
         */
        static void setupReturnButton(sf::Text& text, sf::RectangleShape& rect);
        
        /**
         * @brief Apply hover effects to a button
         * @param rect The button rectangle to modify
         * @param text The button text to modify
         * @param isHovered True if the button is being hovered over
         * @param normalColor The normal color of the button
         * @param hoverColor The hover color of the button
         */
        static void applyButtonHover(sf::RectangleShape& rect, sf::Text& text, bool isHovered,
                                   const sf::Color& normalColor = sf::Color(70, 70, 70, 200),
                                   const sf::Color& hoverColor = sf::Color(100, 100, 100, 200));
        
        // Input utilities
        /**
         * @brief Generate a random username for new players
         * @return A string containing a random username (format: "USERNAME" + 4-digit number)
         */
        static std::string generateRandomUsername();
        
        /**
         * @brief Validate a server code format
         * @param code The server code string to validate
         * @return True if the code is a valid 4-digit number (1000-9999), false otherwise
         */
        static bool isValidServerCode(const std::string& code);
        
        /**
         * @struct Colors
         * @brief Standardized color palette for the GUI
         * 
         * Contains predefined colors used throughout the interface to ensure
         * visual consistency across all GUI elements.
         */
        struct Colors {
            static const sf::Color BACKGROUND;        ///< Dark background color for windows
            static const sf::Color TEXT;              ///< Standard white text color
            static const sf::Color BUTTON_NORMAL;     ///< Default button background color
            static const sf::Color BUTTON_HOVER;      ///< Button color when hovered
            static const sf::Color BUTTON_READY;      ///< Green color for ready buttons
            static const sf::Color RETURN_BUTTON;     ///< Red color for return/back buttons
            static const sf::Color INPUT_BOX;         ///< Background color for input fields
            static const sf::Color HINT_TEXT;         ///< Gray color for placeholder text
        };
        
        /**
         * @struct Sizes
         * @brief Standardized sizes and dimensions for GUI elements
         * 
         * Contains predefined size constants used throughout the interface
         * to ensure consistent sizing and spacing of UI elements.
         */
        struct Sizes {
            static constexpr float BUTTON_WIDTH = 200.0f;          ///< Standard button width
            static constexpr float BUTTON_HEIGHT = 60.0f;          ///< Standard button height
            static constexpr float RETURN_BUTTON_WIDTH = 120.0f;   ///< Return button width
            static constexpr float RETURN_BUTTON_HEIGHT = 40.0f;   ///< Return button height
            static constexpr float INPUT_BOX_HEIGHT = 50.0f;       ///< Height for input text fields
            static constexpr float BUTTON_SPACING = 20.0f;         ///< Spacing between buttons
            
            // Font sizes
            static constexpr int TITLE_FONT_SIZE = 64;              ///< Large font for main titles
            static constexpr int BUTTON_FONT_SIZE = 28;             ///< Standard button text size
            static constexpr int RETURN_BUTTON_FONT_SIZE = 24;      ///< Return button text size
            static constexpr int INPUT_FONT_SIZE = 24;              ///< Input field text size
            static constexpr int HINT_FONT_SIZE = 20;               ///< Placeholder/hint text size
        };
    };
}

#endif // CLIENT_GUI_HELPER_HPP
