/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "ECS/Types.h"

namespace rtype::server::systems {

    /**
     * @brief Apply all systems to the given entity.
     * @param e The entity ID.
     */
    void apply_systems(ECS::EntityID e);

    /**
     * @brief System to transmit packets from the PacketManager to the PacketHandler for each player entity.
     * @param e The entity ID.
     */
    void player_packet_handler_system(ECS::EntityID e);
}

#endif //SYSTEMS_H
