/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packets.h"
#include "network/network.h"
#include "network/controllers/game_controller.h"

using namespace rtype::client::controllers::game_controller;

namespace rtype::client::network {
    void register_controllers() {
        ph.registerCallback(Packets::JOIN_ROOM_ACCEPTED, handle_join_room_accepted);
        ph.registerCallback(Packets::PLAYER_DISCONNECT, handle_player_disconnect);
    }
}
