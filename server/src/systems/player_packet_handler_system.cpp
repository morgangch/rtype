/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "rtype.h"
#include "ECS/Types.hpp"
#include "systems.h"
#include "components/PlayerConn.h"

namespace rtype::server::components {
    class PlayerConn;
}

void rtype::server::systems::player_packet_handler_system(ECS::EntityID e) {
    auto *player = root.world.GetComponent<components::PlayerConn>(e);
    if (!player)
        return;
    player->packet_handler.processPackets(player->packet_manager.fetchReceivedPackets());
}
