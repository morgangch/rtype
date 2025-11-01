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
         * @brief Renders a vessel card with preview, name, and stats
         * @param window The SFML window to render to
         * @param vesselType The vessel type to render
         * @param position Position of the card center
         * @param isSelected Whether this vessel is currently selected
         * @param isHovered Whether this vessel is currently hovered
         */
        void renderVesselCard(
            sf::RenderWindow& window,
            rtype::common::components::VesselType vesselType,
            const sf::Vector2f& position,
            bool isSelected,
            bool isHovered
        );
        
        /**
         * @brief Renders a stat bar with label and value
         * @param window The SFML window to render to
         * @param position Position of the stat bar
         * @param label Label text (e.g., "Speed")
         * @param value Normalized value (0.0 - 2.0)
         * @param color Color of the bar
         */
        void renderStatBar(
            sf::RenderWindow& window,
            const sf::Vector2f& position,
            const std::string& label,
            float value,
            const sf::Color& color
        );

        StateManager& stateManager;
        std::string username;
        std::string serverIp;
        int serverPort;
        uint32_t roomCode;
        
        rtype::common::components::VesselType selectedVessel;
        int hoveredVesselIndex; // -1 if none
        
        // UI elements
        sf::Font font;
        sf::Text titleText;
        sf::Text confirmButtonText;
        sf::Text backButtonText;
        sf::RectangleShape confirmButton;
        sf::RectangleShape backButton;
        
        // Parallax background
        std::unique_ptr<ParallaxSystem> parallax;
        
        // Vessel card positions (4 vessels in 2x2 grid)
        std::array<sf::Vector2f, 4> vesselCardPositions;
        
        // Animation
        float animationTime;
        
        // Vessel preview sprites
        std::array<sf::Sprite, 4> vesselSprites;
        std::array<sf::Texture, 4> vesselTextures;
        
        bool confirmHovered;
        bool backHovered;
    };
}

#endif // CLIENT_VESSEL_SELECTION_STATE_HPP
