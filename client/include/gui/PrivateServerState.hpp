/**
 * @file PrivateServerState.hpp
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

#include "State.hpp"
#include "StateManager.hpp"
#include "GUIHelper.hpp"
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
        
    private:
        StateManager& stateManager;
        std::string username;
        
        // UI Elements
        sf::Text titleText;
        sf::Text serverCodeText;
        sf::Text serverCodeHintText;
        sf::RectangleShape serverCodeBox;
        sf::Text joinButton;
        sf::RectangleShape joinButtonRect;
        sf::Text createButton;
        sf::RectangleShape createButtonRect;
        sf::Text returnButton;
        sf::RectangleShape returnButtonRect;
        
        // State
        std::string serverCode;
        bool isTyping;
        float cursorTimer;
        bool showCursor;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        
        // Event handling methods
        void handleResizeEvent(const sf::Event& event);
        void handleKeyboardEvent(const sf::Event& event);
        void handleMouseButtonEvent(const sf::Event& event);
        void handleTextInputEvent(const sf::Event& event);
        void handleMouseMoveEvent(const sf::Event& event);
        
        // Action methods
        void joinServer();
        void createServer();
    };
}

#endif // CLIENT_PRIVATE_SERVER_STATE_HPP
