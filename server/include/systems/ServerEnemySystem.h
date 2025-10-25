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
    OnlyBasic,
    BasicAndAdvanced,
    BossAndAll
};

struct EnemySpawnConfig {
    rtype::common::components::EnemyType type;
    float interval; // seconds between spawns
    float timer;    // current timer for this type
};

class ServerEnemySystem : public ECS::System {
public:
    ServerEnemySystem();

    void Update(ECS::World &world, float deltaTime) override;

    // Spawn helpers for each enemy type
    void spawnEnemy(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType type);
    void spawnBoss(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType bossType);

    // Configure spawn intervals per type
    void setSpawnInterval(rtype::common::components::EnemyType type, float interval);

    // Set phase (0-1min, 1-3min, >3min)
    void setSpawnPhase(EnemySpawnPhase phase);

private:
    float _levelTimer; // total time since game start
    EnemySpawnPhase _phase;
    bool _bossSpawned;

    std::map<rtype::common::components::EnemyType, EnemySpawnConfig> _enemyConfigs;

    float _stateTick;
    static constexpr float STATE_TICK_INTERVAL = 0.05f;

    // Update helpers
    void updatePhase(float deltaTime);
    void updateEnemySpawning(ECS::World& world, float deltaTime);
    void updateBossSpawning(ECS::World& world, float deltaTime);
    void updatePlayerStateBroadcast(ECS::World& world, float deltaTime);
    void cleanupDeadEntities(ECS::World& world);
};

#endif // SERVER_ENEMY_SYSTEM_H
