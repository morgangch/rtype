/**
 * @file EnemySpawnSystem.h
 * @brief System for spawning enemies periodically
 * 
 * This system manages enemy spawning logic including:
 * - Regular enemy spawning at intervals
 * - Boss spawning at timed intervals
 * - Enemy count limits
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_ENEMY_SPAWN_SYSTEM_H
#define COMMON_SYSTEMS_ENEMY_SPAWN_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Team.h>
#include <common/components/Health.h>

#include <functional>
#include <cstdlib>

namespace rtype::common::systems {
    /**
     * @class EnemySpawnSystem
     * @brief Spawns enemies at regular intervals
     * 
     * This system manages enemy spawning, including different enemy types
     * and boss spawning. It requires callbacks to create entities.
     */
    class EnemySpawnSystem : public ECS::System {
    public:
        /**
         * @brief Enemy creation callback types
         */
        using BasicEnemyCallback = std::function<void(float, float, ECS::World&)>;
        using ShooterEnemyCallback = std::function<void(float, float, ECS::World&)>;
        using BossCallback = std::function<void(float, float, ECS::World&)>;
        using BossCheckCallback = std::function<bool(ECS::World&)>;

    private:
        float m_enemySpawnTimer;
        float m_bossSpawnTimer;
        float m_enemySpawnInterval;
        float m_bossSpawnInterval;
        size_t m_maxEnemies;
        
        BasicEnemyCallback m_createBasicEnemy;
        ShooterEnemyCallback m_createShooterEnemy;
        BossCallback m_createBoss;
        BossCheckCallback m_isBossActive;

    public:
        /**
         * @brief Constructor
         * @param enemyInterval Time between enemy spawns (default: 2.0s)
         * @param bossInterval Time between boss spawns (default: 180.0s)
         * @param maxEnemies Maximum number of enemies on screen (default: 10)
         */
        EnemySpawnSystem(float enemyInterval = 2.0f, 
                        float bossInterval = 180.0f,
                        size_t maxEnemies = 10)
            : ECS::System("EnemySpawnSystem", 30),
              m_enemySpawnTimer(0.0f),
              m_bossSpawnTimer(0.0f),
              m_enemySpawnInterval(enemyInterval),
              m_bossSpawnInterval(bossInterval),
              m_maxEnemies(maxEnemies) {}

        /**
         * @brief Set enemy creation callbacks
         */
        void SetCallbacks(BasicEnemyCallback basic, 
                         ShooterEnemyCallback shooter,
                         BossCallback boss,
                         BossCheckCallback bossCheck) {
            m_createBasicEnemy = basic;
            m_createShooterEnemy = shooter;
            m_createBoss = boss;
            m_isBossActive = bossCheck;
        }

        /**
         * @brief Update spawn timers and spawn enemies
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            if (!m_createBasicEnemy || !m_createShooterEnemy || 
                !m_createBoss || !m_isBossActive) return;

            m_enemySpawnTimer += deltaTime;
            m_bossSpawnTimer += deltaTime;
            
            // Boss spawn check
            if (m_bossSpawnTimer >= m_bossSpawnInterval && !m_isBossActive(world)) {
                // Spawn boss at right side, centered vertically
                // Note: Position depends on screen size (to be provided by game)
                m_bossSpawnTimer = 0.0f;
            }
            
            // Regular enemy spawn check
            if (m_enemySpawnTimer >= m_enemySpawnInterval) {
                // Count current enemies
                size_t enemyCount = countEnemies(world);
                
                // Spawn if under limit
                if (enemyCount < m_maxEnemies) {
                    // Spawn location will be provided by game (screen-dependent)
                    // 40% chance for shooter, 60% for basic
                    int randomType = rand() % 100;
                    
                    m_enemySpawnTimer = 0.0f;
                }
            }
        }

        /**
         * @brief Spawn a basic enemy
         * @param x X position
         * @param y Y position
         * @param world The ECS world
         */
        void SpawnBasicEnemy(float x, float y, ECS::World& world) {
            if (m_createBasicEnemy) {
                m_createBasicEnemy(x, y, world);
            }
        }

        /**
         * @brief Spawn a shooter enemy
         * @param x X position
         * @param y Y position
         * @param world The ECS world
         */
        void SpawnShooterEnemy(float x, float y, ECS::World& world) {
            if (m_createShooterEnemy) {
                m_createShooterEnemy(x, y, world);
            }
        }

        /**
         * @brief Spawn a boss
         * @param x X position
         * @param y Y position
         * @param world The ECS world
         */
        void SpawnBoss(float x, float y, ECS::World& world) {
            if (m_createBoss) {
                m_createBoss(x, y, world);
                m_bossSpawnTimer = 0.0f;
            }
        }

        /**
         * @brief Check if should spawn and trigger spawning
         * @param world The ECS world
         * @param spawnX X position for spawning
         * @param spawnY Y position for spawning (random)
         * @param bossX Boss X position
         * @param bossY Boss Y position (centered)
         */
        void TrySpawn(ECS::World& world, float spawnX, float spawnY,
                     float bossX, float bossY) {
            // Boss spawn
            if (m_bossSpawnTimer >= m_bossSpawnInterval && !m_isBossActive(world)) {
                SpawnBoss(bossX, bossY, world);
            }
            
            // Regular enemy spawn
            if (m_enemySpawnTimer >= m_enemySpawnInterval) {
                size_t enemyCount = countEnemies(world);
                
                if (enemyCount < m_maxEnemies) {
                    int randomType = rand() % 100;
                    if (randomType < 40) {
                        SpawnShooterEnemy(spawnX, spawnY, world);
                    } else {
                        SpawnBasicEnemy(spawnX, spawnY, world);
                    }
                    m_enemySpawnTimer = 0.0f;
                }
            }
        }

        /**
         * @brief Set spawn intervals
         */
        void SetIntervals(float enemyInterval, float bossInterval) {
            m_enemySpawnInterval = enemyInterval;
            m_bossSpawnInterval = bossInterval;
        }

        /**
         * @brief Set maximum enemy count
         */
        void SetMaxEnemies(size_t max) {
            m_maxEnemies = max;
        }

        /**
         * @brief Reset spawn timers
         */
        void ResetTimers() {
            m_enemySpawnTimer = 0.0f;
            m_bossSpawnTimer = 0.0f;
        }

    private:
        /**
         * @brief Count current enemies on screen
         */
        size_t countEnemies(ECS::World& world) {
            size_t count = 0;
            auto* teams = world.GetAllComponents<components::Team>();
            
            if (teams) {
                for (auto& [entity, teamPtr] : *teams) {
                    if (teamPtr->team == components::TeamType::Enemy) {
                        // Only count entities with Health (not projectiles)
                        if (world.GetComponent<components::Health>(entity)) {
                            count++;
                        }
                    }
                }
            }
            
            return count;
        }

    public:
        /**
         * @brief Initialize the system
         * @param world The ECS world
         */
        void Initialize(ECS::World& world) override {
            ResetTimers();
        }

        /**
         * @brief Cleanup the system
         * @param world The ECS world
         */
        void Cleanup(ECS::World& world) override {
            m_createBasicEnemy = nullptr;
            m_createShooterEnemy = nullptr;
            m_createBoss = nullptr;
            m_isBossActive = nullptr;
        }
    };
}

#endif // COMMON_SYSTEMS_ENEMY_SPAWN_SYSTEM_H
