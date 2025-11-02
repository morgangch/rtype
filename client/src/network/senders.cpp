/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#include "network/senders.h"
#include "packets.h"
#include "network/network.h"
#include <iostream>
#include "utils/endiane_converter.h"

namespace rtype::client::network::senders {
    void send_game_start_request() {
        std::cout << "CLIENT: Sending GAME_START_REQUEST packet" << std::endl;
        GameStartRequestPacket packet;
        pm.sendPacketBytesSafe(&packet, sizeof(GameStartRequestPacket), GAME_START_REQUEST,
                                                       nullptr, true);
    }

    void send_join_room_request(const std::string &player_name, std::uint32_t room_code, uint8_t vessel_type) {
        JoinRoomPacket p{};
        p.joinCode =  room_code;
        p.vesselType = vessel_type;

        // Convert to network endian
        to_network_endian(p.joinCode);

        // Secure the player name to avoid overflow
        strncpy(p.name, player_name.c_str(), 31);
        p.name[31] = '\0';
        pm.sendPacketBytesSafe(&p, sizeof(JoinRoomPacket), JOIN_ROOM, nullptr, true);
    }
    
    void send_player_ready(bool isReady) {
        std::cout << "CLIENT: Sending PLAYER_READY packet (isReady=" << isReady << ")" << std::endl;
        PlayerReadyPacket p{};
        p.isReady = isReady;
        pm.sendPacketBytesSafe(&p, sizeof(PlayerReadyPacket), PLAYER_READY, nullptr, true);
    }
    
    void send_player_shoot(bool isCharged, float playerX, float playerY) {
        PlayerShootPacket p{};
        p.isCharged = isCharged;
        p.playerX = playerX;
        p.playerY = playerY;

        // Convert to network endian
        to_network_endian(p.playerX);
        to_network_endian(p.playerY);

        pm.sendPacketBytesSafe(&p, sizeof(PlayerShootPacket), PLAYER_SHOOT, nullptr, true);
        std::cout << "CLIENT: Sent PLAYER_SHOOT (charged: " << isCharged << " pos: " << playerX << "," << playerY << ")" << std::endl;
    }
    
    void send_player_input(bool moveUp, bool moveDown, bool moveLeft, bool moveRight) {
        PlayerInputPacket p{};
        p.moveUp = moveUp;
        p.moveDown = moveDown;
        p.moveLeft = moveLeft;
        p.moveRight = moveRight;
        pm.sendPacketBytesSafe(&p, sizeof(PlayerInputPacket), PLAYER_INPUT, nullptr, false);
    }
    
    void send_spawn_boss_request() {
        std::cout << "CLIENT: Sending SPAWN_BOSS_REQUEST packet (admin only)" << std::endl;
        SpawnBossRequestPacket p{};
        pm.sendPacketBytesSafe(&p, sizeof(SpawnBossRequestPacket), SPAWN_BOSS_REQUEST, nullptr, true);
    }

    void send_lobby_settings_update(uint8_t difficultyIndex, bool friendlyFire, bool aiAssist, bool megaDamage, uint8_t startLevel) {
        LobbySettingsUpdatePacket p{};
        p.difficulty = difficultyIndex;
        p.friendlyFire = friendlyFire;
        p.aiAssist = aiAssist;
        p.megaDamage = megaDamage;
        p.startLevel = startLevel;

        std::cout << "CLIENT: Sending LOBBY_SETTINGS_UPDATE packet: diff="
                  << static_cast<int>(p.difficulty)
                  << ", FF=" << (p.friendlyFire ? "on" : "off")
                  << ", AI=" << (p.aiAssist ? "on" : "off")
                  << ", MEGA=" << (p.megaDamage ? "on" : "off")
                  << ", START_LVL=L" << static_cast<int>(p.startLevel + 1)
                  << std::endl;

        pm.sendPacketBytesSafe(&p, sizeof(LobbySettingsUpdatePacket), LOBBY_SETTINGS_UPDATE, nullptr, true);
    }
}
