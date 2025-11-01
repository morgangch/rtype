/**
 * @file PublicServerState.h
 * @brief Public server lobby state for the R-Type client GUI
 * 
 * This file defines the PublicServerState class, which handles the public
 * server lobby interface where players can quickly join games without
 * needing private server codes. The state provides a simple ready/not ready
 * toggle system for efficient matchmaking.
 * 
 * Key features:
 * - Simple ready state toggle for quick matchmaking
 * - Real-time player count display
 * - Automatic network connection to public servers
 * - Streamlined UI for casual play
 * - Instant game joining without codes
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#ifndef CLIENT_PUBLIC_SERVER_STATE_HPP
#define CLIENT_PUBLIC_SERVER_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "GUIHelper.h"
#include "ParallaxSystem.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    /**
     * @class PublicServerState
     * @brief Manages the public server matchmaking lobby
     * 
     * The PublicServerState provides a streamlined interface for public server
     * matchmaking. Unlike private servers that require codes, public servers
     * allow players to quickly join available games through a simple ready system.
     * 
     * **Key Features:**
     * - One-click ready/not ready toggle
     * - Real-time display of ready player count
     * - Automatic connection to public servers (room ID 0)
     * - Simple and intuitive interface for casual play
     * - Network integration with automatic disconnection
     * 
     * **User Flow:**
     * 1. Player enters public server lobby
     * 2. Automatic connection to public server
     * 3. Player toggles ready state
     * 4. Game starts when sufficient players are ready
     * 
     * Usage example:
     * @code
     * auto publicState = std::make_unique<PublicServerState>(stateManager, "PlayerName");
     * stateManager.changeState(std::move(publicState));
     * @endcode
     */
    class PublicServerState : public State {
    public:
        /**
         * @brief Constructs a new PublicServerState
         * @param stateManager Reference to the state manager
         * @param username The player's username for the lobby
         */
        PublicServerState(StateManager& stateManager, const std::string& username);
        
        /**
         * @brief Handles input events (mouse clicks, keyboard)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Updates the lobby state
         * @param deltaTime Time elapsed since last update (seconds)
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Renders the public server lobby interface
         * @param window The SFML window to render to
         */
        void render(sf::RenderWindow& window) override;
        
        /**
         * @brief Called when entering this state
         * Automatically connects to the public server
         */
        void onEnter() override;
        
        /**
         * @brief Called when exiting this state
         * Disconnects from the public server
         */
        void onExit() override;

        /**
         * @brief Destroy the PublicServerState
         *
         * Destructor defined in the .cpp to avoid instantiating unique_ptr
         * destructor in translation units where ParallaxSystem may be
         * incomplete.
         */
        ~PublicServerState();
        
    private:
        // Core references and configuration
        StateManager& stateManager;     ///< Reference to state manager for navigation
        std::string username;           ///< Player's username for lobby display and networking
        
        // UI Text Elements
        sf::Text playersReadyText;      ///< Displays "Amount of players ready: X"
        sf::Text readyButton;           ///< Deprecated: text hidden when using sprite
        sf::Text returnButton;          ///< "Return" button text for navigation back
        
        // UI Visual Elements
        sf::RectangleShape readyButtonRect;   ///< Clickable area for ready toggle button (sprite drawn instead)
        sf::RectangleShape returnButtonRect;  ///< Clickable area for return button
        sf::Texture returnTexture;      ///< Texture for return button sprite
        sf::Sprite returnSprite;        ///< Sprite for return button
        bool returnSpriteLoaded{false}; ///< True if return texture loaded
        bool returnHovered{false};      ///< Hover state for return button

        // Ready button sprite resources
        sf::Texture readyTexture;        ///< Texture for ready/start button sprite
        sf::Sprite readySprite;          ///< Sprite for ready/start button
        bool readySpriteLoaded{false};   ///< True if ready texture loaded
        bool readyHovered{false};        ///< Hover state for ready button
        
        // Matchmaking State Management
        bool isReady;                   ///< Current ready state of this player
        int playersReady;               ///< Total number of players marked as ready

        /**
         * @name Parallax background (lazy-initialized)
         *
         * The public server lobby shows a parallax background matching the
         * current game level. The parallax system is created on-demand so it can
         * be sized to the active render window. A semi-transparent black overlay
         * (`m_overlay`) is drawn on top of the parallax to keep the UI readable.
         *
         * The destructor for this class is defined in the .cpp so the
         * `std::unique_ptr<ParallaxSystem>` is destroyed in a translation unit
         * where `ParallaxSystem` is a complete type.
         * @{ */
        std::unique_ptr<ParallaxSystem> m_parallaxSystem; ///< Owned parallax system (lazy)
        bool m_parallaxInitialized{false};                 ///< True after creation & sizing
        sf::RectangleShape m_overlay;                      ///< Semi-transparent overlay for readability

        /**
         * @brief Ensure the parallax system exists and is sized to the provided window.
         * @param window Render window used to size/create the parallax
         */
        void ensureParallaxInitialized(const sf::RenderWindow& window);
        /** @} */
        
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
         * @param event The mouse button event for ready toggle and navigation
         */
        void handleMouseButtonEvent(const sf::Event& event);
        
        /**
         * @brief Handle mouse movement events for button hover effects
         * @param event The mouse move event for visual feedback
         */
        void handleMouseMoveEvent(const sf::Event& event);
        
        // Player Action Methods
        /**
         * @brief Toggle the ready state and update UI accordingly
         * Changes button text/color and updates player count display
         */
        void toggleReady();
        
        /**
         * @brief Update the ready player count display text
         * Refreshes both the count display and button text based on ready state
         */
        void updatePlayersReadyText();
    };
}

#endif // CLIENT_PUBLIC_SERVER_STATE_HPP
