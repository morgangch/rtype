/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PLAYERDISCONNECTSYSTEM_H
#define PLAYERDISCONNECTSYSTEM_H
#define PLAYER_TIMEOUT 10000 // 10 seconds
#include "packets.h"
#include "rtype.h"
#include "tools.h"
#include "components/PlayerConn.h"
#include "ECS/System.h"

namespace rtype::server::components {
    class PlayerConn;
}

class PlayerDisconnectSystem : public ECS::System {
public:
    PlayerDisconnectSystem() : ECS::System("PlayerDisconnectSystem", 10) {
    }

    void Update(ECS::World &world, float deltaTime) override {
        uint64_t currentTimestamp = rtype::tools::getCurrentTimestamp();

        for (auto pair: world.GetAllComponents<rtype::server::components::PlayerConn>()) {
            ECS::EntityID entity = pair.first;
            rtype::server::components::PlayerConn *player = pair.second;
            if (!player)
                return;
            if (currentTimestamp - player->last_packet_timestamp > PLAYER_TIMEOUT) {
                // Disconnect the player
                PlayerDisconnectPacket p{};
                p.playerId = entity;
                player->packet_manager.sendPacketBytesSafe(p, sizeof(PlayerDisconnectPacket), PLAYER_DISCONNECT,
                                                           nullptr, true);
                root.world.DestroyEntity(entity);
            }
        }
    }
};

#endif //PLAYERDISCONNECTSYSTEM_H
