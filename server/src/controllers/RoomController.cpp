/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "controllers/RoomController.h"
#include "packets.h"
#include "rtype.h"
#include "tools.h"
#include "components/PlayerConn.h"
#include "components/RoomProperties.h"
#include "components/LobbyState.h"
#include "ECS/Types.h"
#include "services/PlayerService.h"
#include "services/RoomService.h"
#include <common/components/Player.h>
#include <common/components/Velocity.h>
#include <common/components/Position.h>
#include <common/components/Team.h>
#include <common/components/Projectile.h>
#include <common/components/Health.h>
#include <common/components/EnemyType.h>
#include <iostream>
#include <cstring>
#include <cmath>

// Platform-specific network headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <common/utils/bytes_printer.h>

#include "senders.h"
#include "common/utils/endiane_converter.h"
#include "components/LinkedRoom.h"

using namespace rtype::server::controllers;
using namespace rtype::server::services;

// ============================================================================
// HELPER FUNCTIONS - Game Start Flow
// ============================================================================

void room_controller::markPlayersAsInGame(ECS::EntityID room) {
    auto players_in_room = player_service::findPlayersByRoomCode(room);
    std::cout << "Found " << players_in_room.size() << " players in room" << std::endl;

    for (auto pid: players_in_room) {
        auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        if (lobbyState) {
            lobbyState->isInGame = true;
            std::cout << "  - Marked player " << pid << " as in-game" << std::endl;
        }
    }
}

void room_controller::broadcastGameStart(ECS::EntityID room) {
    network::senders::broadcast_game_start(room);
}

void room_controller::broadcastPlayerRoster(ECS::EntityID room) {
    auto players_in_room = player_service::findPlayersByRoomCode(room);

    // Send all PLAYER_JOIN packets to each client so they can create remote player entities
    for (auto recipientPid: players_in_room) {
        auto *recipientConn = root.world.GetComponent<rtype::server::components::PlayerConn>(recipientPid);
        if (!recipientConn) continue;

        // Send info about all OTHER players in the room to this recipient
        for (auto otherPid: players_in_room) {
            if (otherPid == recipientPid) continue; // Don't send self

            auto *otherPlayer = root.world.GetComponent<rtype::common::components::Player>(otherPid);
            if (!otherPlayer) continue;

            network::senders::send_player_join(recipientPid, otherPid, otherPlayer->name);
        }
    }
}

// ============================================================================
// HELPER FUNCTIONS - Join Room Flow
// ============================================================================

ECS::EntityID room_controller::findOrCreatePlayer(const packet_t &packet, const std::string &playerName,
                                                  uint32_t joinCode, const std::string &ipStr, int port) {
    // Look up player by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);

    // Check if player already exists and is in an active game
    if (player) {
        auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);
        if (lobbyState && lobbyState->isInGame) {
            std::cout << "WARNING: Ignoring JOIN_ROOM from player " << player << " who is already in a game" <<
                    std::endl;
            return 0; // Signal to abort join request
        }
        return player; // Existing player, not in game - allow rejoin
    }

    // Player doesn't exist, create new one
    player = player_service::createNewPlayer(playerName, joinCode, ipStr, port);

    // Re-check to ensure player was created successfully
    player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Failed to create or find player entity for network address " << ipStr << ":" << port <<
                std::endl;
    }

    return player;
}

ECS::EntityID room_controller::findOrCreateRoom(uint32_t joinCode, ECS::EntityID player) {
    if (joinCode == 0) {
        // Create a new private room
        return room_service::openNewRoom(false, player);
    } else if (joinCode == 1) {
        // Join a random public room (matchmaking)
        ECS::EntityID room = room_service::findAvailablePublicRoom();
        if (room == 0) {
            // No available public room, create a new one
            room = room_service::openNewRoom(true, player);
        }
        return room;
    } else {
        // Join a specific private room with the given join code
        return room_service::getRoomByJoinCode(static_cast<int>(joinCode));
    }
}

void room_controller::sendJoinAccepted(ECS::EntityID player, ECS::EntityID room) {
    auto *playernet = root.world.GetComponent<components::PlayerConn>(player);
    if (!playernet) {
        std::cerr << "ERROR: Player " << player << " has no PlayerConn component" << std::endl;
        return;
    }

    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!rp) {
        std::cerr << "ERROR: Room " << room << " has no RoomProperties component" << std::endl;
        return;
    }

    // Store room entity ID in player's connection (note: field name 'room_code' is misleading, it stores entity ID)
    playernet->room_code = room;

    std::cout << "Sending JOIN_ROOM_ACCEPTED to player " << player << " (isAdmin=" <<
            (rp->ownerId == player) << ", roomCode=" << rp->joinCode << ", playerServerId=" << player << ")" <<
            std::endl;
    network::senders::send_join_room_accepted(player, (rp->ownerId == player), rp->joinCode,
                                              static_cast<uint32_t>(player));
}

void room_controller::notifyJoiningPlayerOfExisting(ECS::EntityID player, ECS::EntityID room) {
    auto *playernet = root.world.GetComponent<components::PlayerConn>(player);
    if (!playernet) return;

    auto players_in_room = player_service::findPlayersByRoomCode(room);

    // Send existing players to the newly joined player
    for (auto existing: players_in_room) {
        if (existing == player) continue; // Don't send self

        auto *existingPlayer = root.world.GetComponent<rtype::common::components::Player>(existing);
        if (!existingPlayer) continue;

        network::senders::send_player_join(player, existing, existingPlayer->name);
    }
}

void room_controller::notifyExistingPlayersOfNewJoin(ECS::EntityID player, const std::string &playerName,
                                                     ECS::EntityID room) {
    auto players_in_room = player_service::findPlayersByRoom(room);

    for (auto other: players_in_room) {
        if (other == player) continue; // Don't send to self
        network::senders::send_player_join(other, player, playerName);
    }
}

void room_controller::initializeLobbyState(ECS::EntityID player) {
    // Only add LobbyState if player doesn't already have one
    // This prevents duplicate JOIN_ROOM packets from resetting a player's ready state
    auto *existingLobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);

    if (!existingLobbyState) {
        std::cout << "Adding new LobbyState for player " << player << std::endl;
        root.world.AddComponent<rtype::server::components::LobbyState>(player, false, false);
    } else {
        std::cout << "Player " << player << " already has LobbyState (ready=" << existingLobbyState->isReady <<
                "), not resetting" << std::endl;
    }
}

// ============================================================================
// HELPER FUNCTIONS - Projectile Handling
// ============================================================================

ECS::EntityID room_controller::createServerProjectile(ECS::EntityID room, float x, float y, bool isCharged) {
    auto projectile = root.world.CreateEntity();

    // Projectile starts at given position with offset
    float projectileX = x + 32.0f; // Offset to spawn in front of player
    float projectileY = y;

    // Projectile parameters depend on whether it's charged or not
    float projectileSpeed;
    uint16_t damage;
    bool piercing;

    if (isCharged) {
        projectileSpeed = 600.0f;
        damage = 2;
        piercing = true;
    } else {
        projectileSpeed = 500.0f;
        damage = 1;
        piercing = false;
    }

    float projectileVx = projectileSpeed;
    float projectileVy = 0.0f;

    // Add components to server projectile
    root.world.AddComponent<rtype::common::components::Position>(projectile, projectileX, projectileY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(projectile, projectileVx, projectileVy,
                                                                 projectileSpeed);
    root.world.AddComponent<rtype::common::components::Team>(projectile, rtype::common::components::TeamType::Player);
    root.world.AddComponent<rtype::common::components::Projectile>(projectile, damage, piercing, true /* serverOwned */,
                                                                   projectileSpeed,
                                                                   rtype::common::components::ProjectileType::Basic);
    root.world.AddComponent<rtype::server::components::LinkedRoom>(projectile, room);
    return projectile;
}

void room_controller::broadcastProjectileSpawn(ECS::EntityID projectile, ECS::EntityID owner, ECS::EntityID room,
                                               bool isCharged) {
    auto *pos = root.world.GetComponent<rtype::common::components::Position>(projectile);
    auto *vel = root.world.GetComponent<rtype::common::components::Velocity>(projectile);
    auto *proj = root.world.GetComponent<rtype::common::components::Projectile>(projectile);

    if (!pos || !vel || !proj) return;

    network::senders::broadcast_projectile_spawn(
        room, static_cast<uint32_t>(projectile), static_cast<uint32_t>(owner),
        pos->x, pos->y, vel->vx, vel->vy, proj->damage, proj->piercing, isCharged
    );
}

// ============================================================================
// PACKET HANDLERS
// ============================================================================

void room_controller::handleGameStartRequest(const packet_t &packet) {
    std::cout << "=== handleGameStartRequest called ===" << std::endl;

    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found" << std::endl;
        return;
    }

    ECS::EntityID room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Player " << player << " not in a room" << std::endl;
        return;
    }

    // Check if the player is the room owner
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (rp->ownerId != player) {
        std::string ip_str = rtype::tools::ipToString(const_cast<uint8_t *>(packet.header.client_addr));
        std::cerr << "ERROR: Player " << player << " is not room owner (owner is " << rp->ownerId << ")" << std::endl;
        std::cerr << "SECURITY AUDIT: Unauthorized GAME_START_REQUEST attempt by player " << player
                << " (IP: " << ip_str << ", port: " << packet.header.client_port << ")"
                << " for room " << rp->joinCode << " (owner is " << rp->ownerId << ")" << std::endl;
        return;
    }
    if (rp->isGameStarted) {
        std::cout << "WARNING: Game already started for room " << rp->joinCode << std::endl;
        return;
    }

    rp->isGameStarted = true;
    std::cout << "✓ Game started for room " << rp->joinCode << " by admin player " << player << std::endl;

    // Use extracted helper functions
    markPlayersAsInGame(room);
    broadcastGameStart(room);
    broadcastPlayerRoster(room);
}

void room_controller::handlePlayerInput(const packet_t &packet) {
    PlayerInputPacket *p = (PlayerInputPacket *) packet.data;

    // Find the player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) return;

    // Get player's velocity and position components
    auto *vel = root.world.GetComponent<rtype::common::components::Velocity>(player);
    auto *pos = root.world.GetComponent<rtype::common::components::Position>(player);
    if (!vel || !pos) return;

    // Calculate movement direction from input
    float moveX = 0.0f, moveY = 0.0f;

    if (p->moveUp) moveY -= 1.0f;
    if (p->moveDown) moveY += 1.0f;
    if (p->moveLeft) moveX -= 1.0f;
    if (p->moveRight) moveX += 1.0f;

    // Normalize diagonal movement
    float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
    if (magnitude > 0.0f) {
        moveX /= magnitude;
        moveY /= magnitude;
    }

    // Apply velocity (use same speed as client: 400.0f)
    constexpr float PLAYER_SPEED = 400.0f;
    vel->vx = moveX * PLAYER_SPEED;
    vel->vy = moveY * PLAYER_SPEED;

    // Note: Position will be updated by the movement system
    // Server will broadcast updated position via PLAYER_STATE packets
}

void room_controller::handlePlayerShoot(const packet_t &packet) {
    // Parse packet to get charged shot flag and player position
    PlayerShootPacket *p = (PlayerShootPacket *) packet.data;

    // Extract endianness
    from_network_endian(p->playerX);
    from_network_endian(p->playerY);

    bool isCharged = p->isCharged;
    float playerX = p->playerX;
    float playerY = p->playerY;

    // Find the player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found for shooting" << std::endl;
        return;
    }

    // Get player's room to broadcast to all clients
    ECS::EntityID room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Player " << player << " not in a room" << std::endl;
        return;
    }

    // Use extracted helper functions
    ECS::EntityID projectile = createServerProjectile(room, playerX, playerY, isCharged);
    std::cout << "Player " << player << " shot a "
              << (isCharged ? "CHARGED" : "regular") << " projectile (entity " << projectile << ") from position ("
              << playerX << ", " << playerY << ")" << std::endl;
    broadcastProjectileSpawn(projectile, player, room, isCharged);
}

void room_controller::handleJoinRoomPacket(const packet_t &packet) {
    JoinRoomPacket *p = (JoinRoomPacket *) packet.data;

    // Extract endianes
    from_network_endian(p->joinCode);
    std::string ip_str = rtype::tools::ipToString(const_cast<uint8_t *>(packet.header.client_addr));

    // Find or create player entity, checking for active game conflict
    ECS::EntityID player = findOrCreatePlayer(packet, std::string(p->name), p->joinCode, ip_str,
                                              packet.header.client_port);
    if (!player) return; // Player in active game or creation failed

    // Find or create room based on join code
    ECS::EntityID room = findOrCreateRoom(p->joinCode, player);
    if (!room) {
        std::cerr << "ERROR: Room not found for join code " << p->joinCode << std::endl;
        return;
    }

    // Register the room association for the player
    root.world.AddComponent<components::LinkedRoom>(player, room);

    // Send join acceptance to the player
    sendJoinAccepted(player, room);

    // Notify joining player of existing players in room
    notifyJoiningPlayerOfExisting(player, room);

    // Notify existing players of the new player joining
    notifyExistingPlayersOfNewJoin(player, std::string(p->name), room);

    // Initialize lobby state if not already present
    initializeLobbyState(player);

    // Broadcast updated lobby state to all players in the room
    broadcastLobbyState(room);
}

void room_controller::handlePlayerReady(const packet_t &packet) {
    std::cout << "=== handlePlayerReady called (PUBLIC ROOMS ONLY) ===" << std::endl;

    PlayerReadyPacket *p = (PlayerReadyPacket *) packet.data;

    // Extract endianes
    // (none needed, just a bool)

    // Find the player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found by network address" << std::endl;
        return;
    }

    // Get player's room to check if it's public
    ECS::EntityID room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Room not found for player " << player << std::endl;
        return;
    }

    auto *roomProps = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!roomProps) {
        std::cerr << "ERROR: Room properties not found" << std::endl;
        return;
    }

    // Only process ready states for PUBLIC rooms
    // Private rooms don't use ready system - admin just starts when ready
    if (!roomProps->isPublic) {
        std::cout << "INFO: Ignoring PLAYER_READY for private room " << roomProps->joinCode << " (admin-controlled)" <<
                std::endl;
        return;
    }

    std::cout << "Player " << player << " toggling ready to: " << (p->isReady ? "READY" : "NOT READY") <<
            " in public room" << std::endl;

    // Get player's lobby state component
    auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);
    if (!lobbyState) {
        // Player doesn't have lobby state, add it
        std::cout << "Adding LobbyState component to player " << player << std::endl;
        root.world.AddComponent<rtype::server::components::LobbyState>(player, p->isReady, false);
    } else {
        // Update ready state
        std::cout << "Updating existing LobbyState for player " << player << std::endl;
        lobbyState->isReady = p->isReady;
    }

    std::cout << "Player " << player << " is in public room " << room << ", broadcasting lobby state" << std::endl;

    // Broadcast updated lobby state to all players in the room
    broadcastLobbyState(room);
}

void room_controller::broadcastLobbyState(ECS::EntityID room) {
    // Get room properties to check if public/private
    auto *roomProps = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!roomProps) {
        std::cerr << "ERROR: Room properties not found for broadcastLobbyState" << std::endl;
        return;
    }

    // Get all players in this room
    auto players_in_room = player_service::findPlayersByRoomCode(room);

    // Count total players and ready players (only count those not in game yet)
    uint32_t totalPlayers = 0;
    uint32_t readyPlayers = 0;

    for (auto pid: players_in_room) {
        auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);

        // If player doesn't have LobbyState, they're in lobby but not ready
        if (!lobbyState) {
            totalPlayers++;
            continue;
        }

        // Skip players who have transitioned to game already
        if (lobbyState->isInGame) continue;

        totalPlayers++;

        // Only count ready players for PUBLIC rooms
        if (roomProps->isPublic && lobbyState->isReady) {
            readyPlayers++;
        }
    }

    // Build lobby state packet
    // LobbyStatePacket pkt{};
    // pkt.totalPlayers = totalPlayers;
    // pkt.readyPlayers = readyPlayers; // Will be 0 for private rooms

    if (roomProps->isPublic) {
        std::cout << "Broadcasting PUBLIC lobby state: " << totalPlayers << " players, " << readyPlayers << " ready" <<
                std::endl;
    } else {
        std::cout << "Broadcasting PRIVATE lobby state: " << totalPlayers << " players (no ready count for private)" <<
                std::endl;
    }

    // Send to all players in the room (including those in lobby)
    for (auto pid: players_in_room) {
        auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        // Only send to players still in lobby (not yet in game)
        if (lobbyState && lobbyState->isInGame) continue;

        network::senders::send_lobby_state(
            pid,
            totalPlayers,
            readyPlayers
        );
    }
}

void room_controller::handleSpawnBossRequest(const packet_t &packet) {
    std::cout << "=== handleSpawnBossRequest called ===" << std::endl;

    // Find the requesting player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found by network address" << std::endl;
        return;
    }

    std::cout << "Player " << player << " requesting boss spawn" << std::endl;

    // Get player's room
    auto *playerConn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
    if (!playerConn) {
        std::cerr << "ERROR: Player has no connection component" << std::endl;
        return;
    }

    ECS::EntityID room = playerConn->room_code;
    if (!room) {
        std::cerr << "ERROR: Player not in a room" << std::endl;
        return;
    }

    // Verify that the player is the admin of the room
    auto *roomProps = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!roomProps) {
        std::cerr << "ERROR: Room properties not found" << std::endl;
        return;
    }

    if (roomProps->ownerId != player) {
        std::cout << "WARNING: Player " << player << " is not admin of room " << room << ", boss spawn request denied"
                << std::endl;
        return;
    }

    // Verify the game has started
    if (!roomProps->isGameStarted) {
        std::cout << "WARNING: Game not started in room " << room << ", boss spawn request denied" << std::endl;
        return;
    }

    std::cout << "✓ Admin " << player << " authorized to spawn boss in room " << room << std::endl;

    // Check if a boss already exists
    bool bossExists = false;
    auto *enemyTypes = root.world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (enemyTypes) {
        for (auto &etPair: *enemyTypes) {
            auto *et = etPair.second.get();
            if (et && et->type == rtype::common::components::EnemyType::Boss) {
                auto *health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
                if (health && health->isAlive && health->currentHp > 0) {
                    bossExists = true;
                    std::cout << "SERVER: Boss already exists (id=" << etPair.first << "), skipping spawn" << std::endl;
                    break;
                }
            }
        }
    }

    if (bossExists) {
        std::cout << "WARNING: Boss already exists in room " << room << ", spawn request denied" << std::endl;
        return;
    }

    // Spawn the boss
    float spawnX = 1280.0f - 100.0f;
    float spawnY = 360.0f;

    auto boss = root.world.CreateEntity();
    root.world.AddComponent<rtype::common::components::Position>(boss, spawnX, spawnY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(boss, -50.0f, 0.0f, 50.0f);
    root.world.AddComponent<rtype::common::components::Health>(boss, 50);
    root.world.AddComponent<rtype::common::components::Team>(boss, rtype::common::components::TeamType::Enemy);
    root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        boss, rtype::common::components::EnemyType::Boss);
    root.world.AddComponent<rtype::server::components::LinkedRoom>(boss, room);
    std::cout << "SERVER: Admin spawned boss (id=" << boss << ") in room " << room << std::endl;

    network::senders::broadcast_enemy_spawn(room, static_cast<uint32_t>(boss),
                                            rtype::common::components::EnemyType::Boss,
                                            spawnX, spawnY, 50);
}

// Register all packet callbacks on a player's packet handler
void room_controller::registerPlayerCallbacks(PacketHandler &handler) {
    handler.registerCallback(Packets::JOIN_ROOM, handleJoinRoomPacket);
    handler.registerCallback(Packets::GAME_START_REQUEST, handleGameStartRequest);
    handler.registerCallback(Packets::PLAYER_INPUT, handlePlayerInput);
    handler.registerCallback(Packets::PLAYER_READY, handlePlayerReady);
    handler.registerCallback(Packets::PLAYER_SHOOT, handlePlayerShoot);
    handler.registerCallback(Packets::SPAWN_BOSS_REQUEST, handleSpawnBossRequest);
    std::cout <<
            "✓ Registered player callbacks: JOIN_ROOM, GAME_START_REQUEST, PLAYER_INPUT, PLAYER_READY, PLAYER_SHOOT, SPAWN_BOSS_REQUEST"
            << std::endl;
}
