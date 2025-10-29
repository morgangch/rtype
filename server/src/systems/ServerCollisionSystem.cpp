/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Server-side collision detection system implementation
*/

#include "systems/ServerCollisionSystem.h"
#include "components/PlayerConn.h"
#include "components/RoomProperties.h"
#include "components/LobbyState.h"
#include "services/PlayerService.h"
#include "rtype.h"
#include "packetmanager.h"
#include <common/systems/CollisionSystem.h>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace rtype::server::systems {

void ServerCollisionSystem::Update(ECS::World& world, float deltaTime) {
    rtype::common::systems::CollisionHandlers handlers;
    std::vector<ECS::EntityID> toDestroy;

    handlers.onPlayerVsEnemy = [this](ECS::EntityID player, ECS::EntityID enemy, ECS::World& world) {
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        if (!playerHealth) return;

        playerHealth->currentHp -= 1;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        if (playerHealth->currentHp <= 0) {
            playerHealth->isAlive = false;
        }

        broadcastPlayerStateImmediate(world, player);
    };

    handlers.onPlayerProjectileVsEnemy = [this, &toDestroy, &handlers](ECS::EntityID proj, ECS::EntityID enemy, ECS::World& world) {
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemy);
        if (!projData || !enemyHealth) return;

        enemyHealth->currentHp -= projData->damage;

        if (enemyHealth->currentHp <= 0) {
            enemyHealth->isAlive = false;

            auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(enemy);
            if (enemyType && enemyType->type == rtype::common::components::EnemyType::Suicide && handlers.onSuicideExplosion) {
                handlers.onSuicideExplosion(enemy, world);
            }

            toDestroy.push_back(enemy);
        }

        if (!projData->piercing) {
            toDestroy.push_back(proj);
        }
    };

    handlers.onEnemyProjectileVsPlayer = [this, &toDestroy](ECS::EntityID proj, ECS::EntityID player, ECS::World& world) {
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        if (!projData || !playerHealth) return;

        playerHealth->currentHp -= projData->damage;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        if (playerHealth->currentHp <= 0) {
            playerHealth->isAlive = false;
        }

        broadcastPlayerStateImmediate(world, player);
        toDestroy.push_back(proj);
    };

    handlers.onSuicideExplosion = [this, &toDestroy](ECS::EntityID suicideEnemy, ECS::World& world) {
        auto* enemyPos = world.GetComponent<rtype::common::components::Position>(suicideEnemy);
        if (!enemyPos) return;

        const float EXPLOSION_RADIUS = 100.0f;
        const int EXPLOSION_DAMAGE = 2;

        auto* players = world.GetAllComponents<rtype::common::components::Player>();
        if (players) {
            for (auto& [playerEntity, playerPtr] : *players) {
                auto* playerPos = world.GetComponent<rtype::common::components::Position>(playerEntity);
                auto* playerHealth = world.GetComponent<rtype::common::components::Health>(playerEntity);

                if (!playerPos || !playerHealth || playerHealth->invulnerable) continue;

                float dx = playerPos->x - enemyPos->x;
                float dy = playerPos->y - enemyPos->y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance <= EXPLOSION_RADIUS) {
                    playerHealth->currentHp -= EXPLOSION_DAMAGE;
                    playerHealth->invulnerable = true;
                    playerHealth->invulnerabilityTimer = 1.0f;

                    if (playerHealth->currentHp <= 0) {
                        playerHealth->isAlive = false;
                    }

                    broadcastPlayerStateImmediate(world, playerEntity);
                }
            }
        }

        toDestroy.push_back(suicideEnemy);
    };

    rtype::common::systems::CollisionSystem::update(world, deltaTime, handlers);

    std::sort(toDestroy.begin(), toDestroy.end());
    toDestroy.erase(std::unique(toDestroy.begin(), toDestroy.end()), toDestroy.end());

    for (auto entity : toDestroy) {
        broadcastEntityDestroyToAllRooms(world, entity);
        world.DestroyEntity(entity);
    }
}
void ServerCollisionSystem::broadcastEntityDestroyToAllRooms(
    ECS::World& world,
    ECS::EntityID entityId) {

    auto* roomProps = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!roomProps) return;

    EntityDestroyPacket pkt{};
    pkt.entityId = static_cast<uint32_t>(entityId);

    for (const auto& [roomEntity, roomPtr] : *roomProps) {
        if (!roomPtr->isGameStarted) continue;

        auto players = rtype::server::services::player_service::findPlayersByRoomCode(roomEntity);

        for (auto playerId : players) {
            auto* lobbyState = world.GetComponent<rtype::server::components::LobbyState>(playerId);
            if (!lobbyState || !lobbyState->isInGame) continue;
            
            auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(playerId);
            if (!pconn) continue;
            
            pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, false);
        }
    }
}

void ServerCollisionSystem::broadcastPlayerStateImmediate(ECS::World& world, ECS::EntityID playerId) {
    auto* pos = world.GetComponent<rtype::common::components::Position>(playerId);
    auto* health = world.GetComponent<rtype::common::components::Health>(playerId);
    auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(playerId);

    if (!pos || !health || !pconn) return;

    PlayerStatePacket pkt{};
    pkt.playerId = static_cast<uint32_t>(playerId);
    pkt.x = pos->x;
    pkt.y = pos->y;
    pkt.dir = pos->rotation;
    pkt.hp = static_cast<uint16_t>(health->currentHp);
    pkt.isAlive = health->isAlive;
    pkt.invulnerable = health->invulnerable;

    ECS::EntityID room = pconn->room_code;
    auto* allPlayers = world.GetAllComponents<rtype::common::components::Player>();
    if (!allPlayers) return;

    for (auto& [otherPid, playerPtr] : *allPlayers) {
        auto* otherConn = world.GetComponent<rtype::server::components::PlayerConn>(otherPid);
        if (!otherConn || otherConn->room_code != room) continue;

        otherConn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), PLAYER_STATE, nullptr, false);
    }
}

} // namespace rtype::server::systems
