/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H
#include "packet.h"

namespace rtype::client::controllers::game_controller {
    /**
     * @brief Handle the JoinRoomAcceptedPacket received from the server.
     * @param packet The received packet.
     */
    void handle_join_room_accepted(const packet_t& packet);

    /**
     * @brief Handle the PlayerDisconnectPacket received from the server.
     * @param packet The received packet.
     */
    void handle_player_disconnect(const packet_t& packet);

}


#endif //GAME_CONTROLLER_H
