/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/RoomService.h"

#include "packets.h"
#include "rtype.h"
#include <common/components/Player.h>

#include "senders.h"
#include "components/LinkedRoom.h"
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
    auto *room = root.world.GetComponent<server::components::LinkedRoom>(player);

    if (room) {
        return room->room_id;
    }
    return 0; // Return 0 if player not found or not in a room
}

void room_service::kickPlayer(ECS::EntityID player) {
    auto room = getRoomByPlayer(player);

    if (room)
        network::senders::broadcast_player_disconnect(room, static_cast<uint32_t>(player));
    root.world.DestroyEntity(player);
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
