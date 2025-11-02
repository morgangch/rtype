/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef SENDERS_H
#define SENDERS_H
#include <cstdint>
#include <string>

namespace rtype::client::network::senders {
    /**
     * @brief Send a GameStartRequestPacket to the server to start the game.
     */
    void send_game_start_request();

    /**
     * @brief Send a join room request to the server
     * @param player_name The player name
     * @param room_code The room code to join
     */
    void send_join_room_request(const std::string &player_name, std::uint32_t room_code);
    
    /**
     * @brief Send a player ready state update to the server
     * @param isReady Whether the player is ready
     */
    void send_player_ready(bool isReady);
    
    /**
     * @brief Send a player shoot request to the server
     * @param isCharged Whether the shot is a charged shot
     * @param playerX Player's X position at time of shooting
     * @param playerY Player's Y position at time of shooting
     */
    void send_player_shoot(bool isCharged, float playerX, float playerY);
    
    /**
     * @brief Send player input state to the server
     * @param moveUp Whether the up key is pressed
     * @param moveDown Whether the down key is pressed
     * @param moveLeft Whether the left key is pressed
     * @param moveRight Whether the right key is pressed
     */
    void send_player_input(bool moveUp, bool moveDown, bool moveLeft, bool moveRight);
    
    /**
     * @brief Send a boss spawn request to the server (admin only)
     * Server will verify if the sender is an admin before spawning
     */
    void send_spawn_boss_request();

    /**
     * @brief Send a lobby settings update (admin-only) to the server.
     * @param difficultyIndex 0=Easy,1=Normal,2=Hard (cosmetic for now)
     * @param friendlyFire Cosmetic toggle
     * @param aiAssist Spawn AI assistant when exactly one player
     * @param megaDamage Admin projectile damage becomes 1000
     */
    void send_lobby_settings_update(uint8_t difficultyIndex, bool friendlyFire, bool aiAssist, bool megaDamage, uint8_t startLevel);
}

#endif //SENDERS_H
