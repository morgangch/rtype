/**
 * @file MovementSystem.h
 * @brief Movement system for updating entity positions based on velocity
 * 
 * This system is shared between client and server for consistent physics.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_MOVEMENT_SYSTEM_H
#define COMMON_SYSTEMS_MOVEMENT_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>

namespace rtype::common::systems {
    /**
     * @class MovementSystem
     * @brief Updates positions based on velocity
     * 
     * This system is used by both client (for prediction) and server (authoritative).
     * Iterates over all entities with Position and Velocity components and updates
     * their position based on velocity and deltaTime.
     */
    class MovementSystem {
    public:
        /**
         * @brief Update all entities with Position and Velocity
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        static void update(ECS::World& world, float deltaTime) {
            auto* positions = world.GetAllComponents<components::Position>();
            auto* velocities = world.GetAllComponents<components::Velocity>();
            
            if (!positions || !velocities) return;
            
            // Iterate over all positions
            for (auto& [entity, posPtr] : *positions) {
                auto& pos = *posPtr;
                auto* vel = world.GetComponent<components::Velocity>(entity);
                
                if (!vel) continue;
                
                // Update position based on velocity
                pos.x += vel->vx * deltaTime;
                pos.y += vel->vy * deltaTime;
                
                // Optional: clamp to max speed
                if (vel->maxSpeed > 0.0f) {
                    float speed = std::sqrt(vel->vx * vel->vx + vel->vy * vel->vy);
                    if (speed > vel->maxSpeed) {
                        float scale = vel->maxSpeed / speed;
                        vel->vx *= scale;
                        vel->vy *= scale;
                    }
                }
            }
        }
    };
}

#endif // COMMON_SYSTEMS_MOVEMENT_SYSTEM_H
