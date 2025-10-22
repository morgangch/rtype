/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef SENDERS_H
#define SENDERS_H
#include <cstdint>
#include <string>

namespace rtype::client::network::senders {
    /**
     * @brief Send a GameStartRequestPacket to the server to start the game.
     */
    void send_game_start_request();

    /**
     * @brief Send a join room request to the server
     * @param player_name The player name
     * @param room_code The room code to join
     */
    void send_join_room_request(const std::string &player_name, std::uint32_t room_code);
    
    /**
     * @brief Send a player ready state update to the server
     * @param isReady Whether the player is ready
     */
    void send_player_ready(bool isReady);
    
    /**
     * @brief Send a player shoot request to the server
     * @param isCharged Whether the shot is a charged shot
     * @param playerX Player's X position at time of shooting
     * @param playerY Player's Y position at time of shooting
     */
    void send_player_shoot(bool isCharged, float playerX, float playerY);
    
    /**
     * @brief Send a boss spawn request to the server (admin only)
     * Server will verify if the sender is an admin before spawning
     */
    void send_spawn_boss_request();
}

#endif //SENDERS_H
