/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "rtype.h"
#include "ECS/Types.hpp"
#include "systems.h"
#include "../../../common/components/Player.hpp"

void rtype::server::systems::player_packet_handler_system(ECS::EntityID e) {
    auto *player = root.world.GetComponent<common::components::Player>(e);
    if (!player)
        return;
    player->packet_handler.processPackets(player->packet_manager.fetchReceivedPackets());
}
