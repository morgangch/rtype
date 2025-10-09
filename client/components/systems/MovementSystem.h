/**
 * @file MovementSystem.h
 * @brief System for updating entity positions based on velocity
 * 
 * This system applies velocity to position components, handling the
 * basic physics movement for all entities.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_MOVEMENT_SYSTEM_H
#define CLIENT_SYSTEMS_MOVEMENT_SYSTEM_H

#include <ECS/ECS.hpp>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <cmath>
#include <vector>

namespace rtype::client::systems {
    /**
     * @class MovementSystem
     * @brief Updates entity positions based on their velocities
     * 
     * The MovementSystem iterates over all entities with both Position
     * and Velocity components, applying velocity to position over time.
     * 
     * Required Components:
     * - Position: Entity's current location
     * - Velocity: Entity's movement vector
     * 
     * Example usage:
     * @code
     * MovementSystem movementSystem;
     * movementSystem.update(world, deltaTime);
     * @endcode
     */
    class MovementSystem {
    public:
        /**
         * @brief Update all entity positions based on velocity
         * @param world The ECS world containing entities
         * @param deltaTime Time elapsed since last update (seconds)
         */
        void update(ECS::World& world, float deltaTime) {
            // Get all position components
            auto* positions = world.GetAllComponents<rtype::common::components::Position>();
            if (!positions) return;
            
            // Iterate through all position components
            for (auto& [entity, posPtr] : *positions) {
                auto& position = *posPtr;
                
                // Check if this entity also has velocity
                auto* vel = world.GetComponent<rtype::common::components::Velocity>(entity);
                if (!vel) continue;
                
                // Apply velocity to position
                position.x += vel->vx * deltaTime;
                position.y += vel->vy * deltaTime;
                
                // Optional: Clamp velocity to max speed
                float speed = std::sqrt(vel->vx * vel->vx + vel->vy * vel->vy);
                if (speed > vel->maxSpeed && vel->maxSpeed > 0.0f) {
                    float scale = vel->maxSpeed / speed;
                    vel->vx *= scale;
                    vel->vy *= scale;
                }
            }
        }
    };
}

#endif // CLIENT_SYSTEMS_MOVEMENT_SYSTEM_H
