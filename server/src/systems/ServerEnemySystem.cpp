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
    : ECS::System("ServerEnemySystem", 5), _levelTimer(0.0f), _phase(EnemySpawnPhase::OnlyBasic), _bossSpawned(false), _stateTick(0.0f)
{
    // Example: configure spawn intervals for each type
    _enemyConfigs[rtype::common::components::EnemyType::Basic] = {rtype::common::components::EnemyType::Basic, 2.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Snake] = {rtype::common::components::EnemyType::Snake, 2.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Suicide] = {rtype::common::components::EnemyType::Suicide, 3.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Turret] = {rtype::common::components::EnemyType::Turret, 4.0f, 0.0f};
    _enemyConfigs[rtype::common::components::EnemyType::Shooter] = {rtype::common::components::EnemyType::Shooter, 4.0f, 0.0f};
    // Add more types as needed
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
    // Default: spawn classic boss (can be extended for multiple types)
    float spawnX = 1280.0f - 100.0f;
    float spawnY = 360.0f; // Center Y of 720p screen

    // Create boss entity on server world
    auto boss = world.CreateEntity();
    world.AddComponent<rtype::common::components::Position>(boss, spawnX, spawnY, 0.0f);
    world.AddComponent<rtype::common::components::Velocity>(boss, -50.0f, 0.0f, 50.0f);
    world.AddComponent<rtype::common::components::Health>(boss, 50); // Boss has 50 HP
    world.AddComponent<rtype::common::components::Team>(boss, rtype::common::components::TeamType::Enemy);
    world.AddComponent<rtype::common::components::EnemyTypeComponent>(boss, rtype::common::components::EnemyType::TankDestroyer);

    std::cout << "SERVER: Spawning boss (id=" << boss << ") in room " << room << std::endl;

    // Build SpawnEnemyPacket for boss
    SpawnEnemyPacket pkt{};
    pkt.enemyId = static_cast<uint32_t>(boss);
    pkt.enemyType = static_cast<uint16_t>(rtype::common::components::EnemyType::TankDestroyer);
    pkt.x = spawnX;
    pkt.y = spawnY;
    pkt.hp = 50;

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
                if (et && et->type == rtype::common::components::EnemyType::TankDestroyer) {
                    auto* health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
                    if (health && health->isAlive && health->currentHp > 0) {
                        bossExists = true;
                        break;
                    }
                }
            }
        }
        if (!bossExists) {
            spawnBoss(world, room, rtype::common::components::EnemyType::TankDestroyer);
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
                //spawnAllowed = (type == rtype::common::components::EnemyType::Basic);
                spawnAllowed = (type == rtype::common::components::EnemyType::Snake);
                //spawnAllowed = (type == rtype::common::components::EnemyType::Suicide);
                //spawnAllowed = (type == rtype::common::components::EnemyType::Turret);
                break;
            case EnemySpawnPhase::BasicAndAdvanced:
                // TO DO: change advanced enemy type
                spawnAllowed = (type == rtype::common::components::EnemyType::Basic || type == rtype::common::components::EnemyType::Shooter);
                break;
            case EnemySpawnPhase::BossAndAll:
                // TO DO: change boss type
                spawnAllowed = (type != rtype::common::components::EnemyType::TankDestroyer);
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
    // Spawn position logic (can be customized per type)
    float spawnX = 1280.0f + 24.0f;
    float spawnY = 100.0f + (rand() % 520);

    // Customize stats for each mob type
    int hp = 1;
    float vx = -100.0f;
    switch (type) {
        case rtype::common::components::EnemyType::Basic:
            // TO DO: set stats for Basic
            hp = 1; vx = -100.0f;
            break;
        case rtype::common::components::EnemyType::Snake:
            // TO DO: set stats for Snake
            hp = 1; vx = -110.0f;
            break;
        case rtype::common::components::EnemyType::Suicide:
            // TO DO: set stats for Suicide
            hp = 1; vx = -130.0f;
            break;
        case rtype::common::components::EnemyType::Turret:
            // TO DO: set stats for Turret
            hp = 3; vx = -80.0f;
            break;
        case rtype::common::components::EnemyType::Shooter:
            // TO DO: set stats for Shooter
            hp = 3; vx = -120.0f;
            break;
        case rtype::common::components::EnemyType::TankDestroyer:
            hp = 50; vx = -50.0f;
            break;
        default:
            // TO DO: add new types here
            break;
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
