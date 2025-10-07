/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/RoomService.h"

#include "packets.h"
#include "rtype.h"
#include "../../../common/components/Player.hpp"
#include "components/PlayerConn.h"
#include "components/RoomProperties.h"

using namespace rtype::server::services;
using namespace rtype::common::components;

ECS::EntityID room_service::openNewRoom(bool is_public, ECS::EntityID player) {
    auto room = root.world.CreateEntity();
    int join_code = generateFreeJoinCode();

    root.world.AddComponent<components::RoomProperties>(room, join_code, is_public, player);

    // Set the player in the room
    if (auto *playerComp = root.world.GetComponent<server::components::PlayerConn>(player)) {
        playerComp->room_code = room;
    }
    return room;
}

int room_service::generateFreeJoinCode() {
    int code = 0;

    while (isJoinCodeExists(code) || code == 0) {
        code = rand() % 9000 + 1000;
    }
    return code;
}

bool room_service::isJoinCodeExists(int join_code) {
    auto *rooms = root.world.GetAllComponents<components::RoomProperties>();

    for (const auto &pair: *rooms) {
        if (pair.second->joinCode == join_code) {
            return true;
        }
    }
    return false;
}

ECS::EntityID room_service::getRoomByJoinCode(int join_code) {
    auto *rooms = root.world.GetAllComponents<components::RoomProperties>();

    for (const auto &pair: *rooms) {
        if (pair.second->joinCode == join_code) {
            return pair.first;
        }
    }
    return 0; // Return 0 if room not found
}

ECS::EntityID room_service::findAvailablePublicRoom() {
    auto *rooms = root.world.GetAllComponents<components::RoomProperties>();

    for (const auto &pair: *rooms) {
        if (pair.second->isPublic && !pair.second->isGameStarted) {
            return pair.first;
        }
    }
    return 0; // Return 0 if no available public room found
}

ECS::EntityID room_service::getRoomByPlayer(ECS::EntityID player) {
    auto *playerComp = root.world.GetComponent<server::components::PlayerConn>(player);

    if (playerComp) {
        return playerComp->room_code;
    }
    return 0; // Return 0 if player not found or not in a room
}

void room_service::kickPlayer(ECS::EntityID player) {
    auto *playerComp = root.world.GetComponent<components::PlayerConn>(player);

    if (playerComp) {
        ECS::EntityID room = playerComp->room_code;
        PlayerDisconnectPacket p;
        p.playerId = player;
        root.packetManager.sendPacketBytesSafe(&p, sizeof(PlayerDisconnectPacket), Packets::PLAYER_DISCONNECT, nullptr,
                                               true);
        root.world.DestroyEntity(player);
    }
}

void room_service::closeRoom(ECS::EntityID room) {
    auto *rp = root.world.GetComponent<components::RoomProperties>(room);

    if (rp) {
        // Remove all players from the room
        auto *players = root.world.GetAllComponents<common::components::Player>();
        for (const auto &pair: *players) {
            kickPlayer(pair.first);
        }
        // Finally, destroy the room entity
        root.world.DestroyEntity(room);
    }
}

