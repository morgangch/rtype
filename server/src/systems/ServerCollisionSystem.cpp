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
#include <iostream>
#include <cmath>

namespace rtype::server::systems {

void ServerCollisionSystem::Update(ECS::World& world, float deltaTime) {
    std::vector<ECS::EntityID> toDestroy;
    
    // Check all collision types
    checkProjectileVsEnemyCollisions(world, toDestroy);
    
    // Destroy all entities marked for destruction
    for (auto entity : toDestroy) {
        // TODO: Broadcast ENTITY_DESTROY before destroying
        // For now just destroy locally on server
        world.DestroyEntity(entity);
    }
}

void ServerCollisionSystem::checkProjectileVsEnemyCollisions(
    ECS::World& world, 
    std::vector<ECS::EntityID>& toDestroy) {
    
    auto* projectilePositions = world.GetComponents<rtype::common::components::Position>();
    if (!projectilePositions) return;
    
    // Check each projectile against all enemies
    for (const auto& [projEntity, projPosPtr] : *projectilePositions) {
        auto* projTeam = world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(projEntity);
        
        // Skip if not a player projectile
        if (!projTeam || !projData) continue;
        if (projTeam->team != rtype::common::components::TeamType::Player) continue;
        
        // Projectile bounds (simple 10x10 box for now)
        float projX = projPosPtr->x;
        float projY = projPosPtr->y;
        float projW = 20.0f; // Projectile width
        float projH = 10.0f; // Projectile height
        
        // Check against all enemies
        auto* enemyPositions = world.GetComponents<rtype::common::components::Position>();
        if (!enemyPositions) continue;
        
        for (const auto& [enemyEntity, enemyPosPtr] : *enemyPositions) {
            if (enemyEntity == projEntity) continue;
            
            auto* enemyTeam = world.GetComponent<rtype::common::components::Team>(enemyEntity);
            auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemyEntity);
            
            // Check if this is an enemy
            if (!enemyTeam || !enemyHealth) continue;
            if (enemyTeam->team != rtype::common::components::TeamType::Enemy) continue;
            
            // Enemy bounds (assume 33x33 based on typical sprite size)
            float enemyX = enemyPosPtr->x;
            float enemyY = enemyPosPtr->y;
            float enemyW = 33.0f;
            float enemyH = 33.0f;
            
            // Check AABB collision
            if (checkAABB(projX, projY, projW, projH, enemyX, enemyY, enemyW, enemyH)) {
                std::cout << "[ServerCollisionSystem] Projectile " << projEntity << " hit enemy " << enemyEntity << std::endl;
                
                // Damage enemy
                enemyHealth->currentHp -= projData->damage;
                std::cout << "  Enemy health: " << enemyHealth->currentHp << "/" << enemyHealth->maxHp << std::endl;
                
                if (enemyHealth->currentHp <= 0) {
                    std::cout << "  ✓ Enemy destroyed!" << std::endl;
                    enemyHealth->isAlive = false;
                    
                    // Mark enemy for destruction
                    toDestroy.push_back(enemyEntity);
                    
                    // TODO: Broadcast ENTITY_DESTROY to all clients
                    // For now, clients will see enemy disappear when PLAYER_STATE updates
                }
                
                // Destroy projectile if not piercing
                if (!projData->piercing) {
                    std::cout << "  Projectile destroyed (non-piercing)" << std::endl;
                    toDestroy.push_back(projEntity);
                    break; // Projectile destroyed, stop checking
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

void ServerCollisionSystem::broadcastEntityDestroy(
    ECS::World& world, 
    ECS::EntityID entityId, 
    ECS::EntityID roomId) {
    
    // Get all players in the room
    auto players = rtype::server::services::player_service::findPlayersByRoomCode(roomId);
    
    // Create ENTITY_DESTROY packet
    EntityDestroyPacket pkt{};
    pkt.entityId = static_cast<uint32_t>(entityId);
    
    std::cout << "[ServerCollisionSystem] Broadcasting ENTITY_DESTROY for entity " << entityId << " to " << players.size() << " players" << std::endl;
    
    // Send to all players in the room
    for (auto playerId : players) {
        auto* lobbyState = world.GetComponent<rtype::server::components::LobbyState>(playerId);
        if (!lobbyState || !lobbyState->isInGame) continue; // Only send to in-game players
        
        auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(playerId);
        if (!pconn) continue;
        
        pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, false);
        std::cout << "  ✓ Sent ENTITY_DESTROY to player " << playerId << std::endl;
    }
}

} // namespace rtype::server::systems
