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

            // Boss movements
            if (enemyType->type == components::EnemyType::TankDestroyer) {
                handleTankDestroyerMovement(world, entity, pos, vel);
            }
            else if (enemyType->type == components::EnemyType::Serpent) {
                handleSerpentMovement(world, entity, pos, vel, *enemyType);
            }
            else if (enemyType->type == components::EnemyType::Fortress) {
                handleFortressMovement(world, entity, pos, vel, *enemyType);
            }
            else if (enemyType->type == components::EnemyType::Core) {
                handleCoreMovement(world, entity, pos, vel, *enemyType);
            }
            // Basic enemy movements
            else if (enemyType->type == components::EnemyType::Snake) {
                handleSnakeMovement(world, entity, pos, vel, *enemyType);
            }
            else if (enemyType->type == components::EnemyType::Suicide) {
                handleSuicideMovement(world, entity, pos, vel);
            }
            else if (enemyType->type == components::EnemyType::Pata) {
                handlePataMovement(world, entity, pos, vel, *enemyType);
            }
            // Advanced enemy movements
            else if (enemyType->type == components::EnemyType::Flanker) {
                handleFlankerMovement(world, entity, pos, vel);
            }
            else if (enemyType->type == components::EnemyType::Turret) {
                handleTurretMovement(world, entity, pos, vel);
            }
            else if (enemyType->type == components::EnemyType::Waver) {
                handleWaverMovement(world, entity, pos, vel, *enemyType);
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

        /**
         * @brief Handle Pata enemy rapid vertical oscillation
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handlePataMovement(ECS::World& world, ECS::EntityID entity,
                                      components::Position& pos, components::Velocity* vel,
                                      components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            // Pata moves left and oscillates vertically very rapidly
            const float HORIZONTAL_SPEED = -100.0f;
            const float AMPLITUDE = 60.0f;        // Vertical oscillation amplitude
            const float FREQUENCY = 4.0f;         // Wave frequency (fast oscillation)

            vel->vx = HORIZONTAL_SPEED;
            // Rapid vertical oscillation like "wing flapping"
            vel->vy = AMPLITUDE * FREQUENCY * std::cos(enemyType.lifeTime * FREQUENCY);
        }

        /**
         * @brief Handle Flanker enemy diagonal movement to position above/below player
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleFlankerMovement(ECS::World& world, ECS::EntityID entity,
                                         components::Position& pos, components::Velocity* vel) {
            if (!vel) return;

            // Find player
            auto* players = world.GetAllComponents<components::Player>();
            if (!players) {
                vel->vx = -90.0f;
                vel->vy = 0.0f;
                return;
            }

            // Get first player position
            for (auto& [playerEntity, playerPtr] : *players) {
                auto* playerPos = world.GetComponent<components::Position>(playerEntity);
                if (!playerPos) continue;

                // Move diagonally to match player's Y position
                const float HORIZONTAL_SPEED = -90.0f;
                const float VERTICAL_SPEED = 120.0f;

                vel->vx = HORIZONTAL_SPEED;

                // Move toward player's Y position
                float dy = playerPos->y - pos.y;
                if (std::abs(dy) > 10.0f) {  // Dead zone
                    vel->vy = (dy > 0.0f) ? VERTICAL_SPEED : -VERTICAL_SPEED;
                } else {
                    vel->vy = 0.0f;  // Stop when aligned
                }

                break;
            }
        }

        /**
         * @brief Handle Turret enemy erratic zigzag movement
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         */
        static void handleTurretMovement(ECS::World& world, ECS::EntityID entity,
                                       components::Position& pos, components::Velocity* vel) {
            if (!vel) return;

            // Turret is stationary
            vel->vx = 0.0f;
            vel->vy = 0.0f;
        }

        /**
         * @brief Handle Waver enemy erratic zigzag movement
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handleWaverMovement(ECS::World& world, ECS::EntityID entity,
                                       components::Position& pos, components::Velocity* vel,
                                       components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            // Waver moves in rapid zigzag pattern
            const float HORIZONTAL_SPEED = -110.0f;
            const float VERTICAL_AMPLITUDE = 150.0f;
            const float FREQUENCY = 3.0f;  // Fast zigzag

            vel->vx = HORIZONTAL_SPEED;

            // Square wave pattern for sharp zigzag
            float t = enemyType.lifeTime * FREQUENCY;
            float zigzag = (std::sin(t) > 0.0f) ? 1.0f : -1.0f;
            vel->vy = zigzag * VERTICAL_AMPLITUDE;
        }

        /**
         * @brief Handle Serpent boss wave movement
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handleSerpentMovement(ECS::World& world, ECS::EntityID entity,
                                         components::Position& pos, components::Velocity* vel,
                                         components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            // SERPENT: Stays in center, only vertical wave movement!
            const float WAVE_AMPLITUDE = 180.0f;   // HUGE vertical amplitude
            const float WAVE_FREQUENCY = 0.8f;     // Slower, menacing waves
            
            vel->vx = 0.0f;
            
            // Calculate smooth sine wave for vertical movement only
            vel->vy = WAVE_AMPLITUDE * WAVE_FREQUENCY * std::cos(enemyType.lifeTime * WAVE_FREQUENCY);
            
            // Keep serpent within screen bounds (100-620 for 720p screen)
            if (pos.y < 150.0f && vel->vy < 0) vel->vy = std::abs(vel->vy);
            if (pos.y > 570.0f && vel->vy > 0) vel->vy = -std::abs(vel->vy);
        }

        /**
         * @brief Handle Fortress boss stationary with orbiting turrets
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handleFortressMovement(ECS::World& world, ECS::EntityID entity,
                                          components::Position& pos, components::Velocity* vel,
                                          components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            // Fortress is stationary
            vel->vx = 0.0f;
            vel->vy = 0.0f;
        }

        /**
         * @brief Handle Core boss multi-phase movement
         * @param world The ECS world
         * @param entity The entity to handle
         * @param pos Reference to position component
         * @param vel Pointer to velocity component
         * @param enemyType Reference to enemy type component (contains lifeTime)
         */
        static void handleCoreMovement(ECS::World& world, ECS::EntityID entity,
                                      components::Position& pos, components::Velocity* vel,
                                      components::EnemyTypeComponent& enemyType) {
            if (!vel) return;

            auto* health = world.GetComponent<components::Health>(entity);
            if (!health) return;

            // Movement changes based on HP phase
            float hpPercent = (float)health->currentHp / (float)health->maxHp;

            if (hpPercent > 0.66f) {
                // Phase 1: Slow circular movement
                vel->vx = -40.0f;
                vel->vy = 80.0f * std::sin(enemyType.lifeTime * 1.0f);
            } else if (hpPercent > 0.33f) {
                // Phase 2: Faster movement
                vel->vx = -60.0f;
                vel->vy = 120.0f * std::sin(enemyType.lifeTime * 2.0f);
            } else {
                // Phase 3: Erratic chaotic movement
                float t = enemyType.lifeTime;
                vel->vx = -80.0f + 40.0f * std::sin(t * 3.0f);
                vel->vy = 150.0f * std::sin(t * 5.0f + std::cos(t * 2.0f));
            }
        }
    };
}

#endif // COMMON_SYSTEMS_MOVEMENT_SYSTEM_H