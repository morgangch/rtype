/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef SENDERS_H
#define SENDERS_H
#include <cstdint>

#include "components/RoomProperties.h"

namespace rtype::server::network::senders {
    void broadcast_entity_destroy(ECS::EntityID room_id, uint32_t entity_id, uint16_t reason);
}

#endif //SENDERS_H
