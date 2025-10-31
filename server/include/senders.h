/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef SENDERS_H
#define SENDERS_H
#include <cstdint>

#include "common/components/EnemyType.h"
#include "components/RoomProperties.h"

namespace rtype::server::network::senders {
    /**
     * Broadcasts an EntityDestroyPacket to all players in the specified room
     * @param room_id  The room to broadcast to
     * @param entity_id  The entity being destroyed
     * @param reason The reason code for destruction
     */
    void broadcast_entity_destroy(ECS::EntityID room_id, uint32_t entity_id, uint16_t reason);


    /**
     * Sends a PlayerStatePacket to a specific player about another player's state
     * @param room_id  The room to broadcast to
     * @param playerId  The player entity ID
     * @param x  The X position
     * @param y The Y position
     * @param dir  The direction/rotation
     * @param hp  The hit points
     * @param isAlive  Whether the player is alive
     */
    void send_player_state(ECS::EntityID to_player, ECS::EntityID playerId, float x, float y, float dir, uint16_t hp,
                                bool isAlive);

    /**
     * Broadcasts the game start packet to all players in the specified room
     *  @param room_id The room entity ID
     */
    void broadcast_game_start(ECS::EntityID room_id);

    /**
     * Sends a PlayerJoinPacket to the specified player about a new player joining
     * @param player The player entity ID to notify
     * @param new_player The new player entity ID who joined
     * @param new_player_name The name of the new player
     *
     */
    void send_player_join(ECS::EntityID player, ECS::EntityID new_player, const std::string &new_player_name);

    /**
     * Sends a JoinRoomAcceptedPacket to the specified player
     *
     * @param player The player entity ID
     * @param isAdmin Whether the player is the room admin
     * @param roomCode The room join code
     * @param playerServerId The server-assigned player ID
     */
    void send_join_room_accepted(ECS::EntityID player, bool isAdmin, uint32_t roomCode, uint32_t playerServerId);


    /**
     * Broadcasts a projectile spawn packet to all players in the specified room
     * @param room_id  The room to broadcast to
     * @param projectileId The projectile entity ID
     * @param ownerId The owner (shooter) entity ID
     * @param x The X position
     * @param y The Y position
     * @param vx The X velocity
     * @param vy The Y velocity
     * @param damage The damage value
     * @param piercing Whether the projectile is piercing
     * @param isCharged Whether the projectile is a charged shot
     */
    void broadcast_projectile_spawn(ECS::EntityID room_id, uint32_t projectileId, uint32_t ownerId,
                                    float x, float y, float vx, float vy, uint16_t damage, bool piercing,
                                    bool isCharged);


    /**
     * Sends the lobby state to the specified player
     * @param player  The player entity ID to send the lobby state to
     * @param totalPlayers Total number of players in the lobby
     * @param readyPlayers Number of players marked as ready
     */
    void send_lobby_state(ECS::EntityID player, uint32_t totalPlayers, uint32_t readyPlayers);

    /**
     * Broadcasts an enemy spawn packet to all players in the specified room
     * @param room_id  The room entity ID to broadcast to
     * @param enemyId The enemy entity ID
     * @param enemyType The type of enemy
     * @param x The X position
     * @param y The Y position
     * @param hp The enemy's hit points
     */
    void broadcast_enemy_spawn(ECS::EntityID room_id, uint32_t enemyId, common::components::EnemyType enemyType, float x, float y,
                               uint16_t hp);


    /**
     *  Broadcasts a player disconnect packet to all players in the specified room
     * @param room_id  The room entity ID to broadcast to
     * @param playerId  The player entity ID that disconnected
     */
    void broadcast_player_disconnect(ECS::EntityID room_id, uint32_t playerId);

    /**
     * @brief Send server-authoritative in-game score to a specific player
     * @param player The recipient player entity ID (for connection lookup)
     * @param playerId The server entity ID of the player (echoed for client-side mapping)
     * @param score New absolute score value
     */
    void send_player_score(ECS::EntityID player, uint32_t playerId, int32_t score);
}

#endif //SENDERS_H
