/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerEnemySystem - Spawns enemies and bosses, broadcasts player states
*/
#ifndef SERVER_ENEMY_SYSTEM_H
#define SERVER_ENEMY_SYSTEM_H

#include "ECS/System.h"
#include "rtype.h"

/**
 * @brief System responsible for enemy spawning and player state broadcasting
 * 
 * This system handles three main responsibilities:
 * 1. Spawns regular enemies every 2 seconds for active game rooms
 * 2. Spawns bosses every 3 minutes (only if no boss exists)
 * 3. Broadcasts player states to all clients at 20Hz for active games
 * 4. Cleans up dead entities and broadcasts destruction
 * 
 * All enemy spawns are server-authoritative and broadcast via SPAWN_ENEMY
 * packets to ensure all clients see the same enemies at the same time.
 * 
 * @note Priority: 5 (medium priority, gameplay system)
 */

#include <map>
#include <common/components/EnemyType.h>

enum class EnemySpawnPhase {
    OnlyBasic,          // 0-60s: Basic enemies only
    BasicAndAdvanced,   // 60-180s: Basic + Advanced enemies
    BossPhase           // 180s+: Boss + Basic + Advanced enemies
};

struct EnemySpawnConfig {
    rtype::common::components::EnemyType type;
    float interval; // seconds between spawns
    float timer;    // current timer for this type
};

struct LevelDefinition {
    rtype::common::components::EnemyType basicEnemy;
    rtype::common::components::EnemyType advancedEnemy;
    rtype::common::components::EnemyType bossEnemy;
};


/**
 * @class ServerEnemySystem
 * @brief System responsible for enemy spawning and player state broadcasting
 *
 * Handles enemy and boss spawning, player state broadcasting, and entity cleanup.
 */
class ServerEnemySystem : public ECS::System {
public:
    /**
     * @brief Constructor for ServerEnemySystem
     * Initializes timers and spawn phase.
     */
    ServerEnemySystem();

    /**
     * @brief Main update loop for the system
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last update
     * Spawns enemies/bosses, broadcasts player states, and cleans up entities.
     */
    void Update(ECS::World &world, float deltaTime) override;

    /**
     * @brief Spawns a regular enemy in the given room
     * @param world Reference to the ECS world
     * @param room Room entity ID
     * @param type Enemy type to spawn
     */
    void spawnEnemy(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType type);

    /**
     * @brief Spawns a boss enemy in the given room
     * @param world Reference to the ECS world
     * @param room Room entity ID
     * @param bossType Boss type to spawn
     */
    void spawnBoss(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType bossType);

private:
    float _levelTimer; ///< Timer for current level (resets each level)
    int _currentLevel; ///< Current level index (0-3 for 4 sub-levels)
    EnemySpawnPhase _phase; ///< Current enemy spawn phase
    bool _bossSpawned; ///< Whether a boss is currently spawned for this level

    std::vector<LevelDefinition> _levelDefinitions; ///< Enemy definitions for each level
    std::map<rtype::common::components::EnemyType, EnemySpawnConfig> _enemyConfigs; ///< Enemy spawn configs

    float _stateTick; ///< Timer for player state broadcast
    static constexpr float STATE_TICK_INTERVAL = 0.03f; // 30ms for better responsiveness

    // Update helpers
    void updatePhase(float deltaTime);
    void updateEnemySpawning(ECS::World& world, float deltaTime);
    void updateBossSpawning(ECS::World& world, float deltaTime);
    void updatePlayerStateBroadcast(ECS::World& world, float deltaTime);
    void cleanupDeadEntities(ECS::World& world);
    void checkBossDeathAndAdvanceLevel(ECS::World& world);
};

#endif // SERVER_ENEMY_SYSTEM_H
