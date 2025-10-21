/*
** Minimal server-side enemy spawner implementation
*/
#include "systems/ServerEnemySystem.h"
#include "rtype.h"
#include "packets.h"
#include "services/RoomService.h"
#include "services/PlayerService.h"
// Use project-relative includes like other server files
#include "../../../common/components/Position.h"
#include "../../../common/components/Velocity.h"
#include "../../../common/components/Health.h"
#include "../../../common/components/Team.h"
#include "../../../common/components/EnemyType.h"
// Server components
#include "components/RoomProperties.h"
#include "components/PlayerConn.h"
// Common player component
#include "../../../common/components/Player.h"

void ServerEnemySystem::Update(ECS::World &world, float deltaTime) {
    // --- spawn logic ---
    _spawnTimer += deltaTime;
    if (_spawnTimer >= SPAWN_INTERVAL) {
        _spawnTimer = 0.0f;

        // For each room that has started the game, spawn one enemy and notify players
        auto *rooms = root.world.GetAllComponents<rtype::server::components::RoomProperties>();
        if (rooms) {
            for (auto &pair : *rooms) {
                ECS::EntityID room = pair.first;
                // pair.second is a std::unique_ptr<RoomProperties>
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
    }

    // --- player state broadcast logic (ONLY during active games) ---
    _stateTick += deltaTime;
    if (_stateTick >= STATE_TICK_INTERVAL) {
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

    // --- entity cleanup (dead entities) ---
    auto healths = root.world.GetAllComponents<rtype::common::components::Health>();
    if (healths) {
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
}
