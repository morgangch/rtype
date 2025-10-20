/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#ifndef ROOMPROPERTIESCOMPONENT_H
#define ROOMPROPERTIESCOMPONENT_H

#include "ECS/Component.h"

namespace rtype::server::components {
    class RoomProperties : public ECS::Component<RoomProperties> {
    public:
        int joinCode;
        bool isPublic;
        bool isGameStarted = false;
        ECS::EntityID ownerId = 0; // EntityID of the room owner (first player who created the room)
        RoomProperties(int joinCode = 0, bool isPublic = true, int ownerId = 0)
            : joinCode(joinCode), isPublic(isPublic), ownerId(ownerId) {
        };

        /**
         * Broadcast a packet to all players in the room
         * @param data the packet data
         * @param size the size of the packet
         * @param packetType the type of the packet
         * @param important whether the packet is important (reliable)
         */
        void broadcastPacket(void *data, size_t size, uint8_t packetType, bool important) const {
            for (auto &pair: *root.world.GetAllComponents<PlayerConn>()) {
                rtype::server::components::PlayerConn *notifyConn = pair.second.get();
                if (!notifyConn)
                    continue;
                if (notifyConn->room_code == joinCode) {
                    notifyConn->packet_manager.sendPacketBytesSafe(
                        &data, sizeof(data), packetType, nullptr, important);
                }
            }
        }
    };
}

#endif //ROOMPROPERTIESCOMPONENT_H
