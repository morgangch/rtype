/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/PlayerService.h"
#include "rtype.h"
//TODO: optimize the following import.
#include "../../../common/components/Player.hpp"

using namespace rtype::server::services;

ECS::EntityID player_service::createNewPlayer(std::string name, int room_code) {
    auto player = root.world.CreateEntity();

    root.world.AddComponent<rtype::common::components::Player>(player, name, room_code);
    return player;
}
