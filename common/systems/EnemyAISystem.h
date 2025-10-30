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
                if (enemyType && enemyType->type == components::EnemyType::Suicide) continue;

                if (enemyType && enemyType->type == components::EnemyType::TankDestroyer) {
                    handleTankDestroyerShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                } else if (enemyType && enemyType->type == components::EnemyType::Shooter) {
                    handleShooterShooting(entity, pos->x, pos->y, playerX, playerY, playerFound, createProjectile);
                    fireRate->shoot();
                } else {
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
    };
}

#endif // COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H
