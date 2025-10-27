/**
 * @file PrivateServerState.h
 * @brief Private server connection state for the R-Type client GUI
 * 
 * This file defines the PrivateServerState class, which handles the interface
 * for connecting to private servers. Users can either join an existing private
 * server using a 4-digit code or create a new private server as an admin.
 * 
 * Key features:
 * - Server code input with validation (4-digit numbers 1000-9999)
 * - Join existing private server functionality
 * - Create new private server with random code generation
 * - Real-time input validation and user feedback
 * - Animated text cursor for input fields
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#ifndef CLIENT_PRIVATE_SERVER_STATE_HPP
#define CLIENT_PRIVATE_SERVER_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "GUIHelper.h"
#include "SettingsConfig.h"
#include "ParallaxSystem.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    /**
     * @class PrivateServerState
     * @brief Manages the private server connection interface
     * 
     * The PrivateServerState provides an interface for users to connect to private
     * servers in two ways:
     * 
     * **Join Existing Server:**
     * - Enter a 4-digit server code (1000-9999)
     * - Real-time input validation
     * - Connect as a regular player
     * 
     * **Create New Server:**
     * - Generate random 4-digit server code
     * - Become the server admin
     * - Automatically transition to lobby
     * 
     * The state features:
     * - Animated text cursor for input fields
     * - Input validation with user feedback
     * - Network integration for server connection
     * - Smooth state transitions to lobby
     * - Responsive UI layout
     * 
     * Usage example:
     * @code
     * auto privateState = std::make_unique<PrivateServerState>(stateManager, "PlayerName");
     * stateManager.changeState(std::move(privateState));
     * @endcode
     */
    class PrivateServerState : public State {
    public:
        /**
         * @brief Constructs a new PrivateServerState
         * @param stateManager Reference to the state manager
         * @param username The player's username for server connection
         */
        PrivateServerState(StateManager& stateManager, const std::string& username);
        
        /**
         * @brief Handles input events (text input, mouse, keyboard)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Updates animations and input cursor
         * @param deltaTime Time elapsed since last update (seconds)
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Renders the private server interface
         * @param window The SFML window to render to
         */
        void render(sf::RenderWindow& window) override;
        
        /**
         * @brief Called when entering this state
         * Logs entry and initializes the interface
         */
        void onEnter() override;
        
        /**
         * @brief Called when exiting this state
         * Performs cleanup (note: doesn't disconnect as we might be transitioning to lobby)
         */
        void onExit() override;

        /**
         * @brief Destroy the PrivateServerState
         *
         * Destructor defined in the .cpp so unique_ptr destructor is instantiated
         * where ParallaxSystem is complete.
         */
        ~PrivateServerState();
        
    private:
        // Core references and configuration
        StateManager& stateManager;     ///< Reference to state manager for navigation
        std::string username;           ///< Player's username for server connection
        SettingsConfig config;          ///< Settings configuration for network parameters
        
        // UI Text Elements
        sf::Text titleText;             ///< "Private Servers" title display
        sf::Text serverCodeText;        ///< User-entered server code display
        sf::Text serverCodeHintText;    ///< Placeholder text "Enter server code (1000-9999)"
        sf::Text joinButton;            ///< "Join Server" button text
        sf::Text createButton;          ///< "Create New Server" button text
        sf::Text returnButton;          ///< "Return" button text for navigation back
        
        // UI Visual Elements
        sf::RectangleShape serverCodeBox;    ///< Input field background for server code
        sf::RectangleShape joinButtonRect;   ///< Clickable area for join server button
        sf::RectangleShape createButtonRect; ///< Clickable area for create server button
        sf::RectangleShape returnButtonRect; ///< Clickable area for return button
        
        // Input State Management
        std::string serverCode;         ///< Current server code input (max 4 digits)
        bool isTyping;                  ///< Whether the input field is currently active
        float cursorTimer;              ///< Timer for cursor blinking animation
        bool showCursor;                ///< Current cursor visibility state
        
        // UI Management Methods
        /**
         * @brief Initialize all UI elements with fonts, colors, and positioning
         */
        void setupUI();
        
        /**
         * @brief Update element positions based on window size
         * @param windowSize Current window dimensions for responsive layout
         */
        void updateLayout(const sf::Vector2u& windowSize);
        
        // Event Handling Methods
        /**
         * @brief Handle window resize events
         * @param event The resize event containing new dimensions
         */
        void handleResizeEvent(const sf::Event& event);
        
        /**
         * @brief Handle keyboard input events (Escape key)
         * @param event The keyboard event for navigation shortcuts
         */
        void handleKeyboardEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse button click events
         * @param event The mouse button event for UI interactions
         */
        void handleMouseButtonEvent(const sf::Event& event);
        
        /**
         * @brief Handle text input events for server code entry
         * @param event The text input event with numeric validation
         */
        void handleTextInputEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse movement events for button hover effects
         * @param event The mouse move event for visual feedback
         */
        void handleMouseMoveEvent(const sf::Event& event);
        
        // Server Connection Methods
        /**
         * @brief Attempt to join an existing private server
         * Validates server code and connects to specified private lobby
         */
        void joinServer();
        
        /**
         * @brief Create a new private server
         * Generates random server code and creates new private lobby as admin
         */
        void createServer();
        
        /**
         * @brief Get validated port number from config with fallback
         * @return Valid port number (1-65535), or default 4242 if invalid
         * 
         * Parses port from config and validates range. Returns default port
         * if parsing fails or value is out of valid range.
         */
        int getValidatedPort();

        /**
         * @name Parallax background (lazy-initialized)
         *
         * The menu displays a multi-layer parallax background matching the
         * current game level. The parallax system is created on-demand so it
         * can be sized to the active render window. A semi-transparent black
         * overlay (`m_overlay`) is drawn on top of the parallax to keep the
         * input fields and buttons readable.
         *
         * The destructor for this class is defined in the .cpp so that the
         * `std::unique_ptr<ParallaxSystem>` is destroyed in a translation
         * unit where `ParallaxSystem` is a complete type.
         * @{ */
        std::unique_ptr<ParallaxSystem> m_parallaxSystem; ///< Owned parallax system (created lazily)
        bool m_parallaxInitialized{false};                 ///< True after creation and sizing
        sf::RectangleShape m_overlay;                      ///< Semi-transparent dark overlay for readability

        /**
         * @brief Ensure the parallax system exists and is sized to the provided window.
         * @param window Render window used to size/create the parallax system
         */
        void ensureParallaxInitialized(const sf::RenderWindow& window);
        /** @} */
    };
}

#endif // CLIENT_PRIVATE_SERVER_STATE_HPP
