/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "controllers/RoomController.h"
#include "packets.h"
#include "rtype.h"
#include "components/RoomProperties.h"
#include "ECS/Types.hpp"
#include "services/PlayerService.h"
#include "services/RoomService.h"

using namespace rtype::server::controllers;
using namespace rtype::server::services;


void room_controller::handleGameStartRequest(const packet_t &packet) {
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    ECS::EntityID room;

    if (!player)
        return; // The user is not found
    room = room_service::getRoomByPlayer(player);
    if (!room)
        return; // The player is not in a room (strange case)
    // Check if the player is the room owner
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (rp->ownerId != player)
        return; // The player is not the room owner (cheater)
    // TODO: Start the game
}

void room_controller::handleJoinRoomPacket(const packet_t &packet) {
    JoinRoomPacket *p = (JoinRoomPacket *) packet.data;

    ECS::EntityID room = 0;
    ECS::EntityID player = player_service::createNewPlayer(std::string(p->name));

    if (p->joinCode == 0) {
        // Create a new private room
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

    // Allow the client to join the room
    JoinRoomAcceptedPacket a{};
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    a.admin = (rp->ownerId == player);
    a.roomCode = rp->joinCode;
    root.packetManager.sendPacketBytesSafe(&a, sizeof(a), JOIN_ROOM_ACCEPTED, nullptr, true);
}
