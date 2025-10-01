/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PLAYERSERVICE_H
#define PLAYERSERVICE_H
#include <string>

#include "ECS/Types.hpp"

namespace rtype::server::services::player_service {
    /**
     * @brief Creates a new player entity and returns its EntityID
     *
     * @param name The name of the player
     * @param room_code The room code the player is joining (default is 0)
     * @return The EntityID of the newly created player
     */
    ECS::EntityID createNewPlayer(std::string name, int room_code = 0);
}

#endif //PLAYERSERVICE_H
