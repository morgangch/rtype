/*
** Minimal server-side enemy spawner implementation
*/
#include "systems/ServerEnemySystem.h"
#include "rtype.h"
#include "packets.h"
#include "services/RoomService.h"
#include "services/PlayerService.h"
#include <iostream>
// Use project-relative includes like other server files
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/EnemyType.h>
// Server components
#include "components/RoomProperties.h"
#include "components/PlayerConn.h"
// Common player component
#include <common/components/Player.h>

void ServerEnemySystem::Update(ECS::World &world, float deltaTime) {
    updatePhase(deltaTime);
    updateEnemySpawning(world, deltaTime);
    updateBossSpawning(world, deltaTime);
    updatePlayerStateBroadcast(world, deltaTime);
    cleanupDeadEntities(world);
}

ServerEnemySystem::ServerEnemySystem()
    : ECS::System("ServerEnemySystem", 5), 
      _levelTimer(0.0f), 
      _phase(EnemySpawnPhase::OnlyBasic), 
      _bossSpawned(false), 
      _stateTick(0.0f),
      _mapLoaded(false),
      _nextClassicIndex(0),
      _nextEliteIndex(0),
      _nextBossIndex(0)
{
    // Example: configure spawn intervals for each type
    _enemyConfigs[rtype::common::components::EnemyType::Basic] = {rtype::common::components::EnemyType::Basic, 2.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Snake] = {rtype::common::components::EnemyType::Snake, 2.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Suicide] = {rtype::common::components::EnemyType::Suicide, 3.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Turret] = {rtype::common::components::EnemyType::Turret, 4.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Shooter] = {rtype::common::components::EnemyType::Shooter, 4.0f, 0.0f};
    // Add more types as needed
}

bool ServerEnemySystem::loadMap(const std::string& mapDir) {
    try {
        auto& parser = MapParser::getInstance();
        parser.loadFromDirectory(mapDir);
        
        // Get all tiles by type
        _classicEnemySpawns = parser.getTilesByType(TileType::EnemyClassic);
        _eliteEnemySpawns = parser.getTilesByType(TileType::EnemyElite);
        _bossSpawns = parser.getTilesByType(TileType::EnemyBoss);
        
        std::cout << "SERVER: Loaded map " << mapDir << " with:"
                  << "\n  Classic enemy spawns: " << _classicEnemySpawns.size()
                  << "\n  Elite enemy spawns: " << _eliteEnemySpawns.size()
                  << "\n  Boss spawns: " << _bossSpawns.size() << std::endl;
        
        _mapLoaded = true;
        _nextClassicIndex = 0;
        _nextEliteIndex = 0;
        _nextBossIndex = 0;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "SERVER: Error loading map: " << e.what() << std::endl;
        _mapLoaded = false;
        return false;
    }
}

std::pair<float, float> ServerEnemySystem::getSpawnPosition(
    const std::vector<Tile>& tiles,
    size_t& index,
    float defaultX,
    float defaultY
) {
    if (tiles.empty()) {
        // Fallback: return default position
        return {defaultX, defaultY};
    }
    
    // Get tile at current index
    const auto& tile = tiles[index];
    
    // Increment and wrap index for next spawn
    index = (index + 1) % tiles.size();
    
    // Convert tile coordinates to screen position
    // Assuming each tile is 32x32 pixels (adjust as needed)
    constexpr float TILE_SIZE = 32.0f;
    // Place tile X relative to a default spawn X (typically off-screen to the right)
    // This ensures map-defined tiles (which are column indices) spawn off-screen instead
    // of at the left edge (x==0).
    float x = defaultX + tile.x * TILE_SIZE;
    // For Y, use defaultY as the top offset and add tile row * TILE_SIZE so map rows
    // map to vertical positions relative to the playable area.
    float y = defaultY + tile.y * TILE_SIZE;
    
    return {x, y};
}

void ServerEnemySystem::readEnemyStats(const Tile& tile, int& outHp, float& outVx) {
    // Default values
    outHp = 1;
    outVx = -100.0f;
    
    // Read from tile metadata if available
    auto& metadata = tile.definition.metadata;
    
    auto hpIt = metadata.find("health");
    if (hpIt != metadata.end()) {
        try {
            outHp = std::stoi(hpIt->second);
        } catch (...) {
            std::cerr << "SERVER: Invalid health value in tile metadata: " << hpIt->second << std::endl;
        }
    }
    
    auto speedIt = metadata.find("speed");
    if (speedIt != metadata.end()) {
        try {
            outVx = -std::stof(speedIt->second); // Negative for left movement
        } catch (...) {
            std::cerr << "SERVER: Invalid speed value in tile metadata: " << speedIt->second << std::endl;
        }
    }
}

void ServerEnemySystem::updatePhase(float deltaTime)
{
    _levelTimer += deltaTime;
    if (_levelTimer < 60.0f) {
        _phase = EnemySpawnPhase::OnlyBasic;
    } else if (_levelTimer < 180.0f) {
        _phase = EnemySpawnPhase::BasicAndAdvanced;
    } else {
        _phase = EnemySpawnPhase::BossAndAll;
    }
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning
// ============================================================================

void ServerEnemySystem::spawnBoss(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType bossType) {
    // Determine spawn position based on map or fallback to default
    float spawnX, spawnY;
    int hp = 50;
    float vx = -50.0f;
    
    if (_mapLoaded && !_bossSpawns.empty()) {
        // Use map-driven boss spawn
        auto pos = getSpawnPosition(_bossSpawns, _nextBossIndex, 1280.0f - 100.0f, 360.0f);
        spawnX = pos.first;
        spawnY = pos.second;
        
        // Read stats from tile if available
        size_t tileIdx = (_nextBossIndex == 0) ? _bossSpawns.size() - 1 : _nextBossIndex - 1;
        readEnemyStats(_bossSpawns[tileIdx], hp, vx);
    } else {
        // Fallback: use hardcoded center position
        spawnX = 1280.0f - 100.0f;
        spawnY = 360.0f; // Center Y of 720p screen
    }

    // Create boss entity on server world
    auto boss = world.CreateEntity();
    world.AddComponent<rtype::common::components::Position>(boss, spawnX, spawnY, 0.0f);
    world.AddComponent<rtype::common::components::Velocity>(boss, vx, 0.0f, std::abs(vx));
    world.AddComponent<rtype::common::components::Health>(boss, hp);
    world.AddComponent<rtype::common::components::Team>(boss, rtype::common::components::TeamType::Enemy);
    world.AddComponent<rtype::common::components::EnemyTypeComponent>(boss, rtype::common::components::EnemyType::Boss);

    std::cout << "SERVER: Spawning boss (id=" << boss << ") in room " << room 
              << " at (" << spawnX << ", " << spawnY << ") with " << hp << " HP" << std::endl;

    // Build SpawnEnemyPacket for boss
    SpawnEnemyPacket pkt{};
    pkt.enemyId = static_cast<uint32_t>(boss);
    pkt.enemyType = static_cast<uint16_t>(rtype::common::components::EnemyType::Boss);
    pkt.x = spawnX;
    pkt.y = spawnY;
    pkt.hp = hp;

    // Send to all players in the room
    auto players = world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;
    for (auto &pp : *players) {
        ECS::EntityID pid = pp.first;
        auto *pconn = world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue;
        if (pconn->room_code != room) continue;
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), SPAWN_ENEMY, nullptr, true);
        std::cout << "SERVER: Sent boss spawn packet to player " << pid << std::endl;
    }
}

void ServerEnemySystem::updateBossSpawning(ECS::World& world, float deltaTime) {
    // Only spawn boss if phase is BossAndAll and not already spawned
    if (_phase != EnemySpawnPhase::BossAndAll || _bossSpawned)
        return;

    auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;
    for (auto &pair : *rooms) {
        ECS::EntityID room = pair.first;
        if (!pair.second) continue;
        auto *rp = pair.second.get();
        if (!rp || !rp->isGameStarted) continue;

        // Check if a boss already exists in this room
        bool bossExists = false;
        auto* enemyTypes = root.world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
        if (enemyTypes) {
            for (auto& etPair : *enemyTypes) {
                auto* et = etPair.second.get();
                if (et && et->type == rtype::common::components::EnemyType::Boss) {
                    auto* health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
                    if (health && health->isAlive && health->currentHp > 0) {
                        bossExists = true;
                        break;
                    }
                }
            }
        }
        if (!bossExists) {
            spawnBoss(world, room, rtype::common::components::EnemyType::Boss);
            _bossSpawned = true;
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Regular Enemy Spawning
// ============================================================================

void ServerEnemySystem::updateEnemySpawning(ECS::World& world, float deltaTime) {
    // For each enemy type in config, check timer and spawn if needed
    for (auto& [type, config] : _enemyConfigs) {
        config.timer += deltaTime;
        if (config.timer < config.interval)
            continue;
        config.timer = 0.0f;

        // Only spawn types allowed in current phase
        bool spawnAllowed = false;
        switch (_phase) {
            case EnemySpawnPhase::OnlyBasic:
                // TO DO: change basic enemy type
                spawnAllowed = (type == rtype::common::components::EnemyType::Basic);
                //spawnAllowed = (type == rtype::common::components::EnemyType::Snake);
                //spawnAllowed = (type == rtype::common::components::EnemyType::Suicide);
                //spawnAllowed = (type == rtype::common::components::EnemyType::Turret);
                break;
            case EnemySpawnPhase::BasicAndAdvanced:
                // TO DO: change advanced enemy type
                spawnAllowed = (type == rtype::common::components::EnemyType::Basic || type == rtype::common::components::EnemyType::Shooter);
                break;
            case EnemySpawnPhase::BossAndAll:
                // TO DO: change boss type
                spawnAllowed = (type != rtype::common::components::EnemyType::Boss);
                break;
        }
        if (!spawnAllowed) continue;

        // For each room that has started the game, spawn enemy of this type
        auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
        if (!rooms) continue;
        for (auto &pair : *rooms) {
            ECS::EntityID room = pair.first;
            if (!pair.second) continue;
            auto *rp = pair.second.get();
            if (!rp || !rp->isGameStarted) continue;
            spawnEnemy(world, room, type);
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Player State Broadcasting
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
        if (!rp || !rp->isGameStarted) continue; // Skip rooms still in lobby

        auto players = world.GetAllComponents<rtype::common::components::Player>();
        if (!players) continue;
        
        for (auto &pair : *players) {
            ECS::EntityID pid = pair.first;
            auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(pid);
            if (!pconn || pconn->room_code != room) continue; // Skip players not in this room

            auto* pos = world.GetComponent<rtype::common::components::Position>(pid);
            auto* health = world.GetComponent<rtype::common::components::Health>(pid);

            PlayerStatePacket s{};
            s.playerId = static_cast<uint32_t>(pid);
            s.x = pos ? pos->x : 0.0f;
            s.y = pos ? pos->y : 0.0f;
            s.dir = pos ? pos->rotation : 0.0f;
            s.hp = health ? static_cast<uint16_t>(health->currentHp) : 0;
            s.isAlive = health ? health->isAlive : false;

            // Send this player's state to everyone in the same room
            auto allPlayers = world.GetAllComponents<rtype::common::components::Player>();
            if (!allPlayers) continue;
            for (auto &pp : *allPlayers) {
                ECS::EntityID other = pp.first;
                auto* otherConn = world.GetComponent<rtype::server::components::PlayerConn>(other);
                if (!otherConn) continue;
                // restrict to same room
                if (otherConn->room_code != room) continue;
                otherConn->packet_manager.sendPacketBytesSafe(&s, sizeof(s), PLAYER_STATE, nullptr, true);
            }
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Entity Cleanup
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
            EntityDestroyPacket pkt{};
            pkt.entityId = static_cast<uint32_t>(eid);
            pkt.reason = 1; // killed

            // Broadcast destroy packet to all players in the world (could restrict by room)
            auto allPlayers = world.GetAllComponents<rtype::common::components::Player>();
            if (allPlayers) {
                for (auto &pp : *allPlayers) {
                    ECS::EntityID pid = pp.first;
                    auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(pid);
                    if (!pconn) continue;
                    pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, true);
                }
            }

            toDestroy.push_back(eid);
        }
    }
    for (auto e : toDestroy) {
        world.DestroyEntity(e);
    }
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning Helper
// ============================================================================

void ServerEnemySystem::spawnEnemy(ECS::World& world, ECS::EntityID room, rtype::common::components::EnemyType type) {
    // Determine spawn position based on map or fallback to defaults
    float spawnX, spawnY;
    int hp = 1;
    float vx = -100.0f;
    
    if (_mapLoaded) {
        // Use map-driven spawning
        switch (type) {
            case rtype::common::components::EnemyType::Basic:
            case rtype::common::components::EnemyType::Snake:
            case rtype::common::components::EnemyType::Suicide: {
                // Use classic enemy spawns
                // Use a fixed vertical base (100.0f) so tile.row maps deterministically to screen Y
                auto pos = getSpawnPosition(_classicEnemySpawns, _nextClassicIndex, 1280.0f + 24.0f, 100.0f);
                spawnX = pos.first;
                spawnY = pos.second;
                
                // Read stats from tile if available
                if (!_classicEnemySpawns.empty()) {
                    size_t tileIdx = (_nextClassicIndex == 0) ? _classicEnemySpawns.size() - 1 : _nextClassicIndex - 1;
                    readEnemyStats(_classicEnemySpawns[tileIdx], hp, vx);
                }
                break;
            }
            case rtype::common::components::EnemyType::Turret:
            case rtype::common::components::EnemyType::Shooter: {
                // Use elite enemy spawns
                auto pos = getSpawnPosition(_eliteEnemySpawns, _nextEliteIndex, 1280.0f + 24.0f, 100.0f);
                spawnX = pos.first;
                spawnY = pos.second;
                
                // Read stats from tile if available
                if (!_eliteEnemySpawns.empty()) {
                    size_t tileIdx = (_nextEliteIndex == 0) ? _eliteEnemySpawns.size() - 1 : _nextEliteIndex - 1;
                    readEnemyStats(_eliteEnemySpawns[tileIdx], hp, vx);
                }
                break;
            }
            default:
                // Fallback for unknown types
                spawnX = 1280.0f + 24.0f;
                spawnY = 100.0f + (rand() % 520);
                break;
        }
    } else {
        // Fallback: use hardcoded positions and stats
        spawnX = 1280.0f + 24.0f;
        spawnY = 100.0f + (rand() % 520);
        
        // Hardcoded stats for each mob type
        switch (type) {
            case rtype::common::components::EnemyType::Basic:
                hp = 1; vx = -100.0f;
                break;
            case rtype::common::components::EnemyType::Snake:
                hp = 2; vx = -110.0f;
                break;
            case rtype::common::components::EnemyType::Suicide:
                hp = 1; vx = -130.0f;
                break;
            case rtype::common::components::EnemyType::Turret:
                hp = 3; vx = -80.0f;
                break;
            case rtype::common::components::EnemyType::Shooter:
                hp = 3; vx = -120.0f;
                break;
            case rtype::common::components::EnemyType::Boss:
                hp = 50; vx = -50.0f;
                break;
            default:
                break;
        }
    }

    auto enemy = root.world.CreateEntity();
    root.world.AddComponent<rtype::common::components::Position>(enemy, spawnX, spawnY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(enemy, vx, 0.0f, std::abs(vx));
    root.world.AddComponent<rtype::common::components::Health>(enemy, hp);
    root.world.AddComponent<rtype::common::components::Team>(enemy, rtype::common::components::TeamType::Enemy);
    root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(enemy, type);

    SpawnEnemyPacket pkt{};
    pkt.enemyId = static_cast<uint32_t>(enemy);
    pkt.enemyType = static_cast<uint16_t>(type);
    pkt.x = spawnX;
    pkt.y = spawnY;
    pkt.hp = hp;

    auto players = root.world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;
    for (auto &pp : *players) {
        ECS::EntityID pid = pp.first;
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue;
        if (pconn->room_code != room) continue;
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), SPAWN_ENEMY, nullptr, true);
    }
}
