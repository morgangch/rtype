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
    };
}

#endif //ROOMPROPERTIESCOMPONENT_H
