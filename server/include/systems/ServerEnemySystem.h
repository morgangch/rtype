/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerEnemySystem - Tile-driven enemy spawning system
*/
#ifndef SERVER_ENEMY_SYSTEM_H
#define SERVER_ENEMY_SYSTEM_H

#include "ECS/System.h"
#include "rtype.h"
#include <mapparser.h>
#include <map>
#include <vector>
#include <common/components/EnemyType.h>

/**
 * @class ServerEnemySystem
 * @brief System responsible for tile-driven enemy spawning and player state broadcasting
 *
 * This system uses the mapparser library to load enemy spawn locations from .tile files.
 * All enemy stats, types, and behaviors are defined in the tile metadata, making the
 * spawning system fully data-driven and eliminating hardcoded enemy configurations.
 */
class ServerEnemySystem : public ECS::System {
public:
    /**
     * @brief Constructor for ServerEnemySystem
     * Initializes spawn timers and tile-driven spawning system.
     */
    ServerEnemySystem();

    /**
     * @brief Main update loop for the system
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last update
     * 
     * Handles tile-driven enemy spawning, player state broadcasting, and dead entity cleanup.
     */
    void Update(ECS::World &world, float deltaTime) override;

    /**
     * @brief Load enemy spawn locations from a map directory
     * @param mapDir Path to the map directory containing .def, .map, and .tile files
     * @return True if loaded successfully, false otherwise
     * 
     * Loads and categorizes all enemy spawn tiles by type (Classic, Elite, Boss).
     * Each tile contains complete enemy definition including type, stats, and behavior.
     * Uses MapParser to load tile definitions and classify them by type.
     * Classic, Elite, and Boss spawns are extracted and cached for round-robin spawning.
     */
    bool loadMap(const std::string& mapDir);

private:
    float _stateTick; ///< Timer for player state broadcast
    static constexpr float STATE_TICK_INTERVAL = 0.03f; // 30ms for smooth updates

    // Map-driven spawning state
    bool _mapLoaded; ///< Whether a map has been successfully loaded
    std::vector<Tile> _classicEnemySpawns; ///< Tiles for basic/classic enemies
    std::vector<Tile> _eliteEnemySpawns; ///< Tiles for advanced/elite enemies
    std::vector<Tile> _bossSpawns; ///< Tiles for boss enemies
    size_t _nextClassicIndex; ///< Round-robin index for classic spawns
    size_t _nextEliteIndex; ///< Round-robin index for elite spawns
    size_t _nextBossIndex; ///< Round-robin index for boss spawns
    
    // Spawn timing
    std::map<TileType, float> _spawnTimers; ///< Current timer for each tile type
    std::map<TileType, float> _spawnIntervals; ///< Spawn interval for each tile type

    /**
     * @brief Update enemy spawning logic based on timers and loaded tiles
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last update
     * 
     * Cycles through loaded tiles and spawns enemies when timers elapse.
     */
    void updateEnemySpawning(ECS::World& world, float deltaTime);
    
    /**
     * @brief Broadcast player positions and health to all clients
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last update
     */
    void updatePlayerStateBroadcast(ECS::World& world, float deltaTime);
    
    /**
     * @brief Remove dead entities and broadcast destroy packets
     * @param world Reference to the ECS world
     */
    void cleanupDeadEntities(ECS::World& world);
    
    // Tile-driven spawning helpers
    /**
     * @brief Spawn an enemy from a tile definition
     * @param world Reference to the ECS world
     * @param room Room entity ID where enemy will spawn
     * @param tile Tile containing complete enemy definition
     * 
     * Reads all enemy properties from tile metadata:
     * - enemy_type: Enum value (Basic, Snake, Shielded, TankDestroyer, etc.)
     * - health: Hit points
     * - speed: Movement speed
     * - damage: Damage dealt to players
     * - fire_rate: Shooting interval
     * - has_shield: Shield type (cyclic, red, etc.)
     * 
     * Creates entity with all components and broadcasts spawn to clients.
     */
    void spawnEnemyFromTile(ECS::World& world, ECS::EntityID room, const Tile& tile);
    
    /**
     * @brief Parse enemy type from tile metadata
     * @param tile Tile to read from
     * @return Corresponding EnemyType enum value
     * 
     * Reads the "enemy_type" key from tile.definition.metadata and converts
     * the string value to the appropriate enum (e.g., "Snake" -> EnemyType::Snake).
     */
    rtype::common::components::EnemyType getEnemyTypeFromTile(const Tile& tile) const;
    
    /**
     * @brief Read all combat stats from tile metadata
     * @param tile Tile to read from
     * @param outHp Output health points
     * @param outVx Output horizontal velocity (negative = moves left)
     * @param outDamage Output damage value
     * @param outFireRate Output fire rate (seconds between shots)
     * 
     * Reads metadata keys: "health", "speed", "damage", "fire_rate".
     * Falls back to defaults if keys are missing or invalid.
     */
    void readTileStats(const Tile& tile, int& outHp, float& outVx, 
                       int& outDamage, float& outFireRate) const;
};

#endif // SERVER_ENEMY_SYSTEM_H
