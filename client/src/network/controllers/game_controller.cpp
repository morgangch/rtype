/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "network/controllers/game_controller.h"
#include <iostream>
#include "packet.h"
#include "packets.h"
#include "gui/GameState.h"

namespace rtype::client::controllers::game_controller {

void handle_join_room_accepted(const packet_t &packet) {
    JoinRoomAcceptedPacket *p = (JoinRoomAcceptedPacket *) packet.data;

    std::cout << "Successfully connected on room " << p->roomCode << " as " << (p->admin ? "admin" : "classic player") << std::endl;
    // TODO: Start the game here.
}

}
