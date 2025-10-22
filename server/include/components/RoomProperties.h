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

#include "ECS/Component.h"

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

        /**
         * @brief Construct a new RoomProperties component
         * @param joinCode Numeric join code (default: 0)
         * @param isPublic Whether the room is publicly listed (default: true)
         * @param ownerId Owner entity ID (default: 0)
         */
        RoomProperties(int joinCode = 0, bool isPublic = true, int ownerId = 0)
            : joinCode(joinCode), isPublic(isPublic), ownerId(ownerId) {
        };
    };
}

#endif //ROOMPROPERTIESCOMPONENT_H
