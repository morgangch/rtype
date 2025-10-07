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
     * @param player The Entity of the room owner
     * @return The EntityID of the newly created room
     */
    ECS::EntityID openNewRoom(bool is_public, ECS::EntityID player);

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

    /**
     *  @brief Retrieves a room EntityID by its join code
     *  @param join_code The join code of the room to retrieve
     *  @return The EntityID of the room with the specified join code, or 0 if not found
     */
    ECS::EntityID getRoomByJoinCode(int join_code);

    /**
     * @brief Find a public room isn't started yet
     * @return The EntityID of the found room, or 0 if none found
    */
    ECS::EntityID findAvailablePublicRoom();

    /**
     * @brief Retrieves the room EntityID that a player is currently in
     * @param player The EntityID of the player
     * @return The EntityID of the room the player is in, or 0 if not found
     */
    ECS::EntityID getRoomByPlayer(ECS::EntityID player);

    /**
     * @brief Kick a player from a room
     * @param player The EntityID of the player to kick
     */
    void kickPlayer(ECS::EntityID player);

    /**
     * @brief Closes a room and removes all its associated data
     * @param room The EntityID of the room to close
     */
    void closeRoom(ECS::EntityID room);
}

#endif //ROOMSERVICE_H
