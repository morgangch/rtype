/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packets.h"
#include "network/network.h"
#include "network/controllers/game_controller.h"

namespace rtype::client::network {
    void register_controllers() {
        ph.registerCallback(Packets::JOIN_ROOM_ACCEPTED,
                            rtype::client::controllers::game_controller::handle_join_room_accepted);
    }
}
