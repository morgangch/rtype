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
#include <mapparser.h>
#include <map>
#include <vector>
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


/**
 * @class ServerEnemySystem
 * @brief System responsible for enemy spawning and player state broadcasting
 *
 * Handles enemy and boss spawning, player state broadcasting, and entity cleanup.
 * Now supports map-driven enemy spawning using the mapparser library.
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
     * @brief Load enemy spawn locations from a map
     * @param mapDir Path to the map directory
     * @return True if loaded successfully, false otherwise
     * Loads map tiles and filters enemy spawn locations by type.
     */
    bool loadMap(const std::string& mapDir);

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
    float _levelTimer; ///< Total time since game start
    EnemySpawnPhase _phase; ///< Current enemy spawn phase
    bool _bossSpawned; ///< Whether a boss is currently spawned

    std::map<rtype::common::components::EnemyType, EnemySpawnConfig> _enemyConfigs; ///< Enemy spawn configs

    float _stateTick; ///< Timer for player state broadcast
    static constexpr float STATE_TICK_INTERVAL = 0.05f;

    // Map-driven spawning
    bool _mapLoaded; ///< Whether a map has been loaded
    std::vector<Tile> _classicEnemySpawns; ///< Classic enemy spawn tiles from map
    std::vector<Tile> _eliteEnemySpawns; ///< Elite enemy spawn tiles from map
    std::vector<Tile> _bossSpawns; ///< Boss spawn tiles from map
    size_t _nextClassicIndex; ///< Next classic spawn index (for cycling)
    size_t _nextEliteIndex; ///< Next elite spawn index (for cycling)
    size_t _nextBossIndex; ///< Next boss spawn index (for cycling)

    // Update helpers
    void updatePhase(float deltaTime);
    void updateEnemySpawning(ECS::World& world, float deltaTime);
    void updateBossSpawning(ECS::World& world, float deltaTime);
    void updatePlayerStateBroadcast(ECS::World& world, float deltaTime);
    void cleanupDeadEntities(ECS::World& world);
    
    // Map-driven helper methods
    /**
     * @brief Get spawn position from tile or fallback to default
     * @param tiles Vector of spawn tiles
     * @param index Current spawn index (will be incremented and wrapped)
     * @param defaultX Default X position if no tiles available
     * @param defaultY Default Y position if no tiles available
     * @return Spawn position as {x, y}
     */
    std::pair<float, float> getSpawnPosition(
        const std::vector<Tile>& tiles,
        size_t& index,
        float defaultX,
        float defaultY
    );
    
    /**
     * @brief Read enemy stats from tile metadata
     * @param tile Tile with metadata
     * @param outHp Output parameter for health
     * @param outVx Output parameter for velocity X
     * Reads "health" and "speed" keys from tile.definition.metadata
     */
    void readEnemyStats(const Tile& tile, int& outHp, float& outVx);
};

#endif // SERVER_ENEMY_SYSTEM_H
