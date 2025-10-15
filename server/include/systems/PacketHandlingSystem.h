/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKETHANDLINGSYSTEM_H
#define PACKETHANDLINGSYSTEM_H
#include "rtype.h"
#include "components/PlayerConn.h"
#include "ECS/System.h"

namespace rtype::server::components {
    class PlayerConn;
}

class PacketHandlingSystem : public ECS::System {
public:
    PacketHandlingSystem() : ECS::System("PacketHandlingSystem", 10) {
    }

    void Update(ECS::World &world, float deltaTime) override {
        for (ECS::EntityID entity: world.GetAllEntities()) {
            auto *player = root.world.GetComponent<rtype::server::components::PlayerConn>(entity);
            if (!player)
                return;
            player->packet_handler.processPackets(player->packet_manager.fetchReceivedPackets());
        }
    }
};

#endif //PACKETHANDLINGSYSTEM_H
