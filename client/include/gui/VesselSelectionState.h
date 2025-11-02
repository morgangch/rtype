/**
 * @file VesselSelectionState.h
 * @brief Vessel selection state before joining a game room
 * 
 * This file defines the VesselSelectionState class, which provides an interface
 * for players to choose their vessel class before joining a game server.
 * 
 * Key features:
 * - Display all 4 vessel classes with animated sprites
 * - Show vessel stats comparison (speed, damage, defense, fire rate)
 * - Display weapon information (normal and charged shots)
 * - Visual selection with hover effects
 * - Confirm selection to proceed with server connection
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#ifndef CLIENT_VESSEL_SELECTION_STATE_HPP
#define CLIENT_VESSEL_SELECTION_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "GUIHelper.h"
#include "SettingsConfig.h"
#include "ParallaxSystem.h"
#include "components/VesselClass.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include <string>
#include <array>

namespace rtype::client::gui {
    /**
     * @class VesselSelectionState
     * @brief Manages the vessel class selection interface before joining a game
     * 
     * The VesselSelectionState displays all available vessel classes and allows
     * the player to choose one before connecting to the game server.
     * 
     * Vessel Classes:
     * - Crimson Striker (Balanced)
     * - Azure Phantom (Speed)
     * - Emerald Titan (Power)
     * - Solar Guardian (Defense)
     * 
     * Features:
     * - Visual preview of each vessel with animated sprites
     * - Stats comparison bars (speed, damage, defense, fire rate)
     * - Weapon information display (normal + charged modes)
     * - Hover effects and visual feedback
     * - Confirm button to proceed with selection
     * 
     * Usage:
     * @code
     * auto vesselState = std::make_unique<VesselSelectionState>(
     *     stateManager, username, serverIp, serverPort, roomCode
     * );
     * stateManager.changeState(std::move(vesselState));
     * @endcode
     */
    class VesselSelectionState : public State {
    public:
        /**
         * @brief Constructs a new VesselSelectionState
         * @param stateManager Reference to the state manager
         * @param username The player's username
         * @param serverIp IP address of the server to join
         * @param serverPort Port of the server to join
         * @param roomCode Room code to join (0 for create new)
         */
        VesselSelectionState(
            StateManager& stateManager,
            const std::string& username,
            const std::string& serverIp,
            int serverPort,
            uint32_t roomCode
        );
        
        /**
         * @brief Handles input events (mouse clicks, keyboard)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Updates animations and hover states
         * @param deltaTime Time elapsed since last update (seconds)
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Renders the vessel selection interface
         * @param window The SFML window to render to
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
        /**
         * @brief Confirms vessel selection and proceeds to join server
         */
        void confirmSelection();
        
        /**
         * @brief Returns to previous state (cancel selection)
         */
        void goBack();
        
        /**
         * @brief Compute responsive positions and sizes for UI elements.
         * @param size Current render target dimensions (in pixels).
         * @details Called each frame to adapt the layout to the window size.
         */
        void layout(const sf::Vector2u& size);

        /**
         * @brief Render a single vessel row.
         * @param window The window to draw into.
         * @param i Index of the row to render (0..3).
         */
        void renderRow(sf::RenderWindow& window, int i);

        StateManager& stateManager;
        std::string username;
        std::string serverIp;
        int serverPort;
        uint32_t roomCode;
        
        /** Currently selected vessel type (maps to row index 0..3). */
        rtype::common::components::VesselType selectedVessel;
        /** Index of the row currently hovered by the mouse, or -1 if none. */
        int hoveredRowIndex; // -1 if none

        // UI elements
        sf::Font font;
        sf::Text titleText;

        // Parallax background and dark overlay (menu style)
        std::unique_ptr<ParallaxSystem> parallax;
        sf::RectangleShape m_overlay;

        // Top-left return button sprite and clickable rect
        sf::Texture m_returnTexture;
        sf::Sprite m_returnSprite;
        sf::FloatRect m_returnRect; // clickable area
        bool m_returnSpriteLoaded{false};
        bool m_returnHovered{false};

        // Row visuals (4 rows)
        sf::Texture rowSharedTexture; // single sheet with color rows
        std::array<sf::Sprite, 4> rowSprites;
        std::array<sf::RectangleShape, 4> rowBackgrounds;

        // Ready button per row (sprite like lobby ready)
        sf::Texture m_readyTexture;
        std::array<sf::Sprite, 4> rowReadySprites;
        std::array<bool, 4> rowReadyHovered{{false,false,false,false}};

        // Row text blocks
        std::array<sf::Text, 4> rowName;
        std::array<sf::Text, 4> rowRole;
        std::array<sf::Text, 4> rowShoot;
        std::array<sf::Text, 4> rowCharged;
        std::array<sf::Text, 4> rowSpeed;
        std::array<sf::Text, 4> rowDefense;
        std::array<sf::Text, 4> rowFireRate;

        // Animation (for minor hover/selection effects)
        float animationTime;
    };
}

#endif // CLIENT_VESSEL_SELECTION_STATE_HPP
