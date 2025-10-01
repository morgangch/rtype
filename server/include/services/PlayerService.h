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
     * @param ip The IP address of the player (default is empty string)
     * @param port The port number of the player (default is 0)
     * @return The EntityID of the newly created player
     */
    ECS::EntityID createNewPlayer(std::string name, int room_code = 0, std::string ip = "", int port = 0);

    /**
     * @brief Find a player entity from its network properties
     * @param ip The IP address of the player
     * @param port The port number of the player
     * @return The EntityID of the player, or 0 if not found
     */
    ECS::EntityID findPlayerByNetwork(const std::string &ip, int port);
    ECS::EntityID findPlayerByNetwork(const uint8_t * ip, uint16_t port);
}

#endif //PLAYERSERVICE_H
