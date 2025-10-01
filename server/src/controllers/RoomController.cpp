/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "controllers/RoomController.h"
#include "packets.h"
#include "rtype.h"
#include "ECS/Types.hpp"
#include "services/RoomService.h"

using namespace rtype::server::controllers;
using namespace rtype::server::services;


void room_controller::handleJoinRoomPacket(const packet_t &packet) {
    JoinRoomPacket *p = (JoinRoomPacket *) packet.data;

    ECS::EntityID room = 0;
    // TODO: create a player
    ECS::EntityID player = 0;

    if (p->joinCode == 0) {
        room = room_service::openNewRoom(false, player);
    } else if (p->joinCode == 1) {
        // Join a random public room
        room = room_service::findAvailablePublicRoom();
        if (room == 0) {
            // No available public room, create a new one
            room = room_service::openNewRoom(true, player);
        }
    } else {
        // Join a private room with the given join code
        room = room_service::getRoomByJoinCode(static_cast<int>(p->joinCode));
    }

    if (room == 0) {
        // Room not found.
        return;
    }
}
