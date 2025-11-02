/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef LINKEDROOM_H
#define LINKEDROOM_H
#include "ECS/Component.h"

namespace rtype::server::components {
    class LinkedRoom : public ECS::Component<LinkedRoom> {
    public:
        /**
         * @brief The ID of the room entity this player is linked to
         */
        ECS::EntityID room_id;


        /**
         * @brief Construct a new LinkedRoom component
         * @param room_id The ID of the room entity (default: 0)
         */
        LinkedRoom(ECS::EntityID room_id = 0)
            : room_id(room_id) {
        }
    };
}
#endif //LINKEDROOM_H
