/**
 * @file EnemyAISystem.h
 * @brief AI system for enemy shooting behavior
 * 
 * This system manages enemy shooting patterns based on enemy type.
 * Different enemy types have different shooting behaviors.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H
#define COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Team.h>
#include <common/components/Player.h>
#include <common/components/Health.h>
#include <common/components/FireRate.h>
#include <common/components/EnemyType.h>

#include <functional>
#include <cmath>

namespace rtype::common::systems {
    /**
     * @class EnemyAISystem
     * @brief Manages enemy shooting AI
     * 
     * This system handles enemy shooting behavior:
     * - Basic enemies: shoot straight
     * - Shooter enemies: aim at player
     * - Boss enemies: spread pattern
     * 
     * Requires a callback to create projectiles (game-specific logic).
     */
    class EnemyAISystem : public ECS::System {
    public:
        /**
         * @brief Projectile creation callback
         * 
         * Parameters: x, y, vx, vy, world
         */
        using ProjectileCallback = std::function<void(float, float, float, float, ECS::World&)>;

    private:
        ProjectileCallback m_createProjectile;

    public:
        /**
         * @brief Constructor
         */
        EnemyAISystem() : ECS::System("EnemyAISystem", 40) {}

        /**
         * @brief Set the projectile creation callback
         * @param callback Function to create enemy projectiles
         */
        void SetProjectileCallback(ProjectileCallback callback) {
            m_createProjectile = callback;
        }

        /**
         * @brief Update enemy AI
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            if (!m_createProjectile) return;

            auto* teams = world.GetAllComponents<components::Team>();
            if (!teams) return;
            
            // Find player position
            float playerX = 0.0f, playerY = 0.0f;
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
            
            // Process each enemy
            for (auto& [entity, teamPtr] : *teams) {
                // Only process enemies (not projectiles)
                if (teamPtr->team != components::TeamType::Enemy) continue;
                if (!world.GetComponent<components::Health>(entity)) continue;
                
                auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
                auto* fireRate = world.GetComponent<components::FireRate>(entity);
                auto* pos = world.GetComponent<components::Position>(entity);
                
                if (!fireRate || !pos || !fireRate->canFire()) continue;
                
                // Determine shooting behavior based on enemy type
                if (enemyType && enemyType->type == components::EnemyType::Boss) {
                    // Boss: spread pattern (3 projectiles)
                    handleBossShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
                    fireRate->shoot();
                } else if (enemyType && enemyType->type == components::EnemyType::Shooter) {
                    // Shooter: aim at player
                    handleShooterShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
                    fireRate->shoot();
                } else {
                    // Basic enemy: shoot straight left
                    m_createProjectile(pos->x, pos->y, -300.0f, 0.0f, world);
                    fireRate->shoot();
                }
            }
        }

    private:
        /**
         * @brief Handle boss shooting pattern (spread)
         */
        void handleBossShooting(float x, float y, float targetX, float targetY, 
                               bool hasTarget, ECS::World& world) {
            if (!hasTarget) return;
            
            const float PROJECTILE_SPEED = 350.0f;
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= 0.0f) return;
            
            float baseVx = (dx / distance) * PROJECTILE_SPEED;
            float baseVy = (dy / distance) * PROJECTILE_SPEED;
            
            // Center projectile
            m_createProjectile(x, y, baseVx, baseVy, world);
            
            // Spread pattern: ±15 degrees
            const float spreadAngle = 0.26f; // ~15 degrees in radians
            
            // Upper projectile (counter-clockwise)
            float upperVx = baseVx * std::cos(spreadAngle) - baseVy * std::sin(spreadAngle);
            float upperVy = baseVx * std::sin(spreadAngle) + baseVy * std::cos(spreadAngle);
            m_createProjectile(x, y, upperVx, upperVy, world);
            
            // Lower projectile (clockwise)
            float lowerVx = baseVx * std::cos(-spreadAngle) - baseVy * std::sin(-spreadAngle);
            float lowerVy = baseVx * std::sin(-spreadAngle) + baseVy * std::cos(-spreadAngle);
            m_createProjectile(x, y, lowerVx, lowerVy, world);
        }

        /**
         * @brief Handle shooter enemy aiming
         */
        void handleShooterShooting(float x, float y, float targetX, float targetY,
                                  bool hasTarget, ECS::World& world) {
            if (!hasTarget) return;
            
            const float PROJECTILE_SPEED = 300.0f;
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= 0.0f) return;
            
            float vx = (dx / distance) * PROJECTILE_SPEED;
            float vy = (dy / distance) * PROJECTILE_SPEED;
            
            m_createProjectile(x, y, vx, vy, world);
        }

    public:
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
            m_createProjectile = nullptr;
        }
    };
}

#endif // COMMON_SYSTEMS_ENEMY_AI_SYSTEM_H
