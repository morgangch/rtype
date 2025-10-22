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
class ServerEnemySystem : public ECS::System {
public:
    ServerEnemySystem() : ECS::System("ServerEnemySystem", 5), _spawnTimer(0.0f), _stateTick(0.0f), _bossSpawnTimer(0.0f) {}

    /**
     * @brief Update cycle - spawns enemies, broadcasts states, cleans dead entities
     * 
     * Execution order:
     * 1. Boss spawn check (every 3 minutes, only if no boss exists)
     * 2. Regular enemy spawning (every 2 seconds)
     * 3. Player state broadcasting (20Hz for active games)
     * 4. Entity cleanup (dead entities)
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (in seconds)
     */
    void Update(ECS::World &world, float deltaTime) override;

private:
    float _spawnTimer;                                      ///< Timer for regular enemy spawning
    static constexpr float SPAWN_INTERVAL = 2.0f;          ///< Spawn one enemy every 2 seconds

    float _stateTick;                                       ///< Timer for player state broadcasts
    static constexpr float STATE_TICK_INTERVAL = 0.05f;    ///< Broadcast at 20Hz (50ms)
    
    float _bossSpawnTimer;                                  ///< Timer for boss spawning
    static constexpr float BOSS_SPAWN_INTERVAL = 180.0f;   ///< Spawn boss every 3 minutes
    
    /**
     * @brief Spawns a boss for a specific room
     * 
     * Creates a boss entity with:
     * - 50 HP (much tankier than regular enemies)
     * - Positioned at center-right of screen
     * - Boss AI behavior (spread shot pattern)
     * 
     * Broadcasts SPAWN_ENEMY packet to all players in the room.
     * 
     * @param world The ECS world
     * @param room The room entity ID to spawn the boss for
     */
    void spawnBoss(ECS::World& world, ECS::EntityID room);
};

#endif // SERVER_ENEMY_SYSTEM_H
