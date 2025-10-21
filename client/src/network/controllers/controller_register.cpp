/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packets.h"
#include "network/network.h"
#include "network/controllers/game_controller.h"

using namespace rtype::client::controllers::game_controller;

namespace rtype::client::network {
    void register_controllers() {
        ph.registerCallback(Packets::JOIN_ROOM_ACCEPTED, handle_join_room_accepted);
        ph.registerCallback(Packets::PLAYER_DISCONNECT, handle_player_disconnect);
        ph.registerCallback(Packets::SPAWN_ENEMY, handle_spawn_enemy);
        ph.registerCallback(Packets::ENTITY_DESTROY, handle_entity_destroy);
        ph.registerCallback(Packets::PLAYER_JOIN, handle_player_join);
        ph.registerCallback(Packets::PLAYER_STATE, handle_player_state);
        ph.registerCallback(Packets::LOBBY_STATE, handle_lobby_state);
        ph.registerCallback(Packets::GAME_START, handle_game_start);
        ph.registerCallback(Packets::SPAWN_PROJECTILE, handle_spawn_projectile);
    }
}
