/**
 * @file PrivateServerLobbyState.h
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
#include "ParallaxSystem.h"
#include <memory>
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
     * - Waits for admin to start the game
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

        /**
         * @brief Destroy the PrivateServerLobbyState
         *
         * Destructor defined in the .cpp so unique_ptr<ParallaxSystem>
         * is destroyed where ParallaxSystem is a complete type.
         */
        ~PrivateServerLobbyState();
        
    private:
        // Core references and configuration
        StateManager& stateManager;     ///< Reference to state manager for navigation
        std::string username;           ///< Player's username for display and networking
        std::string serverCode;         ///< 4-digit server code for this private lobby
        bool isAdmin;                   ///< Whether this player is the server administrator
        
        // UI Text Elements
        sf::Text playersWaitingText;    ///< "Waiting for room host" for non-admin players
        sf::Text actionButton;          ///< Deprecated: text hidden when using sprite (admin only)
        sf::Text returnButton;          ///< "Return" button text for navigation back
        sf::Text serverCodeDisplay;     ///< Shows server code for sharing with others
        
        // UI Visual Elements
    sf::RectangleShape actionButtonRect;  ///< Clickable area for start game button (sprite drawn instead)
        sf::RectangleShape returnButtonRect;  ///< Clickable area for return button
    // Return sprite resources (replaces text)
    sf::Texture returnTexture;      ///< Texture for return button sprite
    sf::Sprite returnSprite;        ///< Sprite for return button
    bool returnSpriteLoaded{false}; ///< True if return texture loaded
    bool returnHovered{false};      ///< Hover state for return button

        // Start game (admin) button sprite resources
        sf::Texture actionTexture;       ///< Texture for start game sprite (uses Ready.png)
        sf::Sprite actionSprite;         ///< Sprite for start game
        bool actionSpriteLoaded{false};  ///< True if action texture loaded
        bool actionHovered{false};       ///< Hover state for start game
        bool actionPressed{false};       ///< Mouse is pressed on the start button (for glow)
                
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
         * @brief Start the game (admin only)
         * Initiates game start sequence for all players in lobby
         */
        void startGame();
        
        // UI Update Methods
        /**
         * @brief Update the waiting text display
         * Shows "Waiting for room host" for non-admin players
         */
        void updateWaitingText();
        
        /**
         * @brief Update the action button text and appearance (admin only)
         * Changes button to "Start Game" for admin players
         */
        void updateActionButton();

    private:
        /**
         * @name Parallax background (lazy-initialized)
         *
         * Multi-theme parallax background shown behind the lobby UI. The
         * system is created lazily so it can be sized to the active render
         * window. A semi-transparent black overlay (m_overlay) is drawn on
         * top of the parallax to keep text and buttons readable.
         *
         * Note: the class destructor is defined out-of-line in the .cpp so
         * the std::unique_ptr<ParallaxSystem> destructor is instantiated in a
         * translation unit where ParallaxSystem is a complete type.
         * @{ */
        std::unique_ptr<ParallaxSystem> m_parallaxSystem; ///< Owned parallax system (created on demand)
        bool m_parallaxInitialized{false};                 ///< True after creation & sizing
        sf::RectangleShape m_overlay;                      ///< Semi-transparent overlay to improve contrast

        /**
         * @brief Ensure the parallax system exists and is sized to the provided window.
         * @param window Render window used to size/create the parallax
         */
        void ensureParallaxInitialized(const sf::RenderWindow& window);
        /** @} */

        /**
         * @name Lobby Settings (Admin-only)
         * Panel accessible via a settings gear in the bottom-left.
         * Contains three columns: Gameplay, AI, Cheats.
         * - Gameplay: Difficulty (cycles Easy/Normal/Hard), Friendly fire toggle (cosmetic)
         * - AI: AI assist toggle (visible only when exactly one player is in the lobby)
         * - Cheats: Mega damage (+1000 dmg) (applies to admin only)
         * @{ */
        // Access icon (bottom-left)
        sf::Texture m_settingsTexture;
        sf::Sprite  m_settingsSprite;
        bool        m_settingsSpriteLoaded{false};
        bool        m_settingsHovered{false};
        sf::RectangleShape m_settingsRect; // fallback/click zone and hover hit area

        // Panel visibility and background
        bool m_showSettings{false};
        sf::RectangleShape m_settingsPanelRect;

        // Section titles
        sf::Text m_gameplayTitle;
        sf::Text m_aiTitle;
        sf::Text m_cheatsTitle;

        // Gameplay controls
        sf::Text m_difficultyLabel;
        sf::Text m_difficultyValue;
        sf::RectangleShape m_rectDifficulty; // invisible clickable area

        sf::Text m_friendlyFireLabel;
        sf::RectangleShape m_rectFriendlyFire; // invisible clickable area

        // AI controls (shown only when exactly 1 player in lobby)
        sf::Text m_aiAssistLabel;
        sf::RectangleShape m_rectAIAssist; // invisible clickable area
        sf::RectangleShape m_sqAIAssist;   // visual toggle square

        // Cheats controls
        sf::Text m_megaDamageLabel;
        sf::RectangleShape m_rectMegaDamage; // invisible clickable area
        sf::RectangleShape m_sqMegaDamage;   // visual toggle square

        // Settings values (cosmetic except cheats to be applied to admin later)
        int  m_difficultyIndex{1}; // 0=Easy, 1=Normal, 2=Hard
        bool m_friendlyFire{false};
        bool m_aiAssist{true};
        bool m_megaDamage{false};
        // Visual squares for toggles
        sf::RectangleShape m_sqFriendlyFire;

        // Track players to control AI section visibility
        uint32_t m_totalPlayersInLobby{1};

        // Helpers
        void renderSettingsPanel(sf::RenderWindow& window);
        void updateSettingsTexts();
        void updateSettingsLayout(const sf::Vector2u& windowSize);
        /** @} */
        
    public:
        /**
         * @brief Update lobby state from server broadcast
         * @param totalPlayers Total number of players in the lobby
         */
        void updateFromServer(uint32_t totalPlayers);
    };
    
    // Global pointer to current lobby state for network callbacks
    extern PrivateServerLobbyState* g_lobbyState;
}

#endif // CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
