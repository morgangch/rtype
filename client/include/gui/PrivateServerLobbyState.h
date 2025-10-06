/**
 * @file @file PrivateServerLobbyState.h
 * @brief Private server lobby state for the R-Type client GUI
 * 
 * This file defines the PrivateServerLobbyState class, which handles the lobby
 * interface for private servers where players wait for the game to start.
 * The state supports both admin and regular player roles with different
 * functionality for each.
 * 
 * Key features:
 * - Admin controls for starting games
 * - Player ready state management
 * - Real-time player count display
 * - Server code display for sharing
 * - Role-based UI interactions
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#ifndef CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
#define CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "GUIHelper.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    /**
     * @class PrivateServerLobbyState
     * @brief Manages the private server lobby interface
     * 
     * The PrivateServerLobbyState handles the lobby where players wait for a private
     * game to start. It supports two distinct user roles:
     * 
     * **Admin Role:**
     * - Can start the game when ready
     * - Has "Start Game" button instead of ready toggle
     * - Controls game initiation for all players
     * 
     * **Player Role:**
     * - Can toggle ready/not ready status
     * - Waits for admin to start the game
     * - Sees current ready player count
     * 
     * Key features:
     * - Server code display for easy sharing
     * - Real-time ready player count
     * - Role-based button behavior
     * - Network integration for player state
     * - Return to main menu functionality
     * 
     * Usage example:
     * @code
     * // Create lobby for admin
     * auto lobbyState = std::make_unique<PrivateServerLobbyState>(
     *     stateManager, "PlayerName", "1234", true);
     * 
     * // Create lobby for regular player
     * auto lobbyState = std::make_unique<PrivateServerLobbyState>(
     *     stateManager, "PlayerName", "1234", false);
     * @endcode
     */
    class PrivateServerLobbyState : public State {
    public:
        /**
         * @brief Constructs a new PrivateServerLobbyState
         * @param stateManager Reference to the state manager
         * @param username The player's username
         * @param serverCode The 4-digit server code for this lobby
         * @param isAdmin Whether this player is the server admin
         */
        PrivateServerLobbyState(StateManager& stateManager, const std::string& username, 
                               const std::string& serverCode, bool isAdmin);
        
        /**
         * @brief Handles input events (mouse, keyboard)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Updates the lobby state and animations
         * @param deltaTime Time elapsed since last update (seconds)
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Renders the lobby interface
         * @param window The SFML window to render to
         */
        void render(sf::RenderWindow& window) override;
        
        /**
         * @brief Called when entering this state
         * Logs lobby entry and player information
         */
        void onEnter() override;
        
    private:
        // Core references and configuration
        StateManager& stateManager;     ///< Reference to state manager for navigation
        std::string username;           ///< Player's username for display and networking
        std::string serverCode;         ///< 4-digit server code for this private lobby
        bool isAdmin;                   ///< Whether this player is the server administrator
        
        // UI Text Elements
        sf::Text playersReadyText;      ///< Displays current ready player count
        sf::Text actionButton;          ///< "Ready" for players, "Start Game" for admin
        sf::Text returnButton;          ///< "Return" button text for navigation back
        sf::Text serverCodeDisplay;     ///< Shows server code for sharing with others
        
        // UI Visual Elements
        sf::RectangleShape actionButtonRect;  ///< Clickable area for main action button
        sf::RectangleShape returnButtonRect;  ///< Clickable area for return button
        
        // Lobby State Management
        bool isReady;                   ///< Current ready state of this player
        int playersReady;               ///< Total number of players marked as ready
        
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
         * @brief Handle keyboard input events (Escape key for navigation)
         * @param event The keyboard event for navigation shortcuts
         */
        void handleKeyboardEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse button click events
         * @param event The mouse button event for role-based actions
         */
        void handleMouseButtonEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse movement events for button hover effects
         * @param event The mouse move event for visual feedback
         */
        void handleMouseMoveEvent(const sf::Event& event);
        
        // Player Action Methods
        /**
         * @brief Toggle the ready state for regular players
         * Updates player count and button appearance
         */
        void toggleReady();
        
        /**
         * @brief Start the game (admin only)
         * Initiates game start sequence for all players in lobby
         */
        void startGame();
        
        // UI Update Methods
        /**
         * @brief Update the ready player count display text
         * Refreshes the "Amount of players ready: X" text
         */
        void updatePlayersReadyText();
        
        /**
         * @brief Update the action button text and appearance
         * Changes button text/color based on player role and ready state
         */
        void updateActionButton();
    };
}

#endif // CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
