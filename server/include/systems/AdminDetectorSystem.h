/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
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

class AdminDetectorSystem : public ECS::System {
public:
    AdminDetectorSystem() : ECS::System("AdminDetectorSystem", 10) {
    }

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
                // Notify all players in the room about the new admin
                room->broadcastPacket(&p, sizeof(p), ROOM_ADMIN_UPDATE, true);
                break;
            }
        }
    }
};


#endif //ADMINDETECTORSYSTEM_H
