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
#include "SettingsConfig.h"
#include <SFML/Graphics.hpp>
#include "ParallaxSystem.h"
#include <memory>
#include <string>
#include <vector>

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

    /**
     * @brief Destroy the SettingsState
     *
     * Defined in the .cpp to avoid unique_ptr instantiation issues.
     */
    ~SettingsState();

private:
    // State management
    StateManager& stateManager;         ///< Reference to state manager for transitions
    SettingsConfig config;              ///< Configuration manager for loading/saving settings

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
    sf::Texture returnTexture;          ///< Texture for return button sprite
    sf::Sprite returnSprite;            ///< Sprite for return button
    bool returnSpriteLoaded{false};     ///< True if return texture loaded
    bool returnHovered{false};          ///< Hover state for return button
    // Removed hover background rectangle; keep only shrink-on-hover effect

    // Input State
    std::string box1Value;              ///< Current IP address input
    std::string box2Value;              ///< Current port input
    bool typingBox1;                    ///< Whether IP field is active
    bool typingBox2;                    ///< Whether port field is active
    float cursorTimer;                  ///< Timer for cursor blinking animation
    bool showCursor;                    ///< Current cursor visibility state

    // Keybinds UI and State
    enum KeybindAction {
        Up = 0, Down, Left, Right, Shoot, KeybindCount
    };
    sf::Text keybindTitleText;                  ///< "Keybinds" section title
    sf::Text keybindActionTexts[KeybindCount];  ///< Action labels (Up, Down, ...)
    sf::Text keybindKeyTexts[KeybindCount];     ///< Current key labels
    sf::RectangleShape keybindRects[KeybindCount]; ///< Clickable areas for keybinds
    int editingKeybind;                         ///< Index of keybind being edited (-1 if none)
    sf::Text keybindHintText;                   ///< Hint text when editing
    sf::Text resetKeybindsButton;               ///< "Reset to Default" button text
    sf::RectangleShape resetKeybindsRect;       ///< Clickable area for reset button
    sf::Keyboard::Key keybinds[KeybindCount];   ///< Current keybinds

    // IP/PORT UI Elements
    sf::Text ipPortTitleText;                   ///< "Network Settings" section title
    sf::Text ipLabelText;                       ///< "IP Address:" label
    sf::Text portLabelText;                     ///< "Port:" label

    // Daltonism UI Elements
    sf::Text daltonismTitleText;                ///< "Daltonism Mode" title
    sf::Text daltonismValueText;                ///< Current daltonism mode label
    std::vector<std::string> daltonismModes;    ///< List of available modes
    int currentDaltonismIndex{0};               ///< Current selected index
    /**
     * @name Parallax background (lazy-initialized)
     *
     * The settings screen displays the same multi-layer parallax background
     * used throughout the UI so visual context is consistent. The parallax
     * system is allocated on-demand so it can be sized to the active render
     * window. A semi-transparent black overlay (`m_overlay`) is drawn over
     * the parallax to improve contrast for text and controls.
     *
     * The destructor for this class is defined in the .cpp to ensure the
     * `std::unique_ptr<ParallaxSystem>` is destroyed where `ParallaxSystem`
     * is a complete type.
     * @{ */
    std::unique_ptr<ParallaxSystem> m_parallaxSystem; ///< Owned parallax system (created lazily)
    bool m_parallaxInitialized{false};                 ///< True after creation and sizing
    sf::RectangleShape m_overlay;                      ///< Semi-transparent overlay drawn above parallax

    /**
     * @brief Ensure the parallax system exists and is sized to the provided window.
     * @param window Render window used to size/create the parallax system
     */
    void ensureParallaxInitialized(const sf::RenderWindow& window);
    /** @} */
};

} // namespace rtype::client::gui

#endif // CLIENT_GUI_SETTINGSSTATE_H
