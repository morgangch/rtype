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
#include <common/components/Collision.h>
#include <common/components/EnemyType.h>
#include <common/utils/Config.h>
#include <cmath>

namespace rtype::common::systems {
    /**
     * @class MovementSystem
     * @brief Updates positions based on velocity
     *
     * This system is used by both client (for prediction) and server (authoritative).
     * Iterates over all entities with Position and Velocity components and updates
     * their position based on velocity and deltaTime.
     *
     * Also handles special movement behaviors like boss bounce logic.
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

                // Handle special enemy behaviors
                handleEnemyMovement(world, entity, pos, vel);

                // Clamp velocity to max speed
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

    private:
        /**
         * @brief Handle special movement behaviors for enemies
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleEnemyMovement(ECS::World& world, ECS::EntityID entity,
                                       components::Position& pos, components::Velocity* vel) {
            auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
            if (!enemyType) return;

            // TankDestroyer boss: vertical bounce
            if (enemyType->type == components::EnemyType::TankDestroyer) {
                handleTankDestroyerBounce(world, entity, pos, vel);
            }

            // Snake enemy: sine wave pattern (can be added later)
            // if (enemyType->type == components::EnemyType::Snake) {
            //     handleSnakeMovement(world, entity, pos, vel);
            // }
        }

        /**
         * @brief Handle TankDestroyer boss bounce at screen boundaries
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleTankDestroyerBounce(ECS::World& world, ECS::EntityID entity,
                                              components::Position& pos, components::Velocity* vel) {
            if (!vel) return;

            auto* collision = world.GetComponent<components::Collision>(entity);
            if (!collision) return;

            float halfHeight = collision->height / 2.0f;

            // Bounce at top of screen
            if (pos.y - halfHeight <= 0.0f) {
                pos.y = halfHeight;
                vel->vy = std::abs(vel->vy);  // Go down
            }
            // Bounce at bottom of screen
            else if (pos.y + halfHeight >= Config::SCREEN_HEIGHT) {
                pos.y = Config::SCREEN_HEIGHT - halfHeight;
                vel->vy = -std::abs(vel->vy);  // Go up
            }
        }
    };
}

#endif // COMMON_SYSTEMS_MOVEMENT_SYSTEM_H