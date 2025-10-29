/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Server-side collision detection system implementation
*/

#include "systems/ServerCollisionSystem.h"
#include "components/PlayerConn.h"
#include "components/RoomProperties.h"
#include "components/LobbyState.h"
#include "services/PlayerService.h"
#include "rtype.h"
#include "packetmanager.h"
#include <common/components/EnemyType.h>
#include <common/components/Player.h>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace rtype::server::systems {

void ServerCollisionSystem::Update(ECS::World& world, float deltaTime) {
    std::vector<ECS::EntityID> toDestroy;

    // Check all collision types
    checkProjectileVsEnemyCollisions(world, toDestroy);
    checkEnemyProjectilesVsPlayerCollisions(world, toDestroy);
    checkPlayerVsEnemyCollisions(world);  // Applies damage directly, no destruction

    // Remove duplicates from toDestroy list
    std::sort(toDestroy.begin(), toDestroy.end());
    toDestroy.erase(std::unique(toDestroy.begin(), toDestroy.end()), toDestroy.end());
    
    // Broadcast destruction to all clients before destroying locally
    // We need to find which room these entities belong to
    // For now, broadcast to ALL active game rooms (simpler approach)
    for (auto entity : toDestroy) {
        // Determine what type of entity this is for logging
        auto* projComp = world.GetComponent<rtype::common::components::Projectile>(entity);
        auto* healthComp = world.GetComponent<rtype::common::components::Health>(entity);
        
        if (projComp) {
            std::cout << "[ServerCollisionSystem] Broadcasting destruction of PROJECTILE entity " << entity << " (piercing=" << projComp->piercing << ")" << std::endl;
        } else if (healthComp) {
            std::cout << "[ServerCollisionSystem] Broadcasting destruction of ENEMY entity " << entity << " (hp=" << healthComp->currentHp << ")" << std::endl;
        } else {
            std::cout << "[ServerCollisionSystem] Broadcasting destruction of UNKNOWN entity " << entity << std::endl;
        }
        
        broadcastEntityDestroyToAllRooms(world, entity);
        world.DestroyEntity(entity);
    }
}

void ServerCollisionSystem::checkProjectileVsEnemyCollisions(
    ECS::World& world, 
    std::vector<ECS::EntityID>& toDestroy) {
    
    auto* projectilePositions = world.GetAllComponents<rtype::common::components::Position>();
    if (!projectilePositions) return;
    
    // Check each projectile against all enemies
    for (const auto& [projEntity, projPosPtr] : *projectilePositions) {
        auto* projTeam = world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(projEntity);
        
        // Skip if not a player projectile
        if (!projTeam || !projData) continue;
        if (projTeam->team != rtype::common::components::TeamType::Player) continue;
        
        // CRITICAL FIX: Skip projectiles that just spawned (haven't moved yet)
        // This prevents immediate collision with enemies that are being destroyed in the same frame
        if (projData->distanceTraveled < 1.0f) {
            // Skip silently (no log spam)
            continue;
        }
        
        // Projectile bounds (simple 10x10 box for now)
        float projX = projPosPtr->x;
        float projY = projPosPtr->y;
        float projW = 20.0f; // Projectile width
        float projH = 10.0f; // Projectile height
        
        // Check against all enemies
        auto* enemyPositions = world.GetAllComponents<rtype::common::components::Position>();
        if (!enemyPositions) continue;
        
        for (const auto& [enemyEntity, enemyPosPtr] : *enemyPositions) {
            if (enemyEntity == projEntity) continue;
            
            auto* enemyTeam = world.GetComponent<rtype::common::components::Team>(enemyEntity);
            auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemyEntity);
            
            // Check if this is an enemy
            if (!enemyTeam || !enemyHealth) continue;
            if (enemyTeam->team != rtype::common::components::TeamType::Enemy) continue;
            
            // Enemy bounds - adjust based on enemy type
            float enemyX = enemyPosPtr->x;
            float enemyY = enemyPosPtr->y;
            float enemyW = 33.0f;
            float enemyH = 36.0f;
            
            // Boss has much larger hitbox (5x scale)
            auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(enemyEntity);
            if (enemyType && enemyType->type == rtype::common::components::EnemyType::TankDestroyer) {
                enemyW = 33.0f * 5.0f; // 165 pixels
                enemyH = 36.0f * 5.0f; // 180 pixels
            }
            
            // Check AABB collision
            if (checkAABB(projX, projY, projW, projH, enemyX, enemyY, enemyW, enemyH)) {
                std::cout << "[ServerCollisionSystem] Projectile " << projEntity << " hit enemy " << enemyEntity << std::endl;
                std::cout << "  Projectile pos: (" << projX << "," << projY << ") piercing: " << (projData->piercing ? "YES" : "NO") << std::endl;
                std::cout << "  Enemy pos: (" << enemyX << "," << enemyY << ")" << std::endl;
                
                // Damage enemy
                enemyHealth->currentHp -= projData->damage;
                std::cout << "  Enemy health: " << enemyHealth->currentHp << "/" << enemyHealth->maxHp << std::endl;
                
                if (enemyHealth->currentHp <= 0) {
                    std::cout << "  ✓ Enemy destroyed!" << std::endl;
                    enemyHealth->isAlive = false;
                    toDestroy.push_back(enemyEntity);
                }
                
                // Destroy projectile if not piercing
                if (!projData->piercing) {
                    std::cout << "  → Projectile destroyed (non-piercing)" << std::endl;
                    toDestroy.push_back(projEntity);
                    break; // Projectile destroyed, stop checking
                } else {
                    std::cout << "  → Projectile continues (piercing)" << std::endl;
                    // Piercing projectile continues through enemies
                }
            }
        }
    }
}

bool ServerCollisionSystem::checkAABB(
    float x1, float y1, float w1, float h1,
    float x2, float y2, float w2, float h2) {
    
    // Convert center positions to top-left corners
    float left1 = x1 - w1 / 2.0f;
    float right1 = x1 + w1 / 2.0f;
    float top1 = y1 - h1 / 2.0f;
    float bottom1 = y1 + h1 / 2.0f;
    
    float left2 = x2 - w2 / 2.0f;
    float right2 = x2 + w2 / 2.0f;
    float top2 = y2 - h2 / 2.0f;
    float bottom2 = y2 + h2 / 2.0f;
    
    // Check overlap
    return !(right1 < left2 || right2 < left1 || bottom1 < top2 || bottom2 < top1);
}

void ServerCollisionSystem::broadcastEntityDestroyToAllRooms(
    ECS::World& world,
    ECS::EntityID entityId) {
    
    // Get all rooms
    auto* roomProps = world.GetAllComponents<rtype::server::components::RoomProperties>();
    if (!roomProps) return;
    
    EntityDestroyPacket pkt{};
    pkt.entityId = static_cast<uint32_t>(entityId);
    
    std::cout << "[ServerCollisionSystem] Broadcasting ENTITY_DESTROY for entity " << entityId << std::endl;
    
    // For each room, send to all players in game
    for (const auto& [roomEntity, roomPtr] : *roomProps) {
        if (!roomPtr->isGameStarted) continue; // Skip rooms not in game
        
        // Find all players in this room
        auto players = rtype::server::services::player_service::findPlayersByRoomCode(roomEntity);
        
        for (auto playerId : players) {
            auto* lobbyState = world.GetComponent<rtype::server::components::LobbyState>(playerId);
            if (!lobbyState || !lobbyState->isInGame) continue; // Only send to in-game players
            
            auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(playerId);
            if (!pconn) continue;
            
            pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, false);
        }
        
        std::cout << "  ✓ Sent ENTITY_DESTROY to " << players.size() << " players in room " << roomPtr->joinCode << std::endl;
    }
}

void ServerCollisionSystem::checkEnemyProjectilesVsPlayerCollisions(
    ECS::World& world,
    std::vector<ECS::EntityID>& toDestroy) {

    auto* projectilePositions = world.GetAllComponents<rtype::common::components::Position>();
    if (!projectilePositions) return;

    // Iterate through all projectiles
    for (const auto& [projEntity, projPosPtr] : *projectilePositions) {
        auto* projTeam = world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(projEntity);

        // Skip if not an enemy projectile
        if (!projTeam || !projData) continue;
        if (projTeam->team != rtype::common::components::TeamType::Enemy) continue;

        // Skip projectiles that just spawned
        if (projData->distanceTraveled < 1.0f) continue;

        float projX = projPosPtr->x;
        float projY = projPosPtr->y;
        float projW = 20.0f;
        float projH = 10.0f;

        // Check against all players
        auto* playerComponents = world.GetAllComponents<rtype::common::components::Player>();
        if (!playerComponents) continue;

        for (const auto& [playerEntity, playerPtr] : *playerComponents) {
            auto* playerPos = world.GetComponent<rtype::common::components::Position>(playerEntity);
            auto* playerHealth = world.GetComponent<rtype::common::components::Health>(playerEntity);

            if (!playerPos || !playerHealth) continue;
            if (playerHealth->invulnerable) continue;  // Skip invulnerable players

            float playerX = playerPos->x;
            float playerY = playerPos->y;
            float playerW = 33.0f;  // Player hitbox
            float playerH = 17.0f;

            // Check AABB collision
            if (checkAABB(projX, projY, projW, projH, playerX, playerY, playerW, playerH)) {
                std::cout << "[ServerCollisionSystem] Enemy projectile " << projEntity << " hit player " << playerEntity << std::endl;

                // Damage player
                playerHealth->currentHp -= projData->damage;
                std::cout << "  Player health: " << playerHealth->currentHp << "/" << playerHealth->maxHp << std::endl;

                // Grant invulnerability frames
                playerHealth->invulnerable = true;
                playerHealth->invulnerabilityTimer = 1.0f;  // 1 second invulnerability

                if (playerHealth->currentHp <= 0) {
                    std::cout << "  ✗ Player died!" << std::endl;
                    playerHealth->isAlive = false;
                    // TODO: Handle player death (game over, respawn, etc.)
                }

                // Destroy projectile
                toDestroy.push_back(projEntity);
                break;  // Projectile destroyed, stop checking
            }
        }
    }
}

void ServerCollisionSystem::checkPlayerVsEnemyCollisions(ECS::World& world) {
    auto* playerComponents = world.GetAllComponents<rtype::common::components::Player>();
    if (!playerComponents) return;

    for (const auto& [playerEntity, playerPtr] : *playerComponents) {
        auto* playerPos = world.GetComponent<rtype::common::components::Position>(playerEntity);
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(playerEntity);

        if (!playerPos || !playerHealth) continue;
        if (playerHealth->invulnerable) continue;  // Skip invulnerable players

        float playerX = playerPos->x;
        float playerY = playerPos->y;
        float playerW = 33.0f;
        float playerH = 17.0f;

        // Check against all enemies
        auto* enemyPositions = world.GetAllComponents<rtype::common::components::Position>();
        if (!enemyPositions) continue;

        for (const auto& [enemyEntity, enemyPosPtr] : *enemyPositions) {
            if (enemyEntity == playerEntity) continue;

            auto* enemyTeam = world.GetComponent<rtype::common::components::Team>(enemyEntity);
            auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemyEntity);

            // Check if this is an enemy
            if (!enemyTeam || !enemyHealth) continue;
            if (enemyTeam->team != rtype::common::components::TeamType::Enemy) continue;

            float enemyX = enemyPosPtr->x;
            float enemyY = enemyPosPtr->y;
            float enemyW = 33.0f;
            float enemyH = 36.0f;

            // Boss has larger hitbox
            auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(enemyEntity);
            if (enemyType && enemyType->type == rtype::common::components::EnemyType::TankDestroyer) {
                enemyW = 33.0f * 5.0f;
                enemyH = 36.0f * 5.0f;
            }

            // Check AABB collision
            if (checkAABB(playerX, playerY, playerW, playerH, enemyX, enemyY, enemyW, enemyH)) {
                std::cout << "[ServerCollisionSystem] Player " << playerEntity << " collided with enemy " << enemyEntity << std::endl;

                // Damage player (contact damage = 1)
                playerHealth->currentHp -= 1;
                std::cout << "  Player health: " << playerHealth->currentHp << "/" << playerHealth->maxHp << std::endl;

                // Grant invulnerability frames
                playerHealth->invulnerable = true;
                playerHealth->invulnerabilityTimer = 1.0f;

                if (playerHealth->currentHp <= 0) {
                    std::cout << "  ✗ Player died!" << std::endl;
                    playerHealth->isAlive = false;
                    // TODO: Handle player death
                }

                // Enemy doesn't get destroyed (player just takes damage)
                break;  // Player hit, stop checking other enemies this frame
            }
        }
    }
}

} // namespace rtype::server::systems
