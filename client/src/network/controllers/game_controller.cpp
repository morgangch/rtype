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
#include <cstring>

// External references to global player info (defined in network.cpp)
extern std::string g_username;
extern uint32_t g_playerServerId;

// Track if player is admin in current room
static bool g_isPlayerAdmin = false;

// Forward declaration for lobby state access
namespace rtype::client::gui {
    class PrivateServerLobbyState;
    extern PrivateServerLobbyState* g_lobbyState;
}

namespace rtype::client::controllers::game_controller {
    void handle_join_room_accepted(const packet_t &packet) {
        JoinRoomAcceptedPacket *p = (JoinRoomAcceptedPacket *) packet.data;

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
                std::cout << "WARNING: Ignoring JOIN_ROOM_ACCEPTED while in GameState (room " << p->roomCode << ")" << std::endl;
                return;
            }
            
            // IMPORTANT: Ignore duplicate JOIN_ROOM_ACCEPTED if we're already in the lobby
            // This prevents PacketManager retransmissions from resetting the lobby state (e.g. isReady flag)
            if (g_lobbyState != nullptr) {
                std::cout << "WARNING: Ignoring duplicate JOIN_ROOM_ACCEPTED while already in lobby (room " << p->roomCode << ")" << std::endl;
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
        // TODO: Implement
    }

    void handle_spawn_enemy(const packet_t &packet) {
        SpawnEnemyPacket *p = (SpawnEnemyPacket *) packet.data;
        using rtype::client::gui::g_gameState;
        if (!g_gameState) return;

        // Create or update enemy entity on the client
        g_gameState->createEnemyFromServer(p->enemyId, p->x, p->y, p->hp, p->enemyType);
    }

    void handle_entity_destroy(const packet_t &packet) {
        EntityDestroyPacket *p = (EntityDestroyPacket *) packet.data;
        using rtype::client::gui::g_gameState;
        if (g_gameState) g_gameState->destroyEntityByServerId(p->entityId);
    }

    void handle_player_join(const packet_t &packet) {
        PlayerJoinPacket *p = (PlayerJoinPacket *) packet.data;
        using rtype::client::gui::g_gameState;
        if (g_gameState) g_gameState->createRemotePlayer(std::string(p->name), p->newPlayerId);
    }

    void handle_player_state(const packet_t &packet) {
        PlayerStatePacket *p = (PlayerStatePacket *) packet.data;
        using rtype::client::gui::g_gameState;
        if (g_gameState) g_gameState->updateEntityStateFromServer(p->playerId, p->x, p->y, p->hp);
    }
    
    void handle_lobby_state(const packet_t &packet) {
        LobbyStatePacket *p = (LobbyStatePacket *) packet.data;
        using rtype::client::gui::g_lobbyState;
        
        
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
        
        std::cout << "✓ CLIENT: Successfully transitioned to GameState (admin=" << (g_isPlayerAdmin ? "YES" : "NO") << ")" << std::endl;
    }
    
    void handle_spawn_projectile(const packet_t &packet) {
        SpawnProjectilePacket *p = (SpawnProjectilePacket *) packet.data;
        using rtype::client::gui::g_gameState;
        
        if (!g_gameState) return;
        
        // Create projectile entity from server data
        g_gameState->createProjectileFromServer(p->projectileId, p->ownerId, p->x, p->y, p->vx, p->vy, p->damage, p->piercing, p->isCharged);
    }

    void handle_admin_update(const packet_t &packet) {
        RoomAdminUpdatePacket *p = (RoomAdminUpdatePacket *) packet.data;
        ECS::EntityID newAdminId = p->newAdminPlayerId;
        // TODO: Implement
    }
}
