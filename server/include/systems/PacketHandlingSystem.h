/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** PacketHandlingSystem - Processes incoming packets from all connected players
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

/**
 * @brief System that processes incoming network packets for all players
 * 
 * Iterates through all player entities, fetches their received packets,
 * and processes them through their packet handlers. Updates the last
 * packet timestamp for timeout detection.
 * 
 * @note Priority: 10 (runs after most gameplay systems)
 */
class PacketHandlingSystem : public ECS::System {
public:
    PacketHandlingSystem() : ECS::System("PacketHandlingSystem", 10) {
    }

    /**
     * @brief Update cycle - processes packets for all connected players
     * 
     * For each player entity:
     * 1. Fetches received packets from their PacketManager
     * 2. Processes packets through their PacketHandler
     * 3. Updates last_packet_timestamp for disconnect detection
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (unused)
     */
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
