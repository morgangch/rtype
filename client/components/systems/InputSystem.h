/**
 * @file InputSystem.h
 * @brief System for processing player input and updating controllable entities
 * 
 * This system handles keyboard input for player-controlled entities,
 * updating their velocity based on key states.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_INPUT_SYSTEM_H
#define CLIENT_SYSTEMS_INPUT_SYSTEM_H

#include <ECS/ECS.hpp>
#include <SFML/Graphics.hpp>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <client/components/Controllable.h>
#include <client/components/FireRate.h>
#include <cmath>

namespace rtype::client::systems {
    /**
     * @class InputSystem
     * @brief Processes player input for controllable entities
     * 
     * The InputSystem handles keyboard input and updates the velocity
     * of entities marked with the Controllable component. It supports:
     * - ZQSD + Arrow keys for movement
     * - Normalized diagonal movement
     * - Screen bounds checking
     * - Fire input handling
     * 
     * Required Components:
     * - Controllable: Marks entity as player-controlled
     * - Velocity: Updated based on input
     * - Position: For bounds checking
     * 
     * Optional Components:
     * - FireRate: For shooting mechanics
     */
    class InputSystem {
    public:
        /** @brief Screen width for bounds checking */
        float screenWidth;
        
        /** @brief Screen height for bounds checking */
        float screenHeight;
        
        /** @brief Fire key pressed state */
        bool fireKeyPressed;
        
        /**
         * @brief Constructor
         * @param width Screen width
         * @param height Screen height
         */
        InputSystem(float width = 1280.0f, float height = 720.0f)
            : screenWidth(width), screenHeight(height), fireKeyPressed(false) {}
        
        /**
         * @brief Process SFML events (for fire key tracking)
         * @param event SFML event to process
         */
        void handleEvent(const sf::Event& event) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                fireKeyPressed = true;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                fireKeyPressed = false;
            }
        }
        
        /**
         * @brief Update controllable entities (stub - input handled in GameState)
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void update(ECS::World& world, float deltaTime) {
            // Input is handled directly in GameState::update()
            // This is a stub for future refactoring
        }
        
        /**
         * @brief Reset input states (call when entering menus)
         */
        void resetInput() {
            fireKeyPressed = false;
        }
        
        /**
         * @brief Check if fire key is currently pressed
         * @return True if space bar is pressed
         */
        bool isFireKeyPressed() const {
            return fireKeyPressed;
        }
    };
}

#endif // CLIENT_SYSTEMS_INPUT_SYSTEM_H
