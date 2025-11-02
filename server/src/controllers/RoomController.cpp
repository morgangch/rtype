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
#include "systems/ServerEnemySystem.h"
#include "ECS/Types.h"
#include "services/PlayerService.h"
#include "services/RoomService.h"
#include <common/components/Player.h>
#include <common/components/Velocity.h>
#include <common/components/Position.h>
#include <common/components/Team.h>
#include <common/components/Projectile.h>
#include <common/components/Health.h>
#include <common/components/Score.h>
#include <common/components/EnemyType.h>
#include <common/components/VesselClass.h>
#include <common/components/Explosion.h>
#include <common/components/Homing.h>
#include <common/components/Shield.h>
#include <common/components/ForgeAugment.h>
#include <common/components/Bounce.h>
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
#include "components/Assistant.h"
#include "systems/ServerEnemySystem.h"

using namespace rtype::server::controllers;
using namespace rtype::server::services;

// ============================================================================
// HELPER FUNCTIONS - Game Start Flow
// ============================================================================

// Forge helper: spawn extra projectiles based on player's unlocked augment
static void spawnForgeExtras(ECS::EntityID room, ECS::EntityID owner, float x, float y) {
    auto* forge = root.world.GetComponent<rtype::common::components::ForgeAugment>(owner);
    if (!forge || !forge->unlocked || forge->type == rtype::common::components::ForgeAugmentType::None) return;

    using FA = rtype::common::components::ForgeAugmentType;

    switch (forge->type) {
        case FA::DualLaser: {
            // Crimson Striker: Dual laser beams (≈1.5 -> rounded to 2 dmg), straight and fast
            const float speed = 700.f;
            const int dmg = 2;
            const float offY = 6.f;
            auto p1 = room_controller::createSingleProjectile(room, owner, x + 32.f, y - offY, speed, dmg, false, false);
            auto p2 = room_controller::createSingleProjectile(room, owner, x + 32.f, y + offY, speed, dmg, false, false);
            room_controller::broadcastProjectileSpawn(p1, owner, room, false);
            room_controller::broadcastProjectileSpawn(p2, owner, room, false);
            break;
        }
        case FA::BouncySplit: {
            // Azure Phantom: two angled bouncing shots, 1 dmg each, bounce on top/bottom borders
            const float speed = 520.f;
            const float angle = 10.f * 3.1415926f / 180.f;
            float vx = speed * std::cos(0.f);
            float vyUp = speed * std::sin(angle);
            float vyDown = -vyUp;

            auto pUp = room_controller::createSingleProjectile(room, owner, x + 32.f, y, vx, vyUp, speed, 1, false, false);
            auto pDn = room_controller::createSingleProjectile(room, owner, x + 32.f, y, vx, vyDown, speed, 1, false, false);
            // Add Bounce (Y only)
            root.world.AddComponent<rtype::common::components::Bounce>(pUp, 6, false, true);
            root.world.AddComponent<rtype::common::components::Bounce>(pDn, 6, false, true);
            room_controller::broadcastProjectileSpawn(pUp, owner, room, false);
            room_controller::broadcastProjectileSpawn(pDn, owner, room, false);
            break;
        }
        case FA::ShortSpread: {
            // Shotgun: 5 pellets in a cone, short lifetime (~0.5s)
            const int count = 5;
            const float angleSpread = 30.f;
            const float angleStep = angleSpread / (count - 1);
            const float startDeg = -angleSpread / 2.f;
            const float baseSpeed = 600.f;
            const int dmg = 1; // 0.5 -> rounded to 1
            for (int i = 0; i < count; ++i) {
                float deg = startDeg + i * angleStep;
                float rad = deg * 3.1415926f / 180.f;
                float vx = baseSpeed * std::cos(rad);
                float vy = baseSpeed * std::sin(rad);
                auto p = room_controller::createSingleProjectile(room, owner, x + 32.f, y, vx, vy, baseSpeed, dmg, false, false);
                if (auto* pr = root.world.GetComponent<rtype::common::components::Projectile>(p)) {
                    pr->maxDistance = baseSpeed * 0.5f; // ~0.5s travel
                }
                room_controller::broadcastProjectileSpawn(p, owner, room, false);
            }
            break;
        }
        case FA::GuardianTriBeam: {
            // Solar Guardian: triple forward beams, short to mid range
            const float speed = 560.f;
            const int dmg = 1;
            const float offsets[3] = {-8.f, 0.f, 8.f};
            for (float offY : offsets) {
                auto p = room_controller::createSingleProjectile(room, owner, x + 32.f, y + offY, speed, dmg, false, false);
                if (auto* pr = root.world.GetComponent<rtype::common::components::Projectile>(p)) {
                    pr->maxDistance = 380.f; // moderate range
                }
                room_controller::broadcastProjectileSpawn(p, owner, room, false);
            }
            break;
        }
        default: break;
    }
}

void room_controller::markPlayersAsInGame(ECS::EntityID room) {
    auto players_in_room = player_service::findPlayersByRoom(room);
    std::cout << "Found " << players_in_room.size() << " players in room" << std::endl;

    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);

    for (auto pid: players_in_room) {
        auto *lobbyState = root.world.GetComponent<rtype::server::components::LobbyState>(pid);
        if (lobbyState) {
            lobbyState->isInGame = true;
            std::cout << "  - Marked player " << pid << " as in-game" << std::endl;
        }

        // Reset player HP for the game start based on vessel class
        auto *health = root.world.GetComponent<rtype::common::components::Health>(pid);
        auto *vesselClass = root.world.GetComponent<rtype::common::components::VesselClass>(pid);
        if (health && vesselClass) {
            // Base HP per vessel type (hearts in UI)
            int baseHp = 3; // Default
            switch (vesselClass->type) {
                case rtype::common::components::VesselType::CrimsonStriker:
                    baseHp = 3; // Balanced
                    break;
                case rtype::common::components::VesselType::AzurePhantom:
                    baseHp = 3; // Agile (low defense)
                    break;
                case rtype::common::components::VesselType::EmeraldTitan:
                    baseHp = 4; // Tank (4 HP, 4 hearts)
                    break;
                case rtype::common::components::VesselType::SolarGuardian:
                    baseHp = 5; // Defense (5 HP, 5 hearts)
                    break;
            }
            
            health->currentHp = baseHp;
            health->maxHp = baseHp;
            health->isAlive = true;
            health->invulnerable = false;
            health->invulnerabilityTimer = 0.0f;
            std::cout << "  - Reset player " << pid << " HP to " << baseHp << " (Vessel: " << static_cast<int>(vesselClass->type) << ")" << std::endl;
        }

        // Reset player score to 0 for the game start
        auto *score = root.world.GetComponent<rtype::common::components::Score>(pid);
        if (score) {
            score->points = 0;
            score->kills = 0;
            score->deaths = 0;
            score->combo = 0;
            score->comboTimer = 0.0f;
            score->highestCombo = 0;
            std::cout << "  - Reset player " << pid << " score to 0" << std::endl;
        }

        // Broadcast initial player state so clients see the correct HP immediately
        auto *pos = root.world.GetComponent<rtype::common::components::Position>(pid);
        if (pos && health) {
            // Send to all players in the same room
            for (auto recipientPid: players_in_room) {
                network::senders::send_player_state(recipientPid, pid, pos->x, pos->y, pos->rotation,
                                                    static_cast<uint16_t>(health->currentHp), health->isAlive);
            }
        }
    }
}

void room_controller::broadcastGameStart(ECS::EntityID room) {
    network::senders::broadcast_game_start(room);
}

void room_controller::broadcastPlayerRoster(ECS::EntityID room) {
    auto players_in_room = player_service::findPlayersByRoom(room);

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

    // Player doesn't exist, create new one with selected vessel type (from JoinRoomPacket)
    const JoinRoomPacket *jp = reinterpret_cast<const JoinRoomPacket *>(packet.data);
    auto vesselType = static_cast<rtype::common::components::VesselType>(jp ? jp->vesselType : 0);
    player = player_service::createNewPlayer(playerName, joinCode, ipStr, port, vesselType);

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

    // Get player's vessel type
    uint8_t vesselType = 0; // Default: CrimsonStriker
    auto *vesselClass = root.world.GetComponent<rtype::common::components::VesselClass>(player);
    if (vesselClass) {
        vesselType = static_cast<uint8_t>(vesselClass->type);
    }

    std::cout << "Sending JOIN_ROOM_ACCEPTED to player " << player << " (isAdmin=" <<
            (rp->ownerId == player) << ", roomCode=" << rp->joinCode << ", playerServerId=" << player << 
            ", vesselType=" << static_cast<int>(vesselType) << ")" << std::endl;
    network::senders::send_join_room_accepted(player, (rp->ownerId == player), rp->joinCode,
                                              static_cast<uint32_t>(player), vesselType);
}

void room_controller::notifyJoiningPlayerOfExisting(ECS::EntityID player, ECS::EntityID room) {
    auto *playernet = root.world.GetComponent<components::PlayerConn>(player);
    if (!playernet) return;

    auto players_in_room = player_service::findPlayersByRoom(room);

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

ECS::EntityID room_controller::createServerProjectile(ECS::EntityID room, ECS::EntityID owner, float x, float y, bool isCharged) {
    // Get vessel class to determine weapon behavior
    auto *vesselClass = root.world.GetComponent<rtype::common::components::VesselClass>(owner);
    if (!vesselClass) {
        // Fallback to default single projectile if no vessel class
        std::cerr << "WARNING: Player " << owner << " has no VesselClass component, using default weapon" << std::endl;
        vesselClass = new rtype::common::components::VesselClass(); // Default CrimsonStriker
    }
    
    // Get weapon mode based on charged state
    rtype::common::components::WeaponMode weaponMode = isCharged ? 
        vesselClass->chargedWeaponMode : vesselClass->normalWeaponMode;
    
    // Base damage from vessel class
    uint16_t baseDamage = isCharged ? 
        vesselClass->chargedShotDamage : vesselClass->normalShotDamage;
    
    // Apply vessel damage multiplier
    baseDamage = vesselClass->getEffectiveDamage(baseDamage);
    
    // Apply mega damage if enabled
    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (rp && rp->ownerId == owner && rp->megaDamageEnabled) {
        baseDamage = 1000;
    }
    
    bool piercing = isCharged && vesselClass->chargedShotPiercing;
    float baseSpeed = isCharged ? 600.0f : 500.0f;
    float projectileX = x + 32.0f; // Offset to spawn in front of player
    
    // Create projectile(s) based on weapon mode
    ECS::EntityID firstProjectile = 0;
    
    switch (weaponMode) {
        case rtype::common::components::WeaponMode::Single: {
            // Solar Guardian charged: Solar Pulse (radial burst)
            if (vesselClass->type == rtype::common::components::VesselType::SolarGuardian && isCharged) {
                const int count = 12;
                const float baseSpeed = 480.0f;
                const int dmg = 1;
                const float step = 360.0f / count;
                ECS::EntityID first = 0;
                for (int i = 0; i < count; ++i) {
                    float deg = i * step;
                    float rad = deg * 3.1415926f / 180.f;
                    float vx = baseSpeed * std::cos(rad);
                    float vy = baseSpeed * std::sin(rad);
                    auto proj = createSingleProjectile(room, owner, x, y, vx, vy, baseSpeed, dmg, false, true);
                    if (auto* pr = root.world.GetComponent<rtype::common::components::Projectile>(proj)) {
                        pr->maxDistance = 260.0f; // short-lived pulse
                    }
                    broadcastProjectileSpawn(proj, owner, room, true);
                    if (i == 0) first = proj;
                }
                return first; // Done
            }

            // Normal single projectile (CrimsonStriker normal, EmeraldTitan)
            firstProjectile = createSingleProjectile(room, owner, projectileX, y, baseSpeed, baseDamage, piercing, isCharged);
            broadcastProjectileSpawn(firstProjectile, owner, room, isCharged);
            
            // EmeraldTitan: Add explosion component
            if (vesselClass->type == rtype::common::components::VesselType::EmeraldTitan) {
                float radius = isCharged ? 150.0f : 80.0f;  // Larger for charged
                root.world.AddComponent<rtype::common::components::Explosion>(firstProjectile, radius, baseDamage, baseDamage / 2);
            }
            break;
        }
        
        case rtype::common::components::WeaponMode::Dual: {
            // Dual projectiles (AzurePhantom normal) - shoot from top and bottom of ship
            float offset = 10.0f;
            // Keep full damage for each projectile (balanced by lower fire rate)
            auto proj1 = createSingleProjectile(room, owner, projectileX, y - offset, baseSpeed, baseDamage, piercing, isCharged);
            auto proj2 = createSingleProjectile(room, owner, projectileX, y + offset, baseSpeed, baseDamage, piercing, isCharged);
            broadcastProjectileSpawn(proj1, owner, room, isCharged);
            broadcastProjectileSpawn(proj2, owner, room, isCharged);
            firstProjectile = proj1;
            break;
        }
        
        case rtype::common::components::WeaponMode::Burst: {
            // Burst/Homing projectiles (AzurePhantom charged) - 3 homing darts
            int count = vesselClass->projectileCount;
            float angleStep = 15.0f; // 15 degrees between each dart
            float startAngle = -(angleStep * (count - 1)) / 2.0f;
            
            for (int i = 0; i < count; ++i) {
                float angle = startAngle + (i * angleStep);
                float radians = angle * 3.14159f / 180.0f;
                float vx = baseSpeed * std::cos(radians);
                float vy = baseSpeed * std::sin(radians);
                
                auto proj = root.world.CreateEntity();
                root.world.AddComponent<rtype::common::components::Position>(proj, projectileX, y, 0.0f);
                root.world.AddComponent<rtype::common::components::Velocity>(proj, vx, vy, baseSpeed);
                root.world.AddComponent<rtype::common::components::Team>(proj, rtype::common::components::TeamType::Player);
                root.world.AddComponent<rtype::common::components::Projectile>(proj, baseDamage, piercing, true, baseSpeed);
                auto *projComp = root.world.GetComponent<rtype::common::components::Projectile>(proj);
                if (projComp) projComp->ownerId = owner;
                root.world.AddComponent<rtype::server::components::LinkedRoom>(proj, room);
                
                // Add homing component
                root.world.AddComponent<rtype::common::components::Homing>(proj, 0, 800.0f, 180.0f, 450.0f);
                
                // Broadcast each projectile
                broadcastProjectileSpawn(proj, owner, room, isCharged);
                
                if (i == 0) firstProjectile = proj;
            }
            break;
        }
        
        case rtype::common::components::WeaponMode::Spread: {
            // Spread/Shotgun (SolarGuardian normal) - 4 pellets in a cone
            int count = vesselClass->projectileCount;
            float angleSpread = 30.0f; // Total spread angle
            float angleStep = angleSpread / (count - 1);
            float startAngle = -angleSpread / 2.0f;
            
            for (int i = 0; i < count; ++i) {
                float angle = startAngle + (i * angleStep);
                float radians = angle * 3.14159f / 180.0f;
                float vx = baseSpeed * std::cos(radians);
                float vy = baseSpeed * std::sin(radians);
                
                auto proj = createSingleProjectile(room, owner, projectileX, y, vx, vy, baseSpeed, baseDamage, piercing, isCharged);
                broadcastProjectileSpawn(proj, owner, room, isCharged);
                if (i == 0) firstProjectile = proj;
            }
            break;
        }
        
        case rtype::common::components::WeaponMode::Piercing: {
            // Piercing beam (CrimsonStriker charged)
            firstProjectile = createSingleProjectile(room, owner, projectileX, y, baseSpeed, baseDamage, true, isCharged);
            broadcastProjectileSpawn(firstProjectile, owner, room, isCharged);
            break;
        }
    }
    
    // After base projectile(s), append forge extras if unlocked
    spawnForgeExtras(room, owner, x, y);
    return firstProjectile;
}

// Helper function to create a single projectile
ECS::EntityID room_controller::createSingleProjectile(ECS::EntityID room, ECS::EntityID owner, 
                                                       float x, float y, float speed, 
                                                       uint16_t damage, bool piercing, bool isCharged) {
    return createSingleProjectile(room, owner, x, y, speed, 0.0f, speed, damage, piercing, isCharged);
}

ECS::EntityID room_controller::createSingleProjectile(ECS::EntityID room, ECS::EntityID owner, 
                                                       float x, float y, float vx, float vy, float speed,
                                                       uint16_t damage, bool piercing, bool isCharged) {
    auto projectile = root.world.CreateEntity();
    
    root.world.AddComponent<rtype::common::components::Position>(projectile, x, y, 0.0f);
    root.world.AddComponent<rtype::common::components::Velocity>(projectile, vx, vy, speed);
    root.world.AddComponent<rtype::common::components::Team>(projectile, rtype::common::components::TeamType::Player);
    root.world.AddComponent<rtype::common::components::Projectile>(projectile, damage, piercing, true, speed);
    
    auto *projComp = root.world.GetComponent<rtype::common::components::Projectile>(projectile);
    if (projComp) projComp->ownerId = owner;
    
    root.world.AddComponent<rtype::server::components::LinkedRoom>(projectile, room);
    
    return projectile;
}

ECS::EntityID room_controller::createEnemyProjectile(ECS::EntityID room, float x, float y, float vx, float vy, ECS::World& world) {
    auto projectile = world.CreateEntity();

    // Position
    world.AddComponent<rtype::common::components::Position>(projectile, x, y, 0.0f);

    // Velocity - custom direction
    float speed = std::sqrt(vx * vx + vy * vy);
    world.AddComponent<rtype::common::components::Velocity>(projectile, vx, vy, speed);

    // Team - Enemy
    world.AddComponent<rtype::common::components::Team>(projectile, rtype::common::components::TeamType::Enemy);

    // Projectile - 1 damage, non-piercing
    world.AddComponent<rtype::common::components::Projectile>(projectile, 1, false, false);

    // Link to room
    world.AddComponent<rtype::server::components::LinkedRoom>(projectile, room);

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
    
    // Load tile-driven map for enemy spawning
    auto* enemySystem = root.world.GetSystem<ServerEnemySystem>();
    if (enemySystem) {
        // Load the comprehensive R-Type mission map with all enemy types
        if (enemySystem->loadMap("assets/maps/rtype-mission")) {
            std::cout << "✓ SERVER: Loaded tile-driven map 'rtype-mission' for room " << rp->joinCode << std::endl;
        } else {
            std::cerr << "⚠ SERVER: Failed to load tile-driven map 'rtype-mission'" << std::endl;
            // Try fallback to space-corridor
            if (enemySystem->loadMap("assets/maps/space-corridor")) {
                std::cout << "✓ SERVER: Loaded fallback map 'space-corridor' for room " << rp->joinCode << std::endl;
            } else {
                std::cerr << "✗ SERVER: All map loading failed - enemies will not spawn!" << std::endl;
            }
        }
    }
    

    // Use extracted helper functions
    markPlayersAsInGame(room);
    broadcastGameStart(room);
    broadcastPlayerRoster(room);

    // Spawn an AI assistant only if there is exactly 1 human player in the room and AI assist is enabled
    {
        auto players_in_room = player_service::findPlayersByRoom(room);
        if (players_in_room.size() == 1 && rp && rp->aiAssistEnabled) {
            // Create assistant entity (server-controlled, not network-connected)
            auto assistant = root.world.CreateEntity();
            // Place assistant near the left side of the screen, centered vertically
            root.world.AddComponent<rtype::common::components::Position>(assistant, 120.0f, 360.0f, 0.0f);
            root.world.AddComponent<rtype::common::components::Velocity>(assistant, 0.0f, 0.0f, 200.0f);
            root.world.AddComponent<rtype::common::components::Health>(assistant, 5);
            root.world.AddComponent<rtype::common::components::Team>(assistant, rtype::common::components::TeamType::Player);
            root.world.AddComponent<rtype::common::components::Player>(assistant, std::string("AI_ASSIST"), assistant);
            // Assistant is a server-controlled entity and should NOT have a real PlayerConn
            // That would cause networking code to attempt to send packets to an invalid address
            // Instead we only attach a LinkedRoom so broadcasting systems can find it as a source
            root.world.AddComponent<rtype::server::components::Assistant>(assistant, 0.0f);
            root.world.AddComponent<rtype::server::components::LinkedRoom>(assistant, room);

            std::cout << "SERVER: Spawned AI assistant " << assistant << " in room " << room << std::endl;

            // Notify existing players in the room about the new assistant so clients create a remote player entity
            notifyExistingPlayersOfNewJoin(assistant, std::string("AI_ASSIST"), room);
        }
    }
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
    ECS::EntityID projectile = createServerProjectile(room, player, playerX, playerY, isCharged);
    std::cout << "Player " << player << " shot a "
              << (isCharged ? "CHARGED" : "regular") << " projectile from position ("
              << playerX << ", " << playerY << ")" << std::endl;
    // Note: broadcastProjectileSpawn is now called inside createServerProjectile for each projectile created
}

void room_controller::handleLobbySettingsUpdate(const packet_t &packet) {
    LobbySettingsUpdatePacket *p = (LobbySettingsUpdatePacket *) packet.data;

    // Identify the player and room
    ECS::EntityID player = player_service::findPlayerByNetwork(packet.header.client_addr, packet.header.client_port);
    if (!player) {
        std::cerr << "ERROR: Player not found for settings update" << std::endl;
        return;
    }

    ECS::EntityID room = room_service::getRoomByPlayer(player);
    if (!room) {
        std::cerr << "ERROR: Player " << player << " not in a room for settings update" << std::endl;
        return;
    }

    auto *rp = root.world.GetComponent<rtype::server::components::RoomProperties>(room);
    if (!rp) {
        std::cerr << "ERROR: Room properties not found for settings update" << std::endl;
        return;
    }

    // Only the room owner may update settings
    if (rp->ownerId != player) {
        std::cout << "WARNING: Non-admin player " << player << " attempted to update lobby settings in room " << room << std::endl;
        return;
    }

    // Apply settings
    rp->difficultyIndex = p->difficulty;
    rp->friendlyFire = p->friendlyFire;
    rp->aiAssistEnabled = p->aiAssist;
    rp->megaDamageEnabled = p->megaDamage;
    rp->startLevelIndex = p->startLevel;

    std::cout << "✓ Updated lobby settings for room " << room
              << " [diff=" << static_cast<int>(rp->difficultyIndex)
              << ", FF=" << (rp->friendlyFire ? "on" : "off")
              << ", AI=" << (rp->aiAssistEnabled ? "on" : "off")
              << ", MEGA=" << (rp->megaDamageEnabled ? "on" : "off")
              << ", START_LVL=L" << static_cast<int>(rp->startLevelIndex + 1)
              << "]" << std::endl;
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
    auto players_in_room = player_service::findPlayersByRoom(room);

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

    // Check if a boss already exists (any boss type)
    bool bossExists = false;
    auto *enemyTypes = root.world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (enemyTypes) {
        for (auto &etPair: *enemyTypes) {
            auto *et = etPair.second.get();
            if (!et) continue;
            using ET = rtype::common::components::EnemyType;
            if (et->type == ET::TankDestroyer || et->type == ET::Serpent || et->type == ET::Fortress || et->type == ET::Core) {
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

    // TODO: Tile-driven system - Boss spawning needs to be implemented from tiles
    std::cout << "INFO: Boss spawning currently disabled - will be tile-driven in future" << std::endl;
}

// Register all packet callbacks on a player's packet handler
void room_controller::registerPlayerCallbacks(PacketHandler &handler) {
    handler.registerCallback(Packets::JOIN_ROOM, handleJoinRoomPacket);
    handler.registerCallback(Packets::GAME_START_REQUEST, handleGameStartRequest);
    handler.registerCallback(Packets::PLAYER_INPUT, handlePlayerInput);
    handler.registerCallback(Packets::PLAYER_READY, handlePlayerReady);
    handler.registerCallback(Packets::PLAYER_SHOOT, handlePlayerShoot);
    handler.registerCallback(Packets::SPAWN_BOSS_REQUEST, handleSpawnBossRequest);
    handler.registerCallback(Packets::LOBBY_SETTINGS_UPDATE, handleLobbySettingsUpdate);
    std::cout <<
            "✓ Registered player callbacks: JOIN_ROOM, GAME_START_REQUEST, PLAYER_INPUT, PLAYER_READY, PLAYER_SHOOT, SPAWN_BOSS_REQUEST, LOBBY_SETTINGS_UPDATE"
            << std::endl;
}
