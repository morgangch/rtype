/**
 * @file SettingsState.h
 * @brief Settings screen implementation for the R-TYPE client
 * 
 * This file contains the SettingsState class which implements the settings
 * interface of the R-TYPE game. It provides input boxes for IP and Port,
 * a return button, and navigation back to the main menu.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */


#ifndef CLIENT_GUI_SETTINGSSTATE_H
#define CLIENT_GUI_SETTINGSSTATE_H

#include "State.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {

/**
 * @class SettingsState
 * @brief The settings menu state of the R-TYPE game
 * 
 * SettingsState represents the interface for configuring network connection settings.
 * 
 * Features:
 * - Game title display ("SETTINGS")
 * - IP address input field with placeholder text and character limit
 * - Port input field with placeholder text and character limit
 * - Real-time text input with cursor animation
 * - Responsive layout that adapts to window resizing
 * - Interactive hover effects on the return button
 * - Return button to navigate back to the main menu
 * 
 * User Interaction Flow:
 * 1. Player sees the settings menu with title and two input fields
 * 2. Player can type an IP address and port number
 * 3. Player clicks "Return" to go back to the main menu
 * 
 * The state enforces input validation and transitions back to MainMenuState on return.
 */
class SettingsState : public State {
public:
    /**
     * @brief Construct a new SettingsState
     * @param stateManager Reference to the state manager for transitions
     */
    SettingsState(StateManager& stateManager);

    /**
     * @brief Handle SFML events (mouse, keyboard, text input)
     * @param event The SFML event to process
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Update animations and cursor blinking
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the settings UI to the window
     * @param window The render window to draw to
     */
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Called when entering this state
     */
    void onEnter() override;

    /**
     * @brief Called when exiting this state
     */
    void onExit() override;

private:
    // State management
    StateManager& stateManager;         ///< Reference to state manager for transitions

    // UI Text Elements
    sf::Text titleText;                 ///< "SETTINGS" title
    sf::Text box1Text;                  ///< IP address input display
    sf::Text box2Text;                  ///< Port input display
    sf::Text box1Hint;                  ///< Placeholder text for IP field
    sf::Text box2Hint;                  ///< Placeholder text for Port field
    sf::Text returnButton;              ///< "Return" button text

    // UI Visual Elements
    sf::RectangleShape box1Rect;        ///< Input field background for IP
    sf::RectangleShape box2Rect;        ///< Input field background for Port
    sf::RectangleShape returnButtonRect;///< Clickable area for return button

    // Input State
    std::string box1Value;              ///< Current IP address input
    std::string box2Value;              ///< Current port input
    bool typingBox1;                    ///< Whether IP field is active
    bool typingBox2;                    ///< Whether port field is active
    float cursorTimer;                  ///< Timer for cursor blinking animation
    bool showCursor;                    ///< Current cursor visibility state
};

} // namespace rtype::client::gui

#endif // CLIENT_GUI_SETTINGSSTATE_H
