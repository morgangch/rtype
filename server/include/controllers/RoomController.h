/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef ROOMCONTROLLER_H
#define ROOMCONTROLLER_H
#include "packet.h"

namespace rtype::server::controllers::room_controller {
    void handleJoinRoomPacket(const packet_t& packet);
    void handleGameStartRequest(const packet_t& packet);
}

#endif //ROOMCONTROLLER_H
