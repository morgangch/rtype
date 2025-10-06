/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/


#include "systems.h"
#include "rtype.h"

void rtype::server::systems::apply_systems(ECS::EntityID e) {
    player_packet_handler_system(e);
}
