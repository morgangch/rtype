/**
 * @file PlayerInputSystem.h
 * @brief System for processing player input and movement
 * 
 * This system translates keyboard input into player velocity changes.
 * It's shared between client and server for input prediction/validation.
 * 
 * @author R-TYPE Development Team
 * @date 2025-10-15
 */

#ifndef COMMON_SYSTEMS_PLAYER_INPUT_SYSTEM_H
#define COMMON_SYSTEMS_PLAYER_INPUT_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Player.h>

#include <functional>
#include <cmath>

namespace rtype::common::systems {
    /**
     * @class PlayerInputSystem
     * @brief Processes player keyboard input and updates velocity
     * 
     * This system handles player movement based on keyboard input.
     * It supports callbacks for client-specific actions (animations, etc.).
     */
    class PlayerInputSystem : public ECS::System {
    public:
        /**
         * @brief Animation callback (optional, client-side)
         * Parameters: entityID, isMoving
         */
        using AnimationCallback = std::function<void(ECS::EntityID, bool)>;
        
    private:
        bool m_keyUp;
        bool m_keyDown;
        bool m_keyLeft;
        bool m_keyRight;
        AnimationCallback m_animCallback;
        
    public:
        /**
         * @brief Constructor
         */
        PlayerInputSystem() 
            : ECS::System("PlayerInputSystem", 20)
            , m_keyUp(false)
            , m_keyDown(false)
            , m_keyLeft(false)
            , m_keyRight(false) {}
        
        /**
         * @brief Set current input state
         * @param up Up key pressed
         * @param down Down key pressed
         * @param left Left key pressed
         * @param right Right key pressed
         */
        void SetInputState(bool up, bool down, bool left, bool right) {
            m_keyUp = up;
            m_keyDown = down;
            m_keyLeft = left;
            m_keyRight = right;
        }
        
        /**
         * @brief Register animation callback (optional)
         * @param callback Function to call for animation updates
         */
        void RegisterAnimationCallback(AnimationCallback callback) {
            m_animCallback = callback;
        }
        
        /**
         * @brief Update player movement based on input
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            auto* players = world.GetAllComponents<components::Player>();
            if (!players) return;
            
            for (auto& [entity, playerPtr] : *players) {
                auto* vel = world.GetComponent<components::Velocity>(entity);
                auto* pos = world.GetComponent<components::Position>(entity);
                
                if (!vel || !pos) continue;
                
                // Calculate movement direction from input
                float moveX = 0.0f, moveY = 0.0f;
                
                if (m_keyUp) moveY -= 1.0f;
                if (m_keyDown) moveY += 1.0f;
                if (m_keyLeft) moveX -= 1.0f;
                if (m_keyRight) moveX += 1.0f;
                
                // Normalize diagonal movement (prevent faster diagonal speed)
                float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
                if (magnitude > 0.0f) {
                    moveX /= magnitude;
                    moveY /= magnitude;
                }
                
                // Apply movement to velocity
                vel->vx = moveX * vel->maxSpeed;
                vel->vy = moveY * vel->maxSpeed;
                
                // Trigger animation callback if registered (client-side)
                if (m_animCallback) {
                    bool isMoving = magnitude > 0.0f;
                    m_animCallback(entity, isMoving);
                }
            }
        }
        
        /**
         * @brief Initialize the system
         * @param world The ECS world
         */
        void Initialize(ECS::World& world) override {
            // Optional: Add initialization logic
        }
        
        /**
         * @brief Cleanup the system
         * @param world The ECS world
         */
        void Cleanup(ECS::World& world) override {
            // Optional: Add cleanup logic
            m_animCallback = nullptr;
        }
    };
}

#endif // COMMON_SYSTEMS_PLAYER_INPUT_SYSTEM_H
