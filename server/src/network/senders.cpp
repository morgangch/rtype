/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "senders.h"

#include "packets.h"
#include "components/RoomProperties.h"
#include "services/RoomService.h"
#include "rtype.h"
#include <iostream>

#include "common/utils/endiane_converter.h"
#include "services/PlayerService.h"

namespace rtype::server::network::senders {
    void broadcast_entity_destroy(ECS::EntityID room_id, uint32_t entity_id, uint16_t reason) {
        EntityDestroyPacket pkt{};
        pkt.entityId = entity_id;
        pkt.reason = reason;

        to_network_endian(pkt.entityId);
        to_network_endian(pkt.reason);

        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast EntityDestroyPacket, room " << room_id << " not found" << std::endl;
            return;
        }
        room->broadcastPacket(&pkt, sizeof(pkt), ENTITY_DESTROY, true);
    }

    void send_join_room_accepted(ECS::EntityID player, bool isAdmin, uint32_t roomCode, uint32_t playerServerId) {
        JoinRoomAcceptedPacket pkt{};
        pkt.admin = isAdmin;
        pkt.roomCode = roomCode;
        pkt.playerServerId = playerServerId;

        to_network_endian(pkt.roomCode);
        to_network_endian(pkt.playerServerId);

        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
        if (!pconn) {
            std::cerr << "ERROR: Cannot send JoinRoomAcceptedPacket, player " << player << " has no PlayerConn" <<
                    std::endl;
            return;
        }
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), JOIN_ROOM_ACCEPTED, nullptr, true);
    }

    void broadcast_game_start(ECS::EntityID room_id) {
        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast GameStartPacket, room " << room_id << " not found" << std::endl;
            return;
        }

        GameStartPacket pkt{}; // Include start level so clients can sync visuals immediately
        pkt.startLevel = room->startLevelIndex; // 0=Lvl1,1=Lvl2

        std::cout << "Broadcasting GAME_START to room " << room_id << std::endl;
        room->broadcastPacket(&pkt, sizeof(pkt), GAME_START, true);
    }

    void send_player_join(ECS::EntityID player, ECS::EntityID new_player, const std::string &new_player_name) {
        PlayerJoinPacket joinPkt{};
        joinPkt.newPlayerId = new_player;
        strncpy(joinPkt.name, new_player_name.c_str(), sizeof(joinPkt.name) - 1);
        joinPkt.name[sizeof(joinPkt.name) - 1] = '\0';

        // Convert to network endian
        to_network_endian(joinPkt.newPlayerId);

        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
        if (!pconn) {
            std::cerr << "ERROR: Cannot send PlayerJoinPacket, player " << player << " has no PlayerConn" << std::endl;
            return;
        }
        std::cout << "  ✓ Sent PLAYER_JOIN (id=" << new_player << ") to player " << player << std::endl;
        pconn->packet_manager.sendPacketBytesSafe(&joinPkt, sizeof(joinPkt), PLAYER_JOIN, nullptr, true);
    }


    void broadcast_projectile_spawn(ECS::EntityID room_id, uint32_t projectileId, uint32_t ownerId, float x, float y, float vx, float vy, uint16_t damage, bool piercing, bool isCharged) {
        SpawnProjectilePacket spawnPkt{};
        spawnPkt.projectileId = projectileId;
        spawnPkt.ownerId = ownerId;
        spawnPkt.x = x;
        spawnPkt.y = y;
        spawnPkt.vx = vx;
        spawnPkt.vy = vy;
        spawnPkt.damage = damage;
        spawnPkt.piercing = piercing;
        spawnPkt.isCharged = isCharged;

        // Convert to network endian
        to_network_endian(spawnPkt.projectileId);
        to_network_endian(spawnPkt.ownerId);
        to_network_endian(spawnPkt.x);
        to_network_endian(spawnPkt.y);
        to_network_endian(spawnPkt.vx);
        to_network_endian(spawnPkt.vy);
        to_network_endian(spawnPkt.damage);
        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERRxOR: Cannot broadcast SpawnProjectilePacket, room " << room_id << " not found" << std::endl;
            return;
        }
        room->broadcastPacket(&spawnPkt, sizeof(spawnPkt), SPAWN_PROJECTILE, true);
    }

    void send_lobby_state(ECS::EntityID player, uint32_t totalPlayers, uint32_t readyPlayers) {
        LobbyStatePacket pkt{};
        pkt.totalPlayers = totalPlayers;
        pkt.readyPlayers = readyPlayers;

        // Convert to network endian
        to_network_endian(pkt.totalPlayers);
        to_network_endian(pkt.readyPlayers);

        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
        if (!pconn) {
            std::cerr << "ERROR: Cannot send LobbyStatePacket, player " << player << " has no PlayerConn" << std::endl;
            return;
        }
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), LOBBY_STATE, nullptr, true);
    }

    void broadcast_enemy_spawn(ECS::EntityID room_id, uint32_t enemyId, common::components::EnemyType enemyType, float x, float y, uint16_t hp) {
        SpawnEnemyPacket pkt{};
        pkt.enemyId = enemyId;
        pkt.enemyType = static_cast<uint16_t>(enemyType);
        pkt.x = x;
        pkt.y = y;
        pkt.hp = hp;

        // Convert to network endian
        to_network_endian(pkt.enemyId);
        to_network_endian(pkt.enemyType);
        to_network_endian(pkt.x);
        to_network_endian(pkt.y);
        to_network_endian(pkt.hp);

        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast SpawnEnemyPacket, room " << room_id << " not found" << std::endl;
            return;
        }
        room->broadcastPacket(&pkt, sizeof(pkt), SPAWN_ENEMY, true);
    }

    void send_player_state(ECS::EntityID to_player, ECS::EntityID playerId, float x, float y, float dir, uint16_t hp, bool isAlive) {
        // Don't send to dead players - their connection may be invalid
        auto *toHealth = root.world.GetComponent<rtype::common::components::Health>(to_player);
        if (toHealth && (!toHealth->isAlive || toHealth->currentHp <= 0)) {
            return;
        }
        
        PlayerStatePacket pkt{};
        pkt.playerId = playerId;
        pkt.x = x;
        pkt.y = y;
        pkt.dir = dir;
        pkt.hp = hp;
        pkt.isAlive = isAlive;

        // Convert to network endian
        to_network_endian(pkt.playerId);
        to_network_endian(pkt.x);
        to_network_endian(pkt.y);
        to_network_endian(pkt.dir);
        to_network_endian(pkt.hp);

        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(to_player);
        if (!pconn) {
            std::cerr << "ERROR: Cannot send PlayerStatePacket, player " << to_player << " has no PlayerConn" << std::endl;
            return;
        }
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), PLAYER_STATE, nullptr, false);
    }

    void broadcast_all_players_state(ECS::EntityID room_id, const AllPlayersStatePacket& allStates) {
        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast AllPlayersStatePacket, room " << room_id << " not found" << std::endl;
            return;
        }

        // Create a copy to convert to network endian
        AllPlayersStatePacket pkt = allStates;

        // Convert all player data to network endian
        for (uint8_t i = 0; i < pkt.playerCount; ++i) {
            to_network_endian(pkt.players[i].playerId);
            to_network_endian(pkt.players[i].x);
            to_network_endian(pkt.players[i].y);
            to_network_endian(pkt.players[i].dir);
            to_network_endian(pkt.players[i].hp);
            // Note: booleans don't need endian conversion
        }

        // Broadcast to all players in the room
        // Use important=false since states are sent frequently and can tolerate occasional loss
        room->broadcastPacket(&pkt, sizeof(pkt), ALL_PLAYERS_STATE, false);
    }


    void broadcast_player_disconnect(ECS::EntityID room_id, uint32_t playerId) {
        PlayerDisconnectPacket pkt{};
        pkt.playerId = playerId;

        // Convert to network endian
        to_network_endian(pkt.playerId);
        auto room = root.world.GetComponent<rtype::server::components::RoomProperties>(room_id);
        if (!room) {
            std::cerr << "ERROR: Cannot broadcast PlayerDisconnectPacket, room " << room_id << " not found" << std::endl;
            return;
        }
        room->broadcastPacket(&pkt, sizeof(pkt), PLAYER_DISCONNECT, true);
    }

    void send_player_score(ECS::EntityID player, uint32_t playerId, int32_t score) {
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
        if (!pconn) {
            std::cerr << "ERROR: Cannot send PlayerScoreUpdatePacket, player " << player << " has no PlayerConn" << std::endl;
            return;
        }

        PlayerScoreUpdatePacket pkt{};
        pkt.playerId = playerId;
        pkt.score = score;

        to_network_endian(pkt.playerId);
        to_network_endian(pkt.score);

        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), PLAYER_SCORE_UPDATE, nullptr, false);
    }
}
