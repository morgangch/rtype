/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerEnemySystem - Tile-driven enemy spawning implementation
*/
#include "systems/ServerEnemySystem.h"
#include "rtype.h"
#include "packets.h"
#include "services/RoomService.h"
#include "services/PlayerService.h"
#include <iostream>
#include <cstdlib>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/EnemyType.h>
#include <common/components/Shield.h>
#include "components/RoomProperties.h"
#include "components/PlayerConn.h"
#include <common/components/Player.h>
#include "senders.h"
#include "components/LinkedRoom.h"

// ============================================================================
// CONSTRUCTOR & INITIALIZATION
// ============================================================================

ServerEnemySystem::ServerEnemySystem()
    : ECS::System("ServerEnemySystem", 5), 
      _stateTick(0.0f),
      _mapLoaded(false),
      _nextClassicIndex(0),
      _nextEliteIndex(0),
      _nextBossIndex(0)
{
    // Initialize spawn intervals for different tile types
    _spawnIntervals[TileType::EnemyClassic] = 2.5f;  // Spawn every 2.5 seconds
    _spawnIntervals[TileType::EnemyElite] = 5.0f;    // Spawn every 5 seconds
    _spawnIntervals[TileType::EnemyBoss] = 0.0f;     // Manual trigger only
    
    // Initialize timers
    _spawnTimers[TileType::EnemyClassic] = 0.0f;
    _spawnTimers[TileType::EnemyElite] = 0.0f;
    _spawnTimers[TileType::EnemyBoss] = 0.0f;
    
    std::cout << "[ServerEnemySystem] Initialized with tile-driven spawning system" << std::endl;
}

// ============================================================================
// MAP LOADING
// ============================================================================

bool ServerEnemySystem::loadMap(const std::string &mapName) {
    std::cout << "[ServerEnemySystem] loadMap() called with: " << mapName << std::endl;
    
    try {
        auto &parser = MapParser::getInstance();
        std::cout << "[ServerEnemySystem] Calling MapParser::loadFromDirectory()..." << std::endl;
        const auto& allTiles = parser.loadFromDirectory(mapName);
        
        if (allTiles.empty()) {
            std::cerr << "[ServerEnemySystem] MapParser returned no tiles for map: " << mapName << std::endl;
            return false;
        }
        
        std::cout << "[ServerEnemySystem] MapParser succeeded (" << allTiles.size() << " total tiles), extracting enemy spawns..." << std::endl;

        // Classify enemy spawns by type
        _classicEnemySpawns = parser.getTilesByType(TileType::EnemyClassic);
        _eliteEnemySpawns = parser.getTilesByType(TileType::EnemyElite);
        _bossSpawns = parser.getTilesByType(TileType::EnemyBoss);
        
        std::cout << "[ServerEnemySystem] Loaded map '" << mapName << "' successfully:" << std::endl;
        std::cout << "  - Classic enemy spawns: " << _classicEnemySpawns.size() << std::endl;
        std::cout << "  - Elite enemy spawns: " << _eliteEnemySpawns.size() << std::endl;
        std::cout << "  - Boss spawns: " << _bossSpawns.size() << std::endl;
        
        // Log sample metadata from first classic enemy if available
        if (!_classicEnemySpawns.empty()) {
            const auto& tile = _classicEnemySpawns[0];
            std::cout << "  - Sample Classic Enemy Metadata:" << std::endl;
            for (const auto& [key, value] : tile.definition.metadata) {
                std::cout << "      " << key << ": " << value << std::endl;
            }
        }
        
        _mapLoaded = true;
        return true;
        
    } catch (const std::exception &e) {
        std::cerr << "[ServerEnemySystem] Exception loading map '" << mapName << "': " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// MAIN UPDATE LOOP
// ============================================================================

void ServerEnemySystem::Update(ECS::World &world, float deltaTime) {
    // Check if there's at least one active game room
    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    bool hasActiveGame = false;
    if (rooms) {
        for (auto &pair : *rooms) {
            if (pair.second && pair.second->isGameStarted) {
                hasActiveGame = true;
                break;
            }
        }
    }

    // Debug: Log state periodically (every 5 seconds)
    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer >= 5.0f) {
        debugTimer = 0.0f;
        std::cout << "[ServerEnemySystem] Status:" << std::endl;
        std::cout << "  - Map loaded: " << (_mapLoaded ? "YES" : "NO") << std::endl;
        std::cout << "  - Active game: " << (hasActiveGame ? "YES" : "NO") << std::endl;
        if (_mapLoaded) {
            std::cout << "  - Classic spawns: " << _classicEnemySpawns.size() << std::endl;
            std::cout << "  - Elite spawns: " << _eliteEnemySpawns.size() << std::endl;
            std::cout << "  - Classic timer: " << _spawnTimers[TileType::EnemyClassic] 
                      << "/" << _spawnIntervals[TileType::EnemyClassic] << std::endl;
            std::cout << "  - Elite timer: " << _spawnTimers[TileType::EnemyElite] 
                      << "/" << _spawnIntervals[TileType::EnemyElite] << std::endl;
        }
    }

    // Only run enemy spawning logic if game is active and map is loaded
    if (hasActiveGame && _mapLoaded) {
        updateEnemySpawning(world, deltaTime);
        updateObstacleSpawning(world, deltaTime);
    } else if (hasActiveGame && !_mapLoaded) {
        static bool warnedOnce = false;
        if (!warnedOnce) {
            std::cerr << "[ServerEnemySystem] WARNING: Game is active but no map is loaded!" << std::endl;
            warnedOnce = true;
        }
    }

    // Always update player state broadcast (needed even in lobby)
    updatePlayerStateBroadcast(world, deltaTime);
    cleanupDeadEntities(world);
}


// ============================================================================
// TILE-DRIVEN ENEMY SPAWNING
// ============================================================================

void ServerEnemySystem::updateEnemySpawning(ECS::World& world, float deltaTime) {
    // Get all active game rooms
    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;

    // Update spawn timers ONCE per update, not per room
    for (auto& [tileType, timer] : _spawnTimers) {
        timer += deltaTime;
    }

    // Check if we should spawn classic enemies
    bool shouldSpawnClassic = !_classicEnemySpawns.empty() && 
                               _spawnTimers[TileType::EnemyClassic] >= _spawnIntervals[TileType::EnemyClassic];
    
    // Check if we should spawn elite enemies
    bool shouldSpawnElite = !_eliteEnemySpawns.empty() && 
                             _spawnTimers[TileType::EnemyElite] >= _spawnIntervals[TileType::EnemyElite];

    // If nothing to spawn, return early
    if (!shouldSpawnClassic && !shouldSpawnElite) return;

    // Spawn enemies for each active room
    for (auto &pair : *rooms) {
        ECS::EntityID room = pair.first;
        if (!pair.second) continue;
        auto *rp = pair.second.get();
        if (!rp || !rp->isGameStarted) continue;

        // Spawn classic enemy in this room
        if (shouldSpawnClassic) {
            const Tile& tile = _classicEnemySpawns[_nextClassicIndex];
            spawnEnemyFromTile(world, room, tile);
        }

        // Spawn elite enemy in this room
        if (shouldSpawnElite) {
            const Tile& tile = _eliteEnemySpawns[_nextEliteIndex];
            spawnEnemyFromTile(world, room, tile);
        }
    }

    // Reset timers and advance indices AFTER spawning in all rooms
    if (shouldSpawnClassic) {
        _nextClassicIndex = (_nextClassicIndex + 1) % _classicEnemySpawns.size();
        _spawnTimers[TileType::EnemyClassic] = 0.0f;
        std::cout << "[ServerEnemySystem] Classic spawn timer reset, next index: " << _nextClassicIndex << std::endl;
    }

    if (shouldSpawnElite) {
        _nextEliteIndex = (_nextEliteIndex + 1) % _eliteEnemySpawns.size();
        _spawnTimers[TileType::EnemyElite] = 0.0f;
        std::cout << "[ServerEnemySystem] Elite spawn timer reset, next index: " << _nextEliteIndex << std::endl;
    }
}

void ServerEnemySystem::spawnEnemyFromTile(ECS::World& world, ECS::EntityID room, const Tile& tile) {
    // Read all stats from tile metadata
    int hp = 1;
    float vx = -100.0f;
    int damage = 1;
    float fireRate = 2.5f;
    readTileStats(tile, hp, vx, damage, fireRate);
    
    // Determine enemy type from metadata
    rtype::common::components::EnemyType enemyType = getEnemyTypeFromTile(tile);
    
    // Calculate spawn position
    // Tile coordinates are used as base, but we add some randomization
    constexpr float SPAWN_X_BASE = 1280.0f + 24.0f; // Right edge + padding
    constexpr float SCREEN_MIN_Y = 50.0f;
    constexpr float SCREEN_MAX_Y = 670.0f;
    
    float spawnX = SPAWN_X_BASE;
    
    // Use tile.y as a guide, but add randomization for variety
    // This prevents all enemies from spawning at exact tile boundaries
    float baseY = 100.0f + tile.y * (SCREEN_MAX_Y - SCREEN_MIN_Y) / 7.0f; // Distribute across 7 rows
    float randomOffset = (static_cast<float>(std::rand() % 60) - 30.0f); // ±30px random
    float spawnY = baseY + randomOffset;
    
    // Clamp Y position to screen bounds (assuming 720p)
    if (spawnY < SCREEN_MIN_Y) spawnY = SCREEN_MIN_Y;
    if (spawnY > SCREEN_MAX_Y) spawnY = SCREEN_MAX_Y;
    
    // Create enemy entity
    auto enemy = world.CreateEntity();
    world.AddComponent<rtype::common::components::Position>(enemy, spawnX, spawnY, 0.0f);
    world.AddComponent<rtype::common::components::Velocity>(enemy, vx, 0.0f, std::abs(vx));
    world.AddComponent<rtype::common::components::Health>(enemy, hp);
    world.AddComponent<rtype::common::components::Team>(enemy, rtype::common::components::TeamType::Enemy);
    world.AddComponent<rtype::common::components::EnemyTypeComponent>(enemy, enemyType);
    world.AddComponent<rtype::server::components::LinkedRoom>(enemy, room);
    
    // Add shield if specified in metadata
    auto& metadata = tile.definition.metadata;
    auto shieldIt = metadata.find("has_shield");
    if (shieldIt != metadata.end()) {
        if (shieldIt->second == "cyclic") {
            world.AddComponent<rtype::common::components::ShieldComponent>(
                enemy, rtype::common::components::ShieldType::Cyclic, true
            );
            std::cout << "[ServerEnemySystem] Added cyclic shield to enemy " << enemy << std::endl;
        } else if (shieldIt->second == "red") {
            // Red shield requires charged shots
            world.AddComponent<rtype::common::components::ShieldComponent>(
                enemy, rtype::common::components::ShieldType::Red, true, 2
            );
            std::cout << "[ServerEnemySystem] Added red shield to enemy " << enemy 
                      << " (requires 2 charged hits)" << std::endl;
        }
    }
    
    std::cout << "[ServerEnemySystem] Spawned " << tileTypeToString(tile.type) 
              << " enemy type=" << (int)enemyType 
              << " id=" << enemy 
              << " at (" << spawnX << ", " << spawnY << ")"
              << " hp=" << hp 
              << " in room " << room << std::endl;
    
    // Broadcast spawn to all clients in the room
    rtype::server::network::senders::broadcast_enemy_spawn(
        room, static_cast<uint32_t>(enemy), enemyType, spawnX, spawnY, hp
    );
}

// ============================================================================
// TILE METADATA PARSING
// ============================================================================

rtype::common::components::EnemyType ServerEnemySystem::getEnemyTypeFromTile(const Tile& tile) const {
    auto& metadata = tile.definition.metadata;
    auto typeIt = metadata.find("enemy_type");
    
    if (typeIt == metadata.end()) {
        std::cerr << "[ServerEnemySystem] WARNING: No enemy_type in tile metadata, defaulting to Basic" << std::endl;
        return rtype::common::components::EnemyType::Basic;
    }
    
    const std::string& typeStr = typeIt->second;
    
    // Map string to enum
    if (typeStr == "Basic") return rtype::common::components::EnemyType::Basic;
    if (typeStr == "Snake") return rtype::common::components::EnemyType::Snake;
    if (typeStr == "Suicide") return rtype::common::components::EnemyType::Suicide;
    if (typeStr == "Pata") return rtype::common::components::EnemyType::Pata;
    if (typeStr == "Shielded") return rtype::common::components::EnemyType::Shielded;
    if (typeStr == "Flanker") return rtype::common::components::EnemyType::Flanker;
    if (typeStr == "Turret") return rtype::common::components::EnemyType::Turret;
    if (typeStr == "Waver") return rtype::common::components::EnemyType::Waver;
    if (typeStr == "TankDestroyer") return rtype::common::components::EnemyType::TankDestroyer;
    if (typeStr == "Serpent") return rtype::common::components::EnemyType::Serpent;
    if (typeStr == "Fortress") return rtype::common::components::EnemyType::Fortress;
    if (typeStr == "Core") return rtype::common::components::EnemyType::Core;
    
    std::cerr << "[ServerEnemySystem] WARNING: Unknown enemy_type '" << typeStr 
              << "', defaulting to Basic" << std::endl;
    return rtype::common::components::EnemyType::Basic;
}

void ServerEnemySystem::readTileStats(const Tile& tile, int& outHp, float& outVx, 
                                       int& outDamage, float& outFireRate) const {
    // Default values
    outHp = 1;
    outVx = -100.0f;
    outDamage = 1;
    outFireRate = 2.5f;
    
    auto& metadata = tile.definition.metadata;
    
    // Read health
    auto hpIt = metadata.find("health");
    if (hpIt != metadata.end()) {
        try {
            outHp = std::stoi(hpIt->second);
        } catch (...) {
            std::cerr << "[ServerEnemySystem] Invalid health value: " << hpIt->second << std::endl;
        }
    }
    
    // Read speed (negative for left movement)
    auto speedIt = metadata.find("speed");
    if (speedIt != metadata.end()) {
        try {
            outVx = -std::stof(speedIt->second);
        } catch (...) {
            std::cerr << "[ServerEnemySystem] Invalid speed value: " << speedIt->second << std::endl;
        }
    }
    
    // Read damage
    auto damageIt = metadata.find("damage");
    if (damageIt != metadata.end()) {
        try {
            outDamage = std::stoi(damageIt->second);
        } catch (...) {
            std::cerr << "[ServerEnemySystem] Invalid damage value: " << damageIt->second << std::endl;
        }
    }
    
    // Read fire rate
    auto fireRateIt = metadata.find("fire_rate");
    if (fireRateIt != metadata.end()) {
        try {
            outFireRate = std::stof(fireRateIt->second);
        } catch (...) {
            std::cerr << "[ServerEnemySystem] Invalid fire_rate value: " << fireRateIt->second << std::endl;
        }
    }
}

// ============================================================================
// OBSTACLE SPAWNING
// ============================================================================

void ServerEnemySystem::updateObstacleSpawning(ECS::World& world, float deltaTime) {
    _meteoriteTimer += deltaTime;
    _debrisTimer += deltaTime;

    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;

    // Spawn meteorites individually at random Y positions
    if (_meteoriteTimer >= _meteoriteNext) {
        _meteoriteTimer = 0.0f;
        _meteoriteNext = 2.0f + static_cast<float>(rand() % 3000) / 1000.0f; // 2..5s

        for (auto &pair : *rooms) {
            ECS::EntityID room = pair.first;
            if (!pair.second) continue;
            auto *rp = pair.second.get();
            if (!rp || !rp->isGameStarted) continue;

            float spawnX = 1280.0f + 40.0f;
            float spawnY = 60.0f + (rand() % 600); // keep within screen
            int hp = 5;
            float speed = 180.0f + static_cast<float>(rand() % 80); // 180..260

            auto e = world.CreateEntity();
            world.AddComponent<rtype::common::components::Position>(e, spawnX, spawnY, 0.0f);
            world.AddComponent<rtype::common::components::Velocity>(e, -speed, 0.0f, speed);
            world.AddComponent<rtype::common::components::Health>(e, hp);
            world.AddComponent<rtype::common::components::Team>(e, rtype::common::components::TeamType::Enemy);
            world.AddComponent<rtype::common::components::EnemyTypeComponent>(e, rtype::common::components::EnemyType::Meteorite);
            world.AddComponent<rtype::server::components::LinkedRoom>(e, room);

            rtype::server::network::senders::broadcast_enemy_spawn(room, e, rtype::common::components::EnemyType::Meteorite, spawnX, spawnY, hp);
        }
    }

    // Spawn debris rows occasionally (2..6 count)
    if (_debrisTimer >= _debrisNext) {
        _debrisTimer = 0.0f;
        _debrisNext = 6.0f + static_cast<float>(rand() % 6000) / 1000.0f; // 6..12s

        for (auto &pair : *rooms) {
            ECS::EntityID room = pair.first;
            if (!pair.second) continue;
            auto *rp = pair.second.get();
            if (!rp || !rp->isGameStarted) continue;

            int count = 2 + (rand() % 3); // 2..4
            spawnDebrisRow(world, room, count);
        }
    }
}

void ServerEnemySystem::spawnDebrisRow(ECS::World& world, ECS::EntityID room, int count) {
    // Clamp
    if (count < 2) count = 2; 
    if (count > 4) count = 4;
    
    // Vertical spacing across safe band (keep some margins)
    float top = 80.0f;
    float bottom = 640.0f;
    float step = (bottom - top) / (count - 1);

    for (int i = 0; i < count; ++i) {
        float spawnX = 1280.0f + 60.0f;
        float spawnY = top + step * i;
        int hp = 1000;
        float speed = 80.0f + static_cast<float>(rand() % 40); // 80..120

        auto e = world.CreateEntity();
        world.AddComponent<rtype::common::components::Position>(e, spawnX, spawnY, 0.0f);
        world.AddComponent<rtype::common::components::Velocity>(e, -speed, 0.0f, speed);
        world.AddComponent<rtype::common::components::Health>(e, hp);
        world.AddComponent<rtype::common::components::Team>(e, rtype::common::components::TeamType::Enemy);
        world.AddComponent<rtype::common::components::EnemyTypeComponent>(e, rtype::common::components::EnemyType::Debri);
        world.AddComponent<rtype::server::components::LinkedRoom>(e, room);

        rtype::server::network::senders::broadcast_enemy_spawn(room, e, rtype::common::components::EnemyType::Debri, spawnX, spawnY, hp);
    }
}

// ============================================================================
// PLAYER STATE BROADCASTING
// ============================================================================

void ServerEnemySystem::updatePlayerStateBroadcast(ECS::World& world, float deltaTime)
{
    _stateTick += deltaTime;
    if (_stateTick < STATE_TICK_INTERVAL) return;
    
    _stateTick = 0.0f;

    // Only broadcast player states for rooms where game has started
    auto *rooms = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;
    
    for (auto &roomPair : *rooms) {
        ECS::EntityID room = roomPair.first;
        auto *rp = roomPair.second.get();
        if (!rp || !rp->isGameStarted) continue;

        auto players = world.GetAllComponents<rtype::common::components::Player>();
        if (!players) continue;
        
        for (auto &pair : *players) {
            ECS::EntityID pid = pair.first;

            // Determine the room for this player entity
            ECS::EntityID playerRoom = 0;
            auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(pid);
            if (pconn) {
                playerRoom = pconn->room_code;
            } else {
                auto* linked = world.GetComponent<rtype::server::components::LinkedRoom>(pid);
                if (linked) playerRoom = linked->room_id;
            }
            if (playerRoom != room) continue;

            auto* pos = world.GetComponent<rtype::common::components::Position>(pid);
            auto* health = world.GetComponent<rtype::common::components::Health>(pid);
            if (!pos || !health) continue;

            // Send this player's state to every networked client in the same room
            auto allPlayers = world.GetAllComponents<rtype::common::components::Player>();
            if (!allPlayers) continue;
            for (auto &pp : *allPlayers) {
                ECS::EntityID other = pp.first;
                auto* other_room = world.GetComponent<rtype::server::components::LinkedRoom>(other);
                if (!other_room) continue;
                if (other_room->room_id != room) continue;

                auto* otherConn = world.GetComponent<rtype::server::components::PlayerConn>(other);
                if (!otherConn) continue;

                rtype::server::network::senders::send_player_state(
                    other, pid, pos->x, pos->y, pos->rotation, health->currentHp, health->isAlive
                );
            }
        }
    }
}

// ============================================================================
// ENTITY CLEANUP
// ============================================================================

void ServerEnemySystem::cleanupDeadEntities(ECS::World& world) {
    auto healths = world.GetAllComponents<rtype::common::components::Health>();
    if (!healths) return;
    
    std::vector<ECS::EntityID> toDestroy;
    for (auto &pair : *healths) {
        ECS::EntityID eid = pair.first;
        auto* h = pair.second.get();
        if (!h) continue;
        if (!h->isAlive || h->currentHp <= 0) {
            auto room = world.GetComponent<rtype::server::components::LinkedRoom>(eid);
            if (room)
                rtype::server::network::senders::broadcast_entity_destroy(
                    room->room_id, static_cast<uint32_t>(eid), 1
                );
            toDestroy.push_back(eid);
        }
    }
    for (auto e : toDestroy) {
        world.DestroyEntity(e);
    }
}