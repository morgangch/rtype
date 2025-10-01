/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/RoomService.h"

#include "rtype.h"
#include "components/RoomProperties.h"
using namespace rtype::server::services;

ECS::EntityID room_service::openNewRoom(bool is_public) {
    auto room = root.world.CreateEntity();
    int join_code = generateFreeJoinCode();

    root.world.AddComponent<components::RoomProperties>(room, join_code, is_public);
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
