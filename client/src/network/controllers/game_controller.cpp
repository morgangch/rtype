/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "network/controllers/game_controller.h"
#include <iostream>
#include "packet.h"
#include <common/packets/packets.h>
#include "gui/GameState.h"
#include "gui/PrivateServerLobbyState.h"
#include <common/components/Shield.h>
#include <cstring>

#include "utils/endiane_converter.h"

// External references to global player info (defined in network.cpp)
extern std::string g_username;
extern uint32_t g_playerServerId;

// Track if player is admin in current room
static bool g_isPlayerAdmin = false;

// Forward declaration for lobby state access
namespace rtype::client::gui {
    class PrivateServerLobbyState;
    extern PrivateServerLobbyState *g_lobbyState;
}

namespace rtype::client::controllers::game_controller {
    void handle_join_room_accepted(const packet_t &packet) {
        JoinRoomAcceptedPacket *p = (JoinRoomAcceptedPacket *) packet.data;

        // Extract endianes
        from_network_endian(p->roomCode);
        from_network_endian(p->playerServerId);

        std::cout << "Successfully connected on room " << p->roomCode << " as " << (
            p->admin ? "admin" : "classic player") << " with server player ID: " << p->playerServerId << std::endl;


        // Store player server ID and admin status for use when GameState is created
        g_playerServerId = p->playerServerId;
        g_isPlayerAdmin = p->admin;

        // Transition to PrivateServerLobbyState (the lobby where admin can click "Start Game")
        using rtype::client::gui::g_stateManager;
        using rtype::client::gui::PrivateServerLobbyState;
        using rtype::client::gui::g_gameState;
        using rtype::client::gui::g_lobbyState;

        if (g_stateManager) {
            // IMPORTANT: Only transition to lobby if we're NOT already in a game
            // This prevents stale JOIN_ROOM_ACCEPTED packets from kicking us out of GameState
            if (g_gameState != nullptr) {
                std::cout << "WARNING: Ignoring JOIN_ROOM_ACCEPTED while in GameState (room " << p->roomCode << ")" <<
                        std::endl;
                return;
            }

            // IMPORTANT: Ignore duplicate JOIN_ROOM_ACCEPTED if we're already in the lobby
            // This prevents PacketManager retransmissions from resetting the lobby state (e.g. isReady flag)
            if (g_lobbyState != nullptr) {
                std::cout << "WARNING: Ignoring duplicate JOIN_ROOM_ACCEPTED while already in lobby (room " << p->
                        roomCode << ")" << std::endl;
                return;
            }

            std::string serverCodeStr = std::to_string(p->roomCode);
            bool isAdmin = p->admin;

            g_stateManager->changeState(
                std::make_unique<PrivateServerLobbyState>(*g_stateManager, g_username, serverCodeStr, isAdmin)
            );
        } else {
            std::cerr << "StateManager global not set; cannot transition to lobby." << std::endl;
        }
    }

    void handle_player_disconnect(const packet_t &packet) {
        PlayerDisconnectPacket *p = (PlayerDisconnectPacket *) packet.data;

        // Extract endianes
        from_network_endian(p->playerId);
        // TODO: Implement
    }

    void handle_spawn_enemy(const packet_t &packet) {
        SpawnEnemyPacket *p = (SpawnEnemyPacket *) packet.data;

        // Extract endianes
        from_network_endian(p->enemyId);
        from_network_endian(p->enemyType);
        from_network_endian(p->hp);
        from_network_endian(p->x);
        from_network_endian(p->y);

        std::cout << "[CLIENT] Received SPAWN_ENEMY: id=" << p->enemyId << " type=" << static_cast<int>(p->enemyType)
                  << " pos=(" << p->x << "," << p->y << ") hp=" << p->hp << std::endl;

        using rtype::client::gui::g_gameState;
        if (!g_gameState) {
            std::cout << "[CLIENT] ERROR: Cannot spawn enemy - g_gameState is null!" << std::endl;
            return;
        }

        // Create or update enemy entity on the client
        g_gameState->createEnemyFromServer(p->enemyId, p->x, p->y, p->hp, p->enemyType);
    }

    void handle_entity_destroy(const packet_t &packet) {
        EntityDestroyPacket *p = (EntityDestroyPacket *) packet.data;
        // Extract endianes
        from_network_endian(p->entityId);
        from_network_endian(p->reason);

        using rtype::client::gui::g_gameState;
        if (g_gameState) g_gameState->destroyEntityByServerId(p->entityId);
    }

    void handle_player_join(const packet_t &packet) {
        PlayerJoinPacket *p = (PlayerJoinPacket *) packet.data;

        // Extract endianes
        from_network_endian(p->newPlayerId);

        using rtype::client::gui::g_gameState;
        if (g_gameState) {
            // Convert uint8_t to VesselType enum
            rtype::common::components::VesselType vesselType = 
                static_cast<rtype::common::components::VesselType>(p->vesselType);
            g_gameState->createRemotePlayer(std::string(p->name), p->newPlayerId, vesselType);
        }
    }

    void handle_player_state(const packet_t &packet) {
        PlayerStatePacket *p = (PlayerStatePacket *) packet.data;

        // Extract endianes
        from_network_endian(p->playerId);
        from_network_endian(p->x);
        from_network_endian(p->y);
        from_network_endian(p->hp);
        from_network_endian(p->maxHp);

        using rtype::client::gui::g_gameState;
        if (g_gameState) g_gameState->updateEntityStateFromServer(p->playerId, p->x, p->y, p->hp, p->invulnerable, p->maxHp);
    }

    void handle_lobby_state(const packet_t &packet) {
        LobbyStatePacket *p = (LobbyStatePacket *) packet.data;
        using rtype::client::gui::g_lobbyState;

        // Extract endianes
        from_network_endian(p->totalPlayers);
        from_network_endian(p->readyPlayers);

        // Update the lobby state display if we're in the lobby
        if (g_lobbyState) {
            g_lobbyState->updateFromServer(p->totalPlayers);
        }
    }

    void handle_game_start(const packet_t &packet) {
        using rtype::client::gui::g_stateManager;
        using rtype::client::gui::GameState;
        using rtype::client::gui::g_lobbyState;

        std::cout << "=== CLIENT: GAME_START packet received from server ===" << std::endl;
        std::cout << "CLIENT: Transitioning to GameState with playerServerId=" << g_playerServerId << std::endl;

        if (!g_stateManager) {
            std::cerr << "ERROR: g_stateManager is null, cannot transition to GameState" << std::endl;
            return;
        }

        // Clear lobby state pointer
        g_lobbyState = nullptr;

        // Create and transition to GameState with the local player server ID and admin status
        auto gameState = std::make_unique<GameState>(*g_stateManager);
        gameState->setLocalPlayerServerId(g_playerServerId);
        gameState->setIsAdmin(g_isPlayerAdmin);
        g_stateManager->changeState(std::move(gameState));

        std::cout << "✓ CLIENT: Successfully transitioned to GameState (admin=" << (g_isPlayerAdmin ? "YES" : "NO") <<
                ")" << std::endl;
    }

    void handle_spawn_projectile(const packet_t &packet) {
        SpawnProjectilePacket *p = (SpawnProjectilePacket *) packet.data;

        // Extract endianes
        from_network_endian(p->projectileId);
        from_network_endian(p->ownerId);
        from_network_endian(p->x);
        from_network_endian(p->y);
        from_network_endian(p->vx);
        from_network_endian(p->vy);
        from_network_endian(p->damage);

        using rtype::client::gui::g_gameState;

        if (!g_gameState) return;

        // Create projectile entity from server data
        g_gameState->createProjectileFromServer(p->projectileId, p->ownerId, p->x, p->y, p->vx, p->vy, p->damage,
                                                p->piercing, p->isCharged);
    }

    void handle_admin_update(const packet_t &packet) {
        RoomAdminUpdatePacket *p = (RoomAdminUpdatePacket *) packet.data;

        // Extract endianes
        from_network_endian(p->newAdminPlayerId);

        ECS::EntityID newAdminId = p->newAdminPlayerId;
        // TODO: Implement
    }

    void handle_player_score_update(const packet_t &packet) {
        PlayerScoreUpdatePacket *p = (PlayerScoreUpdatePacket *) packet.data;
        from_network_endian(p->playerId);
        from_network_endian(p->score);

        using rtype::client::gui::g_gameState;
        if (!g_gameState) return;

        // If update is for the local player, reflect on HUD
        // For simplicity, accept and set regardless; multi-player HUD can filter by playerId later.
        g_gameState->setScoreFromServer(static_cast<int>(p->score));
    }

    void handle_shield_state(const packet_t &packet) {
        using namespace rtype::client::gui;
        
        if (!g_gameState) return;

        ShieldStatePacket *p = (ShieldStatePacket*) packet.data;
        from_network_endian(p->playerId);
        from_network_endian(p->duration);

        // Update shield state via GameState method
        g_gameState->updateShieldStateFromServer(p->playerId, p->isActive, p->duration);
    }

}
