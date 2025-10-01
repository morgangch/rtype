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
#include "components/NetworkAddress.h"

using namespace rtype::server::services;

ECS::EntityID player_service::createNewPlayer(std::string name, int room_code, std::string ip, int port) {
    auto player = root.world.CreateEntity();

    root.world.AddComponent<rtype::common::components::Player>(player, name, room_code);
    root.world.AddComponent<rtype::server::components::NetworkAddress>(player, ip, port);
    return player;
}

ECS::EntityID player_service::findPlayerByNetwork(const std::string &ip, int port) {
    auto players = root.world.GetAllComponents<rtype::server::components::NetworkAddress>();

    for (const auto &pair: *players) {
        auto *addr = root.world.GetComponent<rtype::server::components::NetworkAddress>(pair.first);
        if (addr && addr->address == ip && addr->port == port) {
            return pair.first;
        }
    }
    return 0;
}

ECS::EntityID player_service::findPlayerByNetwork(const uint8_t *uint8_t, uint16_t port) {
    std::string ip = std::to_string(uint8_t[0]) + "." + std::to_string(uint8_t[1]) + "." +
                     std::to_string(uint8_t[2]) + "." + std::to_string(uint8_t[3]);
    return findPlayerByNetwork(ip, port);
}
