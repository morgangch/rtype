/**
 * @file EnemyAISystem.h
 * @brief Enemy AI system for shooting patterns
 *
 * This system is shared between client and server for consistent enemy behavior.
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H
#define COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Team.h>
#include <common/components/Player.h>
#include <common/components/Health.h>
#include <common/components/FireRate.h>
#include <common/components/EnemyType.h>
#include <functional>
#include <cmath>
#include <iostream>

namespace rtype::common::systems {
    /**
     * @class EnemyAISystem
     * @brief Handles enemy shooting AI patterns
     *
     * This system is used by both client (for prediction) and server (authoritative).
     * Enemies shoot using the same logic on both sides for perfect synchronization.
     *
     * Different enemy types have different shooting patterns:
     * - Basic: Shoots straight left
     * - Shooter: Aims at player
     * - TankDestroyer (Boss): Shoots 3-projectile spread pattern
     */
    class EnemyAISystem {
    public:
        /**
         * @brief Callback type for creating projectiles
         * Parameters: shooterEntity, x, y, vx, vy
         */
        using ProjectileCallback = std::function<void(ECS::EntityID, float, float, float, float)>;

        /**
         * @brief Update enemy AI and shooting behavior
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         * @param createProjectile Callback to create enemy projectiles
         *
         * Finds all enemies and makes them shoot according to their type.
         */
        static void update(ECS::World& world, float deltaTime, ProjectileCallback createProjectile) {
            auto* teams = world.GetAllComponents<components::Team>();
            if (!teams) return;

            float playerX = 0.0f;
            float playerY = 0.0f;
            bool playerFound = false;

            auto* players = world.GetAllComponents<components::Player>();
            if (players) {
                for (auto& [playerEntity, playerPtr] : *players) {
                    auto* pos = world.GetComponent<components::Position>(playerEntity);
                    if (pos) {
                        playerX = pos->x;
                        playerY = pos->y;
                        playerFound = true;
                        break;
                    }
                }
            }

            for (auto& [entity, teamPtr] : *teams) {
                if (teamPtr->team != components::TeamType::Enemy) continue;
                if (!world.GetComponent<components::Health>(entity)) continue;

                auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
                auto* fireRate = world.GetComponent<components::FireRate>(entity);
                auto* pos = world.GetComponent<components::Position>(entity);

                if (!fireRate || !pos) continue;
                if (!fireRate->canFire()) continue;

                // Enemies that don't shoot (only Suicide)
                if (enemyType && enemyType->type == components::EnemyType::Suicide) {
                    continue;
                }

                // Boss shooting patterns
                if (enemyType && enemyType->type == components::EnemyType::TankDestroyer) {
                    handleTankDestroyerShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Serpent) {
                    handleSerpentShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Fortress) {
                    handleFortressShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, enemyType->lifeTime, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Core) {
                    auto* health = world.GetComponent<components::Health>(entity);
                    handleCoreShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, health, enemyType->lifeTime, createProjectile);
                    fireRate->shoot();
                }
                // Advanced enemy shooting patterns
                else if (enemyType && enemyType->type == components::EnemyType::Flanker) {
                    handleFlankerShooting(entity, pos->x, pos->y, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Turret) {
                    handleTurretShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Waver) {
                    handleWaverShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                }
                // Basic enemy shooting patterns
                else if (enemyType && enemyType->type == components::EnemyType::Pata) {
                    handlePataShooting(entity, pos->x, pos->y, createProjectile);
                    fireRate->shoot();
                }
                else if (enemyType && enemyType->type == components::EnemyType::Basic) {
                    createProjectile(entity, pos->x, pos->y, -300.0f, 0.0f);
                    fireRate->shoot();
                }
                // Shielded and Bomber handled separately (no FireRate component or special logic)
                else {
                    // Default: shoot straight left
                    createProjectile(entity, pos->x, pos->y, -300.0f, 0.0f);
                    fireRate->shoot();
                }
            }
        }

    private:
        /**
         * @brief Handle TankDestroyer boss shooting pattern (3-projectile spread)
         * @param shooter Enemy entity ID
         * @param x Enemy X position
         * @param y Enemy Y position
         * @param targetX Player X position
         * @param targetY Player Y position
         * @param hasTarget Whether player was found
         * @param createProjectile Callback to create projectiles
         */
        static void handleTankDestroyerShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                                bool hasTarget, ProjectileCallback createProjectile) {
            if (!hasTarget) return;

            // Calculate direction to player
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= 0.0f) return;

            const float PROJECTILE_SPEED = 350.0f;
            float baseVx = (dx / distance) * PROJECTILE_SPEED;
            float baseVy = (dy / distance) * PROJECTILE_SPEED;

            // Center projectile - aimed at player
            createProjectile(shooter, x, y, baseVx, baseVy);

            // Spread pattern: ±15 degrees from center
            const float spreadAngle = 0.26f; // ~15 degrees in radians

            // Upper projectile (rotate counter-clockwise)
            float upperVx = baseVx * std::cos(spreadAngle) - baseVy * std::sin(spreadAngle);
            float upperVy = baseVx * std::sin(spreadAngle) + baseVy * std::cos(spreadAngle);
            createProjectile(shooter, x, y, upperVx, upperVy);

            // Lower projectile (rotate clockwise)
            float lowerVx = baseVx * std::cos(-spreadAngle) - baseVy * std::sin(-spreadAngle);
            float lowerVy = baseVx * std::sin(-spreadAngle) + baseVy * std::cos(-spreadAngle);
            createProjectile(shooter, x, y, lowerVx, lowerVy);
        }

        /**
         * @brief Handle Shooter enemy aiming at player
         * @param shooter Enemy entity ID
         * @param x Enemy X position
         * @param y Enemy Y position
         * @param targetX Player X position
         * @param targetY Player Y position
         * @param hasTarget Whether player was found
         * @param createProjectile Callback to create projectiles
         */
        static void handleShooterShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                         bool hasTarget, ProjectileCallback createProjectile) {
            if (!hasTarget) return;

            // Calculate direction to player
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= 0.0f) return;

            // Normalize and apply speed
            const float PROJECTILE_SPEED = 300.0f;
            float vx = (dx / distance) * PROJECTILE_SPEED;
            float vy = (dy / distance) * PROJECTILE_SPEED;

            createProjectile(shooter, x, y, vx, vy);
        }

        /**
         * @brief Handle Pata enemy double shot pattern
         */
        static void handlePataShooting(ECS::EntityID shooter, float x, float y,
                                      ProjectileCallback createProjectile) {
            // Fire two projectiles slightly offset vertically
            createProjectile(shooter, x, y - 10.0f, -300.0f, 0.0f);  // Upper shot
            createProjectile(shooter, x, y + 10.0f, -300.0f, 0.0f);  // Lower shot
        }

        /**
         * @brief Handle Flanker enemy perpendicular shooting
         */
        static void handleFlankerShooting(ECS::EntityID shooter, float x, float y,
                                         ProjectileCallback createProjectile) {
            // Fire downward (perpendicular to movement)
            createProjectile(shooter, x, y, 0.0f, 250.0f);  // Shoot down
            createProjectile(shooter, x, y, 0.0f, -250.0f); // Shoot up
        }

        /**
         * @brief Handle Turret enemy 3-shot burst aimed at player
         * Turret is stationary and fires a tight 3-projectile burst toward player
         */
        static void handleTurretShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                        bool hasTarget, ProjectileCallback createProjectile) {
            if (!hasTarget) {
                // Default shoot left if no target
                createProjectile(shooter, x, y, -300.0f, 0.0f);
                return;
            }

            // Calculate direction to player
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance <= 0.0f) return;

            const float PROJECTILE_SPEED = 400.0f;
            float vx = (dx / distance) * PROJECTILE_SPEED;
            float vy = (dy / distance) * PROJECTILE_SPEED;

            // Fire 3-shot burst with tight spread (±5 degrees)
            const float spreadAngle = 0.087f; // ~5 degrees in radians

            // Center shot aimed directly at player
            createProjectile(shooter, x, y, vx, vy);

            // Upper shot (+5 degrees)
            float upperVx = vx * std::cos(spreadAngle) - vy * std::sin(spreadAngle);
            float upperVy = vx * std::sin(spreadAngle) + vy * std::cos(spreadAngle);
            createProjectile(shooter, x, y, upperVx, upperVy);

            // Lower shot (-5 degrees)
            float lowerVx = vx * std::cos(-spreadAngle) - vy * std::sin(-spreadAngle);
            float lowerVy = vx * std::sin(-spreadAngle) + vy * std::cos(-spreadAngle);
            createProjectile(shooter, x, y, lowerVx, lowerVy);
        }

        /**
         * @brief Handle Waver enemy triple burst
         */
        static void handleWaverShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                       bool hasTarget, ProjectileCallback createProjectile) {
            if (!hasTarget) {
                // Shoot straight if no target
                createProjectile(shooter, x, y, -300.0f, 0.0f);
                return;
            }

            // Calculate direction to player
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance <= 0.0f) return;

            const float PROJECTILE_SPEED = 280.0f;
            float vx = (dx / distance) * PROJECTILE_SPEED;
            float vy = (dy / distance) * PROJECTILE_SPEED;

            // Fire triple burst: center, +15deg, -15deg
            const float spreadAngle = 0.26f; // ~15 degrees

            createProjectile(shooter, x, y, vx, vy);  // Center

            float upperVx = vx * std::cos(spreadAngle) - vy * std::sin(spreadAngle);
            float upperVy = vx * std::sin(spreadAngle) + vy * std::cos(spreadAngle);
            createProjectile(shooter, x, y, upperVx, upperVy);

            float lowerVx = vx * std::cos(-spreadAngle) - vy * std::sin(-spreadAngle);
            float lowerVy = vx * std::sin(-spreadAngle) + vy * std::cos(-spreadAngle);
            createProjectile(shooter, x, y, lowerVx, lowerVy);
        }

        /**
         * @brief Handle Serpent boss shooting (5-spread pattern aimed at player)
         * Serpent shoots a WIDE 5-projectile spread toward the player
         * Unlike TankDestroyer (3-spread straight left), this actively tracks player!
         */
        static void handleSerpentShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                         bool hasTarget, ProjectileCallback createProjectile) {
            // If no target, shoot spread to the left
            float dx = hasTarget ? (targetX - x) : -1.0f;
            float dy = hasTarget ? (targetY - y) : 0.0f;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance <= 0.0f) distance = 1.0f;

            const float PROJECTILE_SPEED = 350.0f; // Faster projectiles than TankDestroyer (320)
            float baseVx = (dx / distance) * PROJECTILE_SPEED;
            float baseVy = (dy / distance) * PROJECTILE_SPEED;

            // WIDE 5-projectile spread: center, ±20deg, ±40deg (wider than TankDestroyer)
            const float spread1 = 0.35f;  // ~20 degrees
            const float spread2 = 0.70f;  // ~40 degrees

            createProjectile(shooter, x, y, baseVx, baseVy);  // Center shot

            // Four additional shots in wide spread pattern
            for (float angle : {spread1, -spread1, spread2, -spread2}) {
                float rotVx = baseVx * std::cos(angle) - baseVy * std::sin(angle);
                float rotVy = baseVx * std::sin(angle) + baseVy * std::cos(angle);
                createProjectile(shooter, x, y, rotVx, rotVy);
            }
        }

        /**
         * @brief Handle Fortress boss random non-targeted shooting pattern
         * Shoots in random directions to avoid network saturation
         * The boss stays stationary and fires projectiles randomly
         */
        static void handleFortressShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                          bool hasTarget, float lifeTime, ProjectileCallback createProjectile) {
            const float PROJECTILE_SPEED = 280.0f;

            // Fire 4 projectiles in random directions (not aimed at player)
            // This reduces network traffic compared to 8-direction patterns
            for (int i = 0; i < 4; i++) {
                // Generate random angle for each projectile
                float randomAngle = (std::rand() % 360) * 3.14159f / 180.0f;
                float vx = std::cos(randomAngle) * PROJECTILE_SPEED;
                float vy = std::sin(randomAngle) * PROJECTILE_SPEED;
                createProjectile(shooter, x, y, vx, vy);
            }
        }

        /**
         * @brief Handle Core boss multi-phase shooting
         */
        static void handleCoreShooting(ECS::EntityID shooter, float x, float y, float targetX, float targetY,
                                      bool hasTarget, components::Health* health, float lifeTime,
                                      ProjectileCallback createProjectile) {
            if (!health) return;

            float hpPercent = (float)health->currentHp / (float)health->maxHp;

            if (hpPercent > 0.66f) {
                // Phase 1: Slow rotating circular pattern (8 directions)
                const float PROJECTILE_SPEED = 280.0f;
                float baseAngle = lifeTime * 1.5f;

                for (int i = 0; i < 8; i++) {
                    float angle = baseAngle + (i * 3.14159f * 2.0f / 8.0f);
                    float vx = std::cos(angle) * PROJECTILE_SPEED;
                    float vy = std::sin(angle) * PROJECTILE_SPEED;
                    createProjectile(shooter, x, y, vx, vy);
                }
            }
            else if (hpPercent > 0.33f) {
                // Phase 2: Spiral pattern (12 directions, rotating faster)
                const float PROJECTILE_SPEED = 320.0f;
                float baseAngle = lifeTime * 3.0f;

                for (int i = 0; i < 12; i++) {
                    float angle = baseAngle + (i * 3.14159f * 2.0f / 12.0f);
                    float vx = std::cos(angle) * PROJECTILE_SPEED;
                    float vy = std::sin(angle) * PROJECTILE_SPEED;
                    createProjectile(shooter, x, y, vx, vy);
                }
            }
            else {
                // Phase 3: Chaotic dense pattern (16 directions)
                const float PROJECTILE_SPEED = 350.0f;
                float baseAngle = lifeTime * 5.0f;

                for (int i = 0; i < 16; i++) {
                    float angle = baseAngle + (i * 3.14159f * 2.0f / 16.0f);
                    // Vary speed slightly for chaos
                    float speed = PROJECTILE_SPEED + 50.0f * std::sin(lifeTime * 7.0f + i);
                    float vx = std::cos(angle) * speed;
                    float vy = std::sin(angle) * speed;
                    createProjectile(shooter, x, y, vx, vy);
                }
            }
        }
    };
}

#endif // COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H
