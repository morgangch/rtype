/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#ifndef ROOMPROPERTIESCOMPONENT_H
#define ROOMPROPERTIESCOMPONENT_H

#include "ECS/Component.hpp"

namespace rtype::server::components {
    class RoomProperties : public ECS::Component<RoomProperties> {
    public:
        int joinCode;
        bool isPublic;
        RoomProperties(int joinCode = 0, bool isPublic = true)
            : joinCode(joinCode), isPublic(isPublic) {
        };
    };
}

#endif //ROOMPROPERTIESCOMPONENT_H
