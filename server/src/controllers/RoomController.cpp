/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "controllers/RoomController.h"
#include "packets.h"
#include "ECS/Types.hpp"
#include "services/RoomService.h"

void rtype::server::controllers::room_controller::handleJoinRoomPacket(const packet_t &packet) {
    JoinRoomPacket *p = (JoinRoomPacket *) packet.data;

    ECS::EntityID room = 0;

    if (p->joinCode == 0) {
        room = rtype::server::services::room_service::openNewRoom(false);
    } else if (p->joinCode == 1) {
        // Join a random public room
        // TODO: implement
    } else {
        // Join a private room with the given join code
        room = rtype::server::services::room_service::getRoomByJoinCode(static_cast<int>(p->joinCode));
    }

    if (room == 0) {
        // Room not found.
        return;
    }

    // TODO: Create a player instance, link it to the room and make it owner if the join_code is 0.
}
