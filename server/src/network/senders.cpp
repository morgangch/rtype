/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "senders.h"

#include "packets.h"
#include "components/RoomProperties.h"
#include "services/RoomService.h"
#include "rtype.h"
#include <iostream>

#include "common/utils/endiane_converter.h"

namespace rtype::server::network::senders {
    void broadcast_entity_destroy(ECS::EntityID room_id, uint32_t entity_id, uint16_t reason) {
        EntityDestroyPacket pkt{};
        pkt.entityId = entity_id;
        pkt.reason = reason;

        to_network_endian(pkt.entityId);
        to_network_endian(pkt.reason);

        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast EntityDestroyPacket, room " << room_id << " not found" << std::endl;
            return;
        }
        room->broadcastPacket(&pkt, sizeof(pkt), ENTITY_DESTROY, true);
    }
}
