/**
 * @file PrivateServerLobbyState.hpp
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

#include "State.hpp"
#include "StateManager.hpp"
#include "GUIHelper.hpp"
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
        StateManager& stateManager;
        std::string username;
        std::string serverCode;
        bool isAdmin;
        
        // UI Elements
        sf::Text playersReadyText;
        sf::Text actionButton; // "Ready" for players, "Start" for admin
        sf::RectangleShape actionButtonRect;
        sf::Text returnButton;
        sf::RectangleShape returnButtonRect;
        sf::Text serverCodeDisplay;
        
        // State
        bool isReady;
        int playersReady;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        void toggleReady();
        void startGame();
        void updatePlayersReadyText();
        void updateActionButton();
    };
}

#endif // CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
