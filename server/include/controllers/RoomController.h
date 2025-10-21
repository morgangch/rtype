/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef ROOMCONTROLLER_H
#define ROOMCONTROLLER_H
#include "packet.h"
#include "packethandler.h"
#include "ECS/Types.h"

namespace rtype::server::controllers::room_controller {
    void handleJoinRoomPacket(const packet_t& packet);
    void handleGameStartRequest(const packet_t& packet);
    void handlePlayerInput(const packet_t& packet);
    void handlePlayerReady(const packet_t& packet);
    void handlePlayerShoot(const packet_t& packet);
    void broadcastLobbyState(ECS::EntityID room);
    void registerPlayerCallbacks(PacketHandler& handler);
}

#endif //ROOMCONTROLLER_H
