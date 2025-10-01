/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/RoomService.h"

#include "rtype.h"
#include "../../../common/components/Player.hpp"
#include "components/RoomProperties.h"

using namespace rtype::server::services;
using namespace rtype::common::components;

ECS::EntityID room_service::openNewRoom(bool is_public, ECS::EntityID player) {
    auto room = root.world.CreateEntity();
    int join_code = generateFreeJoinCode();

    root.world.AddComponent<components::RoomProperties>(room, join_code, is_public, player);

    // Set the player in the room
    if (auto *playerComp = root.world.GetComponent<common::components::Player>(player)) {
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
