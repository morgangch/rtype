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
}

#endif //SENDERS_H
