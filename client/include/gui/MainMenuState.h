/**
 * @file MainMenuState.h
 * @brief Main menu screen implementation for the R-TYPE client
 * 
 * This file contains the MainMenuState class which implements the main menu
 * interface of the R-TYPE game. It provides username input, server selection
 * buttons, and navigation to other game states.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_MAIN_MENU_STATE_HPP
#define CLIENT_MAIN_MENU_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "GUIHelper.h"
#include "ParallaxSystem.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "MusicManager.h"
#include <memory>

namespace rtype::client::gui {
    
    /**
     * @class MainMenuState
     * @brief The main menu state of the R-TYPE game
     * 
     * MainMenuState represents the primary interface that players see when starting
     * the game. It provides the following functionality:
     * 
     * Features:
     * - Game title display ("THE TOP R-TYPE")
     * - Username input field with placeholder text
     * - Automatic username generation if none provided
     * - Public servers button (connects to public server browser)
     * - Private servers button (connects to private server management)
     * - Responsive layout that adapts to window resizing
     * - Interactive hover effects on buttons
     * - Real-time text input with cursor animation
     * 
     * User Interaction Flow:
     * 1. Player sees the main menu with title and input field
     * 2. Player can type a username or leave empty for auto-generation
     * 3. Player clicks "Public servers" to browse public games
     * 4. Player clicks "Private servers" to create/join private games
     * 
     * The state automatically transitions to PublicServerState or PrivateServerState
     * based on user selection, passing the username for network identification.
     */
    class MainMenuState : public State {
    public:
        /**
         * @brief Construct a new MainMenuState
         * @param stateManager Reference to the state manager for transitions
         */
        MainMenuState(StateManager& stateManager);

        /**
         * @brief Destroy the MainMenuState
         *
         * Defined out-of-line in the .cpp to avoid requiring the full
         * definition of ParallaxSystem in translation units that only
         * forward-declare MainMenuState (fixes unique_ptr destructor error).
         */
        ~MainMenuState();
        
        /**
         * @brief Handle input events (mouse clicks, text input, window resize)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Update animations and cursor blinking
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Render the main menu to the screen
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
         * @brief Get the current username
         * @return Const reference to the username string
         */
        const std::string& getUsername() const { return username; }

        /**
         * @brief Set the parallax theme according to a level index
         * @param levelIndex 0 = level1 (space default), 1 = level2 (hallway), ...
         */
        void setParallaxThemeFromLevel(int levelIndex);
        
    private:
        StateManager& stateManager;     ///< Reference to state manager for transitions
        MusicManager m_musicManager;    ///< Local music manager for menu music
        
    // UI Text Elements
    sf::Text titleText;             ///< "THE TOP R-TYPE" title
    sf::Text usernameText;          ///< User-entered username display
    sf::Text usernameHintText;      ///< Placeholder text for username field
    sf::Text publicServersButton;   ///< "Public servers" button text
    sf::Text privateServersButton;  ///< "Private servers" button text
    sf::Text settingsButtonText;    ///< "Settings" button text

    // UI Visual Elements
    sf::RectangleShape usernameBox;     ///< Input field background
    sf::RectangleShape publicButtonRect;   ///< Public servers button clickable area
    sf::RectangleShape privateButtonRect;  ///< Private servers button clickable area
    sf::RectangleShape settingsButtonRect; ///< Clickable area for settings button
        
        // Input State
        std::string username;           ///< Current username input
        bool isTyping;                  ///< Whether the input field is active
        float cursorTimer;              ///< Timer for cursor blinking animation
        bool showCursor;                ///< Current cursor visibility state
        
        // Helper methods
        /**
         * @brief Initialize all UI elements with fonts, colors, and initial text
         */
        void setupUI();
        
        /**
         * @brief Update element positions based on window size
         * @param windowSize Current window dimensions
         */
        void updateLayout(const sf::Vector2u& windowSize);

        /**
         * Parallax background system shown behind the menu
         * Lazily created so we can size it to the actual window.
         */
        std::unique_ptr<ParallaxSystem> m_parallaxSystem;
        bool m_parallaxInitialized{false};

        /**
         * Semi-transparent overlay drawn on top of the parallax to
         * keep menu UI readable.
         */
        sf::RectangleShape m_overlay;

        /**
         * Ensure the parallax system exists and is sized to the provided window.
         */
        void ensureParallaxInitialized(const sf::RenderWindow& window);
        
        // Event handling methods
        /**
         * @brief Handle window resize events
         * @param event The resize event
         */
        void handleResizeEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse button press events
         * @param event The mouse button event
         */
        void handleMouseButtonEvent(const sf::Event& event);
        
        /**
         * @brief Handle text input events
         * @param event The text input event
         */
        void handleTextInputEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse movement events for hover effects
         * @param event The mouse move event
         */
        void handleMouseMoveEvent(const sf::Event& event);
        
        /**
         * @brief Handle key press events (including debug mode F1)
         * @param event The key press event
         */
        void handleKeyPressEvent(const sf::Event& event);
        
        // Button action handlers
        /**
         * @brief Handle public servers button click
         * Transitions to PublicServerState with current username
         */
        void onPublicServersClick();
        
        /**
         * @brief Handle private servers button click
         * Transitions to PrivateServerState with current username
         */
        void onPrivateServersClick();
    };
}

#endif // CLIENT_MAIN_MENU_STATE_HPP
