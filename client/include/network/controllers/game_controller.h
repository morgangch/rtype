/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Network game controllers header - callback declarations
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
     * @brief Handle the RoomAdminUpdatePacket received from the server.
     * @param packet The received packet.
     */
    void handle_admin_update(const packet_t& packet);

    /**
     * @brief Handle the PlayerDisconnectPacket received from the server.
     * @param packet The received packet.
     */
    void handle_player_disconnect(const packet_t& packet);

    /**
     * @brief Handle SPAWN_ENEMY packet from server.
     */
    void handle_spawn_enemy(const packet_t& packet);

    /**
     * @brief Handle ENTITY_DESTROY packet from server.
     */
    void handle_entity_destroy(const packet_t& packet);

    /**
     * @brief Handle PLAYER_JOIN packet from server.
     */
    void handle_player_join(const packet_t& packet);

    /**
     * @brief Handle PLAYER_STATE packet from server.
     */
    void handle_player_state(const packet_t& packet);
    
    /**
     * @brief Handle ALL_PLAYERS_STATE packet from server (optimized batch update).
     */
    void handle_all_players_state(const packet_t& packet);
    
    /**
     * @brief Handle LOBBY_STATE packet from server.
     */
    void handle_lobby_state(const packet_t& packet);
    
    /**
     * @brief Handle GAME_START packet from server (transition all clients to game).
     */
    void handle_game_start(const packet_t& packet);
    
    /**
     * @brief Handle SPAWN_PROJECTILE packet from server.
     */
    void handle_spawn_projectile(const packet_t& packet);

    /**
     * @brief Handle PLAYER_SCORE_UPDATE packet from server.
     */
    void handle_player_score_update(const packet_t& packet);

}

#endif //GAME_CONTROLLER_H
