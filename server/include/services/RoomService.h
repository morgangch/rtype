/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef ROOMSERVICE_H
#define ROOMSERVICE_H
#include "ECS/Types.hpp"

namespace rtype::server::services::room_service {
    /**
     * @brief Opens a new room and returns its EntityID
     *
     * @param is_public Specifies if the room is public or private
     * @return The EntityID of the newly created room
     */
    ECS::EntityID openNewRoom(bool is_public);

    /**
     * @brief Generates a unique join code for a room
     * @return  A unique join code for a room
     */
    int generateFreeJoinCode();

    /**
     * @brief Checks if a join code already exists
     * @param join_code The join code to check for existence
     * @return True if the join code already exists, false otherwise
     */
    bool isJoinCodeExists(int join_code);
}

#endif //ROOMSERVICE_H
