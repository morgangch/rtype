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
    updateBossSpawning(deltaTime);
    updateEnemySpawning(deltaTime);
    updatePlayerStateBroadcast(deltaTime);
    cleanupDeadEntities();
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning
// ============================================================================

void ServerEnemySystem::updateBossSpawning(float deltaTime) {
    _bossSpawnTimer += deltaTime;
    if (_bossSpawnTimer < BOSS_SPAWN_INTERVAL) return;
    
    _bossSpawnTimer = 0.0f;
    
    // For each active game room, check if boss exists, if not spawn one
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
                    // Boss exists, check if it's alive
                    auto* health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
                    if (health && health->isAlive && health->currentHp > 0) {
                        bossExists = true;
                        std::cout << "SERVER: Boss already exists (id=" << etPair.first << "), skipping spawn for room " << room << std::endl;
                        break;
                    }
                }
            }
        }
        
        // Only spawn boss if none exists
        if (!bossExists) {
            spawnBoss(root.world, room);
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Regular Enemy Spawning
// ============================================================================

void ServerEnemySystem::updateEnemySpawning(float deltaTime) {
    _spawnTimer += deltaTime;
    if (_spawnTimer < SPAWN_INTERVAL) return;
    
    _spawnTimer = 0.0f;

    // For each room that has started the game, spawn one enemy and notify players
    auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;
    
    for (auto &pair : *rooms) {
        ECS::EntityID room = pair.first;
        if (!pair.second) continue;
        auto *rp = pair.second.get();
        if (!rp || !rp->isGameStarted) continue;

        // Simple spawn position and properties
        float spawnX = 1280.0f + 24.0f; // off-screen right
        float spawnY = 100.0f + (rand() % 520); // between 100 and 620

        // Create enemy entity on server world
        auto enemy = root.world.CreateEntity();
        root.world.AddComponent<rtype::common::components::Position>(enemy, spawnX, spawnY, 0.0f);
        root.world.AddComponent<rtype::common::components::Velocity>(enemy, -100.0f, 0.0f, 100.0f);
        root.world.AddComponent<rtype::common::components::Health>(enemy, 1);
        root.world.AddComponent<rtype::common::components::Team>(enemy, rtype::common::components::TeamType::Enemy);
        root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(enemy, rtype::common::components::EnemyType::Basic);

        // Build SpawnEnemyPacket
        SpawnEnemyPacket pkt{};
        pkt.enemyId = static_cast<uint32_t>(enemy);
        pkt.enemyType = static_cast<uint16_t>(rtype::common::components::EnemyType::Basic);
        pkt.x = spawnX;
        pkt.y = spawnY;
        pkt.hp = 1;

        // Send to all players in the room
        auto players = root.world.GetAllComponents<rtype::common::components::Player>();
        if (!players) continue;
        for (auto &pp : *players) {
            ECS::EntityID pid = pp.first;
            auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
            if (!pconn) continue;
            if (pconn->room_code != room) continue;
            pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), SPAWN_ENEMY, nullptr, true);
        }
    }
}

// ============================================================================
// PRIVATE METHODS - Player State Broadcasting
// ============================================================================

void ServerEnemySystem::updatePlayerStateBroadcast(float deltaTime) {
    _stateTick += deltaTime;
    if (_stateTick < STATE_TICK_INTERVAL) return;
    
    _stateTick = 0.0f;

    // Only broadcast player states for rooms where game has started
    auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!rooms) return;
    
    for (auto &roomPair : *rooms) {
        ECS::EntityID room = roomPair.first;
        auto *rp = roomPair.second.get();
        if (!rp || !rp->isGameStarted) continue; // Skip rooms still in lobby
        
        auto players = root.world.GetAllComponents<rtype::common::components::Player>();
        if (!players) continue;
        
        for (auto &pair : *players) {
            ECS::EntityID pid = pair.first;
            auto* pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
            if (!pconn || pconn->room_code != room) continue; // Skip players not in this room
            
            auto* pos = root.world.GetComponent<rtype::common::components::Position>(pid);
            auto* health = root.world.GetComponent<rtype::common::components::Health>(pid);

            PlayerStatePacket s{};
            s.playerId = static_cast<uint32_t>(pid);
            s.x = pos ? pos->x : 0.0f;
            s.y = pos ? pos->y : 0.0f;
            s.dir = pos ? pos->rotation : 0.0f;
            s.hp = health ? static_cast<uint16_t>(health->currentHp) : 0;
            s.isAlive = health ? health->isAlive : false;

            // Send this player's state to everyone in the same room
            auto allPlayers = root.world.GetAllComponents<rtype::common::components::Player>();
            if (!allPlayers) continue;
            for (auto &pp : *allPlayers) {
                ECS::EntityID other = pp.first;
                auto* otherConn = root.world.GetComponent<rtype::server::components::PlayerConn>(other);
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

void ServerEnemySystem::cleanupDeadEntities() {
    auto healths = root.world.GetAllComponents<rtype::common::components::Health>();
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
            auto allPlayers = root.world.GetAllComponents<rtype::common::components::Player>();
            if (allPlayers) {
                for (auto &pp : *allPlayers) {
                    ECS::EntityID pid = pp.first;
                    auto* pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
                    if (!pconn) continue;
                    pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, true);
                }
            }

            toDestroy.push_back(eid);
        }
    }
    for (auto e : toDestroy) {
        root.world.DestroyEntity(e);
    }
}

// ============================================================================
// PRIVATE METHODS - Boss Spawning Helper
// ============================================================================

void ServerEnemySystem::spawnBoss(ECS::World& world, ECS::EntityID room) {
    // Boss spawn position (center-right of screen)
    float spawnX = 1280.0f - 100.0f;
    float spawnY = 360.0f; // Center Y of 720p screen
    
    // Create boss entity on server world
    auto boss = root.world.CreateEntity();
    root.world.AddComponent<rtype::common::components::Position>(boss, spawnX, spawnY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(boss, -50.0f, 0.0f, 50.0f);
    root.world.AddComponent<rtype::common::components::Health>(boss, 50); // Boss has 50 HP
    root.world.AddComponent<rtype::common::components::Team>(boss, rtype::common::components::TeamType::Enemy);
    root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(boss, rtype::common::components::EnemyType::Boss);
    
    std::cout << "SERVER: Spawning boss (id=" << boss << ") in room " << room << std::endl;
    
    // Build SpawnEnemyPacket for boss
    SpawnEnemyPacket pkt{};
    pkt.enemyId = static_cast<uint32_t>(boss);
    pkt.enemyType = static_cast<uint16_t>(rtype::common::components::EnemyType::Boss);
    pkt.x = spawnX;
    pkt.y = spawnY;
    pkt.hp = 50;
    
    // Send to all players in the room
    auto players = root.world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;
    for (auto &pp : *players) {
        ECS::EntityID pid = pp.first;
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue;
        if (pconn->room_code != room) continue;
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), SPAWN_ENEMY, nullptr, true);
        std::cout << "SERVER: Sent boss spawn packet to player " << pid << std::endl;
    }
}
