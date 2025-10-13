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

    // Launch the game using the global StateManager if available (same logic as in the lobby)
    using rtype::client::gui::g_stateManager;

    if (g_stateManager) {
        g_stateManager->changeState(std::make_unique<rtype::client::gui::GameState>(*g_stateManager));
    } else {
        std::cerr << "StateManager global not set; cannot start GameState from network controller." << std::endl;
    }
}

}
