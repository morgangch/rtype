/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** AdminDetectorSystem - Monitors room admin status and reassigns when admin disconnects
*/
#ifndef ADMINDETECTORSYSTEM_H
#define ADMINDETECTORSYSTEM_H
#include "packets.h"
#include "rtype.h"
#include "components/PlayerConn.h"
#include "components/RoomProperties.h"
#include "ECS/System.h"
#include "ECS/World.h"

namespace rtype::server::components {
    class PlayerConn;
}

/**
 * @brief System that detects admin disconnection and promotes new admins
 * 
 * Monitors all rooms to ensure they always have an admin. When the current
 * admin disconnects, automatically promotes the next available player in
 * the room to admin status and broadcasts the change to all room members.
 * 
 * @note Priority: 10 (runs after most other systems)
 */
class AdminDetectorSystem : public ECS::System {
public:
    AdminDetectorSystem() : ECS::System("AdminDetectorSystem", 10) {
    }

    /**
     * @brief Update cycle - checks for admin disconnection and reassigns if needed
     * 
     * For each room:
     * 1. Checks if the current admin is still connected
     * 2. If admin disconnected, promotes the first available player
     * 3. Broadcasts ROOM_ADMIN_UPDATE to all players in the room
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (unused)
     */
    void Update(ECS::World &world, float deltaTime) override {
        for (auto &pair: *world.GetAllComponents<rtype::server::components::RoomProperties>()) {
            rtype::server::components::RoomProperties *room = pair.second.get();
            if (!room)
                continue;
            ECS::EntityID admin_id = room->ownerId;

            // Check if the admin player is still connected on that room
            bool admin_still_connected = false;
            for (auto &player_pair: *world.GetAllComponents<rtype::server::components::PlayerConn>()) {
                ECS::EntityID playerId = player_pair.first;
                rtype::server::components::PlayerConn *playerConn = player_pair.second.get();
                if (!playerConn)
                    continue;
                if (playerConn->room_code == room->joinCode && playerId == admin_id) {
                    admin_still_connected = true;
                    break;
                }
            }
            if (admin_still_connected)
                continue;
            // Admin has disconnected, assign a new admin
            for (auto &conn_pair: *world.GetAllComponents<rtype::server::components::PlayerConn>()) {
                rtype::server::components::PlayerConn *playerConn = conn_pair.second.get();
                if (!playerConn)
                    continue;
                if (playerConn->room_code != room->joinCode)
                    continue;
                // Assign new admin
                room->ownerId = conn_pair.first;
                RoomAdminUpdatePacket p{conn_pair.first};
                // Broadcast to all players in the room about the new admin
                room->broadcastPacket(&p, sizeof(p), ROOM_ADMIN_UPDATE, true);
                break;
            }
        }
    }
};


#endif //ADMINDETECTORSYSTEM_H
