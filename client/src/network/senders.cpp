/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#include "network/senders.h"

#include "packets.h"
#include "network/network.h"

namespace rtype::client::network::senders {
    void send_game_start_request() {
        GameStartRequestPacket packet;
        rtype::client::network::pm.sendPacketBytesSafe(&packet, sizeof(GameStartRequestPacket), GAME_START_REQUEST,
                                                       nullptr, true);
    }

    void send_join_room_request(const std::string &player_name, std::uint32_t room_code) {
        JoinRoomPacket p{};
        p.joinCode = room_code;
        // Secure the player name to avoid overflow
        strncpy(p.name, player_name.c_str(), 31);
        p.name[31] = '\0';
        rtype::client::network::pm.sendPacketBytesSafe(&p, sizeof(JoinRoomPacket), JOIN_ROOM, nullptr, true);
    }
}
