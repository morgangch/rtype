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
#include <common/components/Projectile.h>
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

            // Update enemy lifeTime first
            auto* enemyTypes = world.GetAllComponents<components::EnemyTypeComponent>();
            if (enemyTypes) {
                for (auto& [entity, typePtr] : *enemyTypes) {
                    typePtr->lifeTime += deltaTime;
                }
            }

            // Iterate over all positions
            for (auto& [entity, posPtr] : *positions) {
                auto& pos = *posPtr;
                auto* vel = world.GetComponent<components::Velocity>(entity);

                if (!vel) continue;

                // Handle special enemy behaviors BEFORE position update
                handleEnemyMovement(world, entity, pos, vel, deltaTime);

                // Update position based on velocity
                pos.x += vel->vx * deltaTime;
                pos.y += vel->vy * deltaTime;

                // Update projectile distance traveled (needed for collision detection)
                auto* proj = world.GetComponent<components::Projectile>(entity);
                if (proj) {
                    float dx = vel->vx * deltaTime;
                    float dy = vel->vy * deltaTime;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    proj->distanceTraveled += distance;
                }

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
         * @param deltaTime Time elapsed since last update
         */
        static void handleEnemyMovement(ECS::World& world, ECS::EntityID entity,
                                       components::Position& pos, components::Velocity* vel, float deltaTime) {
            auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
            if (!enemyType) return;

            // TankDestroyer boss: vertical only movement with bounce
            if (enemyType->type == components::EnemyType::TankDestroyer) {
                handleTankDestroyerMovement(world, entity, pos, vel);
            }
            // Snake enemy: sine wave pattern
            else if (enemyType->type == components::EnemyType::Snake) {
                handleSnakeMovement(world, entity, pos, vel, *enemyType);
            }
            // Suicide enemy: move towards player
            else if (enemyType->type == components::EnemyType::Suicide) {
                handleSuicideMovement(world, entity, pos, vel);
            }
        }

        /**
         * @brief Handle TankDestroyer boss vertical movement (no horizontal)
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleTankDestroyerMovement(ECS::World& world, ECS::EntityID entity,
                                              components::Position& pos, components::Velocity* vel) {
            if (!vel) return;

            // Boss doesn't move horizontally
            vel->vx = 0.0f;

            // Set vertical speed if not already set
            if (vel->vy == 0.0f) {
                vel->vy = 100.0f; // Start moving down
            }

            auto* collision = world.GetComponent<components::Collision>(entity);
            float halfHeight = collision ? collision->height / 2.0f : 90.0f; // Boss height ~180px

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

        /**
         * @brief Handle Snake enemy sine wave movement
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handleSnakeMovement(ECS::World& world, ECS::EntityID entity,
                                       components::Position& pos, components::Velocity* vel,
                                       components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            // Snake moves left and oscillates up/down in sine wave
            // Use lifeTime for consistent wave pattern
            const float HORIZONTAL_SPEED = -120.0f;
            const float AMPLITUDE = 80.0f;        // Vertical oscillation amplitude
            const float FREQUENCY = 2.0f;         // Wave frequency (cycles per second)

            // Calculate sine wave Y position offset
            float waveOffset = std::sin(enemyType.lifeTime * FREQUENCY) * AMPLITUDE;

            // Set horizontal velocity (constant left movement)
            vel->vx = HORIZONTAL_SPEED;

            // Set vertical velocity based on derivative of sine wave
            // dy/dt = AMPLITUDE * FREQUENCY * cos(time * FREQUENCY)
            vel->vy = AMPLITUDE * FREQUENCY * std::cos(enemyType.lifeTime * FREQUENCY);
        }

        /**
         * @brief Handle Suicide enemy movement towards player
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleSuicideMovement(ECS::World& world, ECS::EntityID entity,
                                         components::Position& pos, components::Velocity* vel) {
            if (!vel) return;

            // Find player
            auto* players = world.GetAllComponents<components::Player>();
            if (!players) {
                // No player found, move left by default
                vel->vx = -150.0f;
                vel->vy = 0.0f;
                return;
            }

            // Get first player position
            for (auto& [playerEntity, playerPtr] : *players) {
                auto* playerPos = world.GetComponent<components::Position>(playerEntity);
                if (!playerPos) continue;

                // Calculate direction to player
                float dx = playerPos->x - pos.x;
                float dy = playerPos->y - pos.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance > 0.0f) {
                    // Normalize and apply speed
                    const float SUICIDE_SPEED = 200.0f;
                    vel->vx = (dx / distance) * SUICIDE_SPEED;
                    vel->vy = (dy / distance) * SUICIDE_SPEED;
                }

                break; // Only track first player
            }
        }
    };
}

#endif // COMMON_SYSTEMS_MOVEMENT_SYSTEM_H