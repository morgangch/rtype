/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** PlayerDisconnectSystem - Detects and handles player timeouts and disconnections
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

/**
 * @brief System that detects player timeouts and handles disconnections
 * 
 * Monitors all connected players and checks if they've sent packets recently.
 * If a player hasn't sent any packets for PLAYER_TIMEOUT milliseconds,
 * broadcasts a disconnection packet and removes them from the game.
 * 
 * @note Priority: 10 (runs after packet handling)
 * @note Timeout threshold: 10 seconds (PLAYER_TIMEOUT)
 */
class PlayerDisconnectSystem : public ECS::System {
public:
    PlayerDisconnectSystem() : ECS::System("PlayerDisconnectSystem", 10) {
    }

    /**
     * @brief Update cycle - checks for player timeouts
     * 
     * For each player:
     * 1. Checks time since last packet received
     * 2. If timeout exceeded, sends PLAYER_DISCONNECT packet
     * 3. Destroys the player entity
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (unused)
     */
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
