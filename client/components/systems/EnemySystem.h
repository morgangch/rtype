/**
 * @file EnemySystem.h
 * @brief System for enemy AI, spawning, and behavior
 * 
 * This system handles enemy spawning, movement patterns,
 * shooting behavior, and cleanup.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_ENEMY_SYSTEM_H
#define CLIENT_SYSTEMS_ENEMY_SYSTEM_H

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <client/components/Sprite.h>
#include <client/components/FireRate.h>
#include <client/components/systems/ProjectileSystem.h>
#include <vector>
#include <cstdlib>

namespace rtype::client::systems {
    /**
     * @class EnemySystem
     * @brief Manages enemy lifecycle and AI behavior
     * 
     * The EnemySystem handles:
     * - Periodic enemy spawning
     * - Enemy movement (horizontal scroll)
     * - Enemy shooting AI
     * - Off-screen enemy cleanup
     * - Enemy count limiting
     * 
     * Enemies are team 1, move left at 100 px/s, and shoot periodically.
     */
    class EnemySystem {
    public:
        /** @brief Screen width */
        float screenWidth;
        
        /** @brief Screen height */
        float screenHeight;
        
        /** @brief Time between enemy spawns */
        static constexpr float SPAWN_INTERVAL = 2.0f;
        
        /** @brief Maximum number of enemies */
        static constexpr size_t MAX_ENEMIES = 10;
        
        /** @brief Enemy fire interval */
        static constexpr float FIRE_INTERVAL = 2.5f;
        
        /** @brief Random timer resolution */
        static constexpr int RANDOM_TIMER_RESOLUTION = 1000;
        
        /** @brief Spawn timer */
        float spawnTimer;
        
        /**
         * @brief Constructor
         * @param width Screen width
         * @param height Screen height
         */
        EnemySystem(float width = 1280.0f, float height = 720.0f)
            : screenWidth(width), screenHeight(height), spawnTimer(0.0f) {}
        
        /**
         * @brief Spawn a new enemy
         * @param world ECS world
         */
        void spawnEnemy(ECS::World& world) {
            auto enemy = world.CreateEntity();
            
            // Random Y position
            float randomY = 50.0f + static_cast<float>(rand() % static_cast<int>(screenHeight - 100.0f));
            
            // Position (spawn off-screen right)
            world.AddComponent<rtype::common::components::Position>(
                enemy, screenWidth + 24.0f, randomY, 0.0f);
            
            // Velocity (move left)
            world.AddComponent<rtype::common::components::Velocity>(
                enemy, -100.0f, 0.0f, 100.0f);
            
            // Sprite (red, medium size)
            world.AddComponent<rtype::client::components::Sprite>(
                enemy, sf::Vector2f(24.0f, 24.0f), sf::Color::Red);
            
            // Health (1 HP)
            world.AddComponent<rtype::common::components::Health>(enemy, 1);
            
            // Team (enemy team)
            world.AddComponent<rtype::common::components::Team>(enemy, rtype::common::components::TeamType::Enemy);
            
            // Fire rate with random initial cooldown
            float randomCooldown = static_cast<float>(rand() % RANDOM_TIMER_RESOLUTION) / 
                                  static_cast<float>(RANDOM_TIMER_RESOLUTION) * FIRE_INTERVAL;
            auto* fireRate = world.AddComponent<rtype::client::components::FireRate>(enemy, FIRE_INTERVAL);
            fireRate->cooldown = randomCooldown;
        }
        
        /**
         * @brief Update enemy system - spawning, AI, cleanup
         * @param world ECS world
         * @param deltaTime Time elapsed since last update
         * @param createEnemyCallback Callback to create enemies (x, y) -> EntityID
         * @param createProjectileCallback Callback to create enemy projectiles (x, y) -> EntityID
         */
        template<typename CreateEnemyFunc, typename CreateProjectileFunc>
        void update(ECS::World& world, float deltaTime, 
                   CreateEnemyFunc createEnemyCallback,
                   CreateProjectileFunc createProjectileCallback) {
            // Count current enemies by iterating all positions
            auto* positions = world.GetAllComponents<rtype::common::components::Position>();
            if (!positions) return;
            
            size_t enemyCount = 0;
            for (auto& [entity, posPtr] : *positions) {
                auto* team = world.GetComponent<rtype::common::components::Team>(entity);
                if (team && team->team == rtype::common::components::TeamType::Enemy) {
                    enemyCount++;
                }
            }
            
            // Spawn enemies periodically
            spawnTimer += deltaTime;
            if (spawnTimer >= SPAWN_INTERVAL && enemyCount < MAX_ENEMIES) {
                float randomY = 50.0f + static_cast<float>(rand() % static_cast<int>(screenHeight - 100.0f));
                createEnemyCallback(screenWidth + 24.0f, randomY);
                spawnTimer = 0.0f;
            }
            
            // Enemy shooting AI + cleanup
            std::vector<ECS::EntityID> toDestroy;
            for (auto& [entity, posPtr] : *positions) {
                auto& pos = *posPtr;
                auto* team = world.GetComponent<rtype::common::components::Team>(entity);
                if (!team || team->team != rtype::common::components::TeamType::Enemy) continue;
                
                // Shooting logic
                auto* fireRate = world.GetComponent<rtype::client::components::FireRate>(entity);
                if (fireRate && fireRate->canFire()) {
                    createProjectileCallback(pos.x, pos.y);
                    fireRate->shoot();
                }
                
                // Cleanup off-screen enemies (left side)
                if (pos.x < -50.0f) {
                    toDestroy.push_back(entity);
                }
            }
            
            // Destroy off-screen enemies
            for (auto entity : toDestroy) {
                world.DestroyEntity(entity);
            }
        }
        
        /**
         * @brief Reset spawn timer
         */
        void reset() {
            spawnTimer = 0.0f;
        }
    };
}

#endif // CLIENT_SYSTEMS_ENEMY_SYSTEM_H
