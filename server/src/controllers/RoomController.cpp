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
#include "../../../common/components/Player.h"
#include "../../../common/components/Velocity.h"
#include "../../../common/components/Position.h"
#include "../../../common/components/Team.h"
#include "../../../common/components/Projectile.h"
#include "../../../common/components/Health.h"
#include "../../../common/components/EnemyType.h"
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

#include "../../common/utils/bytes_printer.h"

using namespace rtype::server::controllers;
using namespace rtype::server::services;


void room_controller::handleGameStartRequest(const packet_t &packet) {
    std::cout << "=== handleGameStartRequest called ===" << std::endl;
    
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    ECS::EntityID room;

    if (!player) {
        std::cerr << "ERROR: Player not found" << std::endl;
        return;
    }
    
    room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Player " << player << " not in a room" << std::endl;
        return;
    }
    
    // Check if the player is the room owner
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (rp->ownerId != player) {
        std::cerr << "ERROR: Player " << player << " is not room owner (owner is " << rp->ownerId << ")" << std::endl;
        return;
    }
    
    if (rp->isGameStarted) {
        std::cout << "WARNING: Game already started for room " << rp->joinCode << std::endl;
        return;
    }
    
    rp->isGameStarted = true;
    
    std::cout << "✓ Game started for room " << rp->joinCode << " by admin player " << player << std::endl;
    
    // Mark all players in the room as now in-game (no longer in lobby)
    auto players_in_room = player_service::findPlayersByRoomCode(room);
    std::cout << "Found " << players_in_room.size() << " players in room" << std::endl;
    
    for (auto pid : players_in_room) {
        auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        if (lobbyState) {
            lobbyState->isInGame = true;
            std::cout << "  - Marked player " << pid << " as in-game" << std::endl;
        }
    }
    
    // Broadcast GAME_START to all clients in the room so they all transition to GameState
    GameStartPacket startPkt{};
    for (auto pid : players_in_room) {
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) {
            std::cerr << "  - WARNING: Player " << pid << " has no PlayerConn" << std::endl;
            continue;
        }
        pconn->packet_manager.sendPacketBytesSafe(&startPkt, sizeof(startPkt), GAME_START, nullptr, true);
        std::cout << "  ✓ Sent GAME_START to player " << pid << std::endl;
    }
    
    // CRITICAL: After sending GAME_START, send all PLAYER_JOIN packets to each client
    // so they can create remote player entities when entering GameState
    for (auto recipientPid : players_in_room) {
        auto *recipientConn = root.world.GetComponent<rtype::server::components::PlayerConn>(recipientPid);
        if (!recipientConn) continue;
        
        // Send info about all OTHER players in the room to this recipient
        for (auto otherPid : players_in_room) {
            if (otherPid == recipientPid) continue; // Don't send self
            
            auto *otherPlayer = root.world.GetComponent<rtype::common::components::Player>(otherPid);
            if (!otherPlayer) continue;
            
            PlayerJoinPacket joinPkt{};
            joinPkt.newPlayerId = static_cast<uint32_t>(otherPid);
            strncpy(joinPkt.name, otherPlayer->name.c_str(), sizeof(joinPkt.name) - 1);
            joinPkt.name[sizeof(joinPkt.name) - 1] = '\0';
            
            recipientConn->packet_manager.sendPacketBytesSafe(&joinPkt, sizeof(joinPkt), PLAYER_JOIN, nullptr, true);
            std::cout << "  ✓ Sent PLAYER_JOIN (id=" << otherPid << ") to player " << recipientPid << std::endl;
        }
    }
}

void room_controller::handlePlayerInput(const packet_t &packet) {
    PlayerInputPacket *p = (PlayerInputPacket *) packet.data;
    
    // Find the player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) return;
    
    // Get player's velocity and position components
    auto* vel = root.world.GetComponent<rtype::common::components::Velocity>(player);
    auto* pos = root.world.GetComponent<rtype::common::components::Position>(player);
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
    std::cout << "=== handlePlayerShoot called ===" << std::endl;
    
    // Parse packet to get charged shot flag and player position
    PlayerShootPacket *p = (PlayerShootPacket *) packet.data;
    bool isCharged = p->isCharged;
    float playerX = p->playerX;
    float playerY = p->playerY;
    
    std::cout << "  Shot type: " << (isCharged ? "CHARGED" : "NORMAL") << " from position (" << playerX << "," << playerY << ")" << std::endl;
    
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
    
    // Create projectile entity on server
    auto projectile = root.world.CreateEntity();
    
    // Projectile starts at player's reported position with offset (32px is player width from old code)
    float projectileX = playerX + 32.0f; // Offset to spawn in front of player
    float projectileY = playerY;
    
    // Projectile parameters depend on whether it's charged or not
    float projectileSpeed;
    uint16_t damage;
    bool piercing;
    
    if (isCharged) {
        // Charged shot: faster, more damage, piercing (matches old createChargedProjectile)
        projectileSpeed = 600.0f;
        damage = 2;
        piercing = true;
        std::cout << "  Creating CHARGED projectile: speed=" << projectileSpeed << ", damage=" << damage << ", piercing=YES" << std::endl;
    } else {
        // Normal shot (matches old createPlayerProjectile)
        projectileSpeed = 500.0f;
        damage = 1;
        piercing = false;
        std::cout << "  Creating NORMAL projectile: speed=" << projectileSpeed << ", damage=" << damage << ", piercing=NO" << std::endl;
    }
    
    float projectileVx = projectileSpeed;
    float projectileVy = 0.0f;
    
    // Add components to server projectile
    root.world.AddComponent<rtype::common::components::Position>(projectile, projectileX, projectileY, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(projectile, projectileVx, projectileVy, projectileSpeed);
    root.world.AddComponent<rtype::common::components::Team>(projectile, rtype::common::components::TeamType::Player);
    root.world.AddComponent<rtype::common::components::Projectile>(projectile, damage, piercing, true /* serverOwned */, projectileSpeed, rtype::common::components::ProjectileType::Basic);
    
    std::cout << "✓ Created server projectile entity " << projectile << " at (" << projectileX << "," << projectileY << ") [SERVER-OWNED]" << std::endl;
    
    // Broadcast SPAWN_PROJECTILE to all clients in the room
    SpawnProjectilePacket spawnPkt{};
    spawnPkt.projectileId = static_cast<uint32_t>(projectile);
    spawnPkt.ownerId = static_cast<uint32_t>(player);
    spawnPkt.x = projectileX;
    spawnPkt.y = projectileY;
    spawnPkt.vx = projectileVx;
    spawnPkt.vy = projectileVy;
    spawnPkt.damage = damage;
    spawnPkt.piercing = piercing;
    spawnPkt.isCharged = isCharged;
    
    auto players_in_room = player_service::findPlayersByRoomCode(room);
    for (auto pid : players_in_room) {
        auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        if (!lobbyState || !lobbyState->isInGame) continue; // Only send to players in-game
        
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue;
        
        pconn->packet_manager.sendPacketBytesSafe(&spawnPkt, sizeof(spawnPkt), SPAWN_PROJECTILE, nullptr, false);
        std::cout << "  ✓ Sent SPAWN_PROJECTILE to player " << pid << std::endl;
    }
}

void room_controller::handleJoinRoomPacket(const packet_t &packet) {
    JoinRoomPacket *p = (JoinRoomPacket *) packet.data;

    ECS::EntityID room = 0;
    // convert ip to string
    std::string ip_str = rtype::tools::ipToString(const_cast<uint8_t*>(packet.header.client_addr));

    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    
    // Check if player already exists and is in a game
    if (player) {
        auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);
        if (lobbyState && lobbyState->isInGame) {
            std::cout << "WARNING: Ignoring JOIN_ROOM from player " << player << " who is already in a game" << std::endl;
            return; // Ignore duplicate JOIN_ROOM packets during active games
        }
    }
    
    if (!player)
        player = player_service::createNewPlayer(std::string(p->name), p->joinCode, ip_str,
                                                           packet.header.client_port);

    if (p->joinCode == 0) {
        // Create a new private room
        room = room_service::openNewRoom(false, player);
    } else if (p->joinCode == 1) {
        // Join a random public room
        room = room_service::findAvailablePublicRoom();
        if (room == 0) {
            // No available public room, create a new one
            room = room_service::openNewRoom(true, player);
        }
    } else {
        // Join a private room with the given join code
        room = room_service::getRoomByJoinCode(static_cast<int>(p->joinCode));
    }

    if (room == 0) {
        // Room not found.
        return;
    }

    // Allow the client to join the room
    JoinRoomAcceptedPacket a{};
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    a.admin = (rp->ownerId == player);
    a.roomCode = rp->joinCode;
    a.playerServerId = static_cast<uint32_t>(player); // Send the player's server entity ID
    std::cout << "Player " << player << " joined room " << rp->joinCode << " (admin: " << (a.admin ? "yes" : "no") << ")" << std::endl;
    auto playernet = root.world.GetComponent<components::PlayerConn>(player);
    if (!playernet)
        return;

    // Ensure the player's connection stores the actual room entity id (not the numeric join code)
    playernet->room_code = room;

    // Send JoinRoomAccepted to the player
    playernet->packet_manager.sendPacketBytesSafe(&a, sizeof(a), JOIN_ROOM_ACCEPTED, nullptr, true);

    // Get all players already in the room
    auto players_in_room = rtype::server::services::player_service::findPlayersByRoomCode(room);

    // Send existing players to the newly joined player
    for (auto existing : players_in_room) {
        if (existing == player) continue; // don't send self
        auto *existingPlayer = root.world.GetComponent<rtype::common::components::Player>(existing);
        if (!existingPlayer) continue;

        PlayerJoinPacket existingPkt{};
        existingPkt.newPlayerId = static_cast<uint32_t>(existing);
        strncpy(existingPkt.name, existingPlayer->name.c_str(), sizeof(existingPkt.name) - 1);
        existingPkt.name[sizeof(existingPkt.name) - 1] = '\0';
        playernet->packet_manager.sendPacketBytesSafe(&existingPkt, sizeof(existingPkt), PLAYER_JOIN, nullptr, true);
    }

    // Notify other players in the room that a new player joined
    PlayerJoinPacket joinPkt{};
    joinPkt.newPlayerId = static_cast<uint32_t>(player);
    strncpy(joinPkt.name, p->name, sizeof(joinPkt.name) - 1);
    joinPkt.name[sizeof(joinPkt.name) - 1] = '\0';

    for (auto other : players_in_room) {
        if (other == player) continue;
        auto *otherConn = root.world.GetComponent<rtype::server::components::PlayerConn>(other);
        if (!otherConn) continue;
        otherConn->packet_manager.sendPacketBytesSafe(&joinPkt, sizeof(joinPkt), PLAYER_JOIN, nullptr, true);
    }
    
    // Add LobbyState component to the new player ONLY if they don't already have one
    // This prevents duplicate JOIN_ROOM packets from resetting a player's ready state
    auto* existingLobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);
    if (!existingLobbyState) {
        std::cout << "Adding new LobbyState for player " << player << std::endl;
        root.world.AddComponent<rtype::server::components::LobbyState>(player, false, false);
    } else {
        std::cout << "Player " << player << " already has LobbyState (ready=" << existingLobbyState->isReady << "), not resetting" << std::endl;
    }
    
    // Broadcast updated lobby state to all players in the room
    broadcastLobbyState(room);
}

void room_controller::handlePlayerReady(const packet_t &packet) {
    std::cout << "=== handlePlayerReady called ===" << std::endl;
    
    PlayerReadyPacket *p = (PlayerReadyPacket *) packet.data;
    
    // Find the player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found by network address" << std::endl;
        return;
    }
    
    std::cout << "Player " << player << " toggling ready to: " << (p->isReady ? "READY" : "NOT READY") << std::endl;
    
    // Get player's lobby state component
    auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(player);
    if (!lobbyState) {
        // Player doesn't have lobby state, add it
        std::cout << "Adding LobbyState component to player " << player << std::endl;
        root.world.AddComponent<rtype::server::components::LobbyState>(player, p->isReady, false);
    } else {
        // Update ready state
        std::cout << "Updating existing LobbyState for player " << player << std::endl;
        lobbyState->isReady = p->isReady;
    }
    
    // Get player's room
    ECS::EntityID room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Room not found for player " << player << std::endl;
        return;
    }
    
    std::cout << "Player " << player << " is in room " << room << ", broadcasting lobby state" << std::endl;
    
    // Broadcast updated lobby state to all players in the room
    broadcastLobbyState(room);
}

void room_controller::broadcastLobbyState(ECS::EntityID room) {
    // Get all players in this room
    auto players_in_room = player_service::findPlayersByRoomCode(room);
    
    // Count total players and ready players (only count those not in game yet)
    uint32_t totalPlayers = 0;
    uint32_t readyPlayers = 0;
    
    for (auto pid : players_in_room) {
        auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        
        // If player doesn't have LobbyState, they're in lobby but not ready
        if (!lobbyState) {
            totalPlayers++;
            continue;
        }
        
        // Skip players who have transitioned to game already
        if (lobbyState->isInGame) continue;
        
        totalPlayers++;
        if (lobbyState->isReady) readyPlayers++;
    }
    
    // Build lobby state packet
    LobbyStatePacket pkt{};
    pkt.totalPlayers = totalPlayers;
    pkt.readyPlayers = readyPlayers;
    
    std::cout << "Broadcasting lobby state: " << totalPlayers << " players, " << readyPlayers << " ready" << std::endl;
    
    // Send to all players in the room (including those in lobby)
    for (auto pid : players_in_room) {
        auto* lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        // Only send to players still in lobby (not yet in game)
        if (lobbyState && lobbyState->isInGame) continue;
        
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue;
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), LOBBY_STATE, nullptr, true);
    }
}

void room_controller::handleSpawnBossRequest(const packet_t& packet) {
    std::cout << "=== handleSpawnBossRequest called ===" << std::endl;
    
    // Find the requesting player entity by network address
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found by network address" << std::endl;
        return;
    }
    
    std::cout << "Player " << player << " requesting boss spawn" << std::endl;
    
    // Get player's room
    auto* playerConn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
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
    auto* roomProps = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!roomProps) {
        std::cerr << "ERROR: Room properties not found" << std::endl;
        return;
    }
    
    if (roomProps->ownerId != player) {
        std::cout << "WARNING: Player " << player << " is not admin of room " << room << ", boss spawn request denied" << std::endl;
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
    auto* enemyTypes = root.world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (enemyTypes) {
        for (auto& etPair : *enemyTypes) {
            auto* et = etPair.second.get();
            if (et && et->type == rtype::common::components::EnemyType::Boss) {
                auto* health = root.world.GetComponent<rtype::common::components::Health>(etPair.first);
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
    root.world.AddComponent<rtype::common::components::EnemyTypeComponent>(boss, rtype::common::components::EnemyType::Boss);
    
    std::cout << "SERVER: Admin spawned boss (id=" << boss << ") in room " << room << std::endl;
    
    // Broadcast boss spawn to all players in the room
    SpawnEnemyPacket pkt{};
    pkt.enemyId = static_cast<uint32_t>(boss);
    pkt.enemyType = static_cast<uint16_t>(rtype::common::components::EnemyType::Boss);
    pkt.x = spawnX;
    pkt.y = spawnY;
    pkt.hp = 50;
    
    auto players = root.world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;
    for (auto &pp : *players) {
        ECS::EntityID pid = pp.first;
        auto *pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn || pconn->room_code != room) continue;
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), SPAWN_ENEMY, nullptr, true);
        std::cout << "SERVER: Sent boss spawn packet to player " << pid << std::endl;
    }
}

// Register all packet callbacks on a player's packet handler
void room_controller::registerPlayerCallbacks(PacketHandler& handler) {
    handler.registerCallback(Packets::JOIN_ROOM, handleJoinRoomPacket);
    handler.registerCallback(Packets::GAME_START_REQUEST, handleGameStartRequest);
    handler.registerCallback(Packets::PLAYER_INPUT, handlePlayerInput);
    handler.registerCallback(Packets::PLAYER_READY, handlePlayerReady);
    handler.registerCallback(Packets::PLAYER_SHOOT, handlePlayerShoot);
    handler.registerCallback(Packets::SPAWN_BOSS_REQUEST, handleSpawnBossRequest);
    std::cout << "✓ Registered player callbacks: JOIN_ROOM, GAME_START_REQUEST, PLAYER_INPUT, PLAYER_READY, PLAYER_SHOOT, SPAWN_BOSS_REQUEST" << std::endl;
}


