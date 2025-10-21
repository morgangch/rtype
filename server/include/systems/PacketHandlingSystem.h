/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKETHANDLINGSYSTEM_H
#define PACKETHANDLINGSYSTEM_H
#include "rtype.h"
#include "tools.h"
#include "components/PlayerConn.h"
#include "ECS/System.h"
#include <iostream>

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
                continue; // Skip entities that aren't players (e.g., rooms, enemies)
            
            int processedPackets = player->packet_handler.processPackets(player->packet_manager.fetchReceivedPackets());
            if (processedPackets > 0) {
                player->last_packet_timestamp = rtype::tools::getCurrentTimestamp();
                std::cout << "[PacketHandlingSystem] Processed " << processedPackets << " packet(s) for player " << entity << std::endl;
            }
        }
    }
};

#endif //PACKETHANDLINGSYSTEM_H
