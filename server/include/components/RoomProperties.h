/**
 * @file RoomProperties.h
 * @brief Component storing room metadata on the server
 *
 * This component is attached to a room-entity and contains properties used by
 * the server to manage matchmaking, visibility and game lifecycle for the room.
 *
 * Example uses:
 * - When broadcasting room lists to clients
 * - When deciding which players receive in-game state updates
 * - Determining who is allowed to perform admin actions in the room
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

#ifndef ROOMPROPERTIESCOMPONENT_H
#define ROOMPROPERTIESCOMPONENT_H

#include <iostream>
#include "ECS/Component.h"
#include "rtype.h"
#include "components/PlayerConn.h"
#include "services/PlayerService.h"
#include "common/components/Health.h"

namespace rtype::server::components {
    /**
     * @brief Server-side room metadata component
     *
     * Members:
     * - joinCode: numeric join code for the room
     * - isPublic: whether the room appears in the public lobby
     * - isGameStarted: true once the room has transitioned into active game
     * - ownerId: EntityID of the player who created/owns the room (admin)
     */
    class RoomProperties : public ECS::Component<RoomProperties> {
    public:
        /**
         * @brief Construct a new RoomProperties component
         * @param joinCode Numeric join code (default: 0)
         * @param isPublic Whether the room is publicly listed (default: true)
         * @param ownerId Owner entity ID (default: 0)
         */
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
            auto players = services::player_service::findPlayersByRoomCode(joinCode);

            if (players.size() == 0) {
                std::cout << "Room " << joinCode << " has no players to broadcast to." << std::endl;
                return;
            }
            for (auto player: players) {
                // Skip dead players - their network connection may be invalid
                auto *health = root.world.GetComponent<rtype::common::components::Health>(player);
                if (health && (!health->isAlive || health->currentHp <= 0)) {
                    continue;
                }
                
                auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
                if (!pconn) {
                    continue;
                }
                pconn->packet_manager.sendPacketBytesSafe(
                    data, size, packetType, nullptr, important);
            }
        }

        /**
         * @brief Numeric room code used for joining private rooms
         */
        int joinCode;

        /**
         * @brief Public visibility flag for the room
         */
        bool isPublic;

        /**
         * @brief True when the match in this room has started
         */
        bool isGameStarted = false;

        /**
         * @brief Entity ID of the room owner (first player to create it)
         */
        ECS::EntityID ownerId = 0; // EntityID of the room owner (first player who created the room)

        // Lobby settings (admin-controlled)
        uint8_t difficultyIndex = 1;  // 0=Easy,1=Normal,2=Hard (cosmetic)
        bool friendlyFire = false;    // cosmetic for now
        bool aiAssistEnabled = false; // spawn AI assistant if exactly one human
        bool megaDamageEnabled = false; // admin projectile dmg=1000
        uint8_t startLevelIndex = 0; // debug start level: 0=Lvl1, 1=Lvl2
    };
}

#endif //ROOMPROPERTIESCOMPONENT_H
