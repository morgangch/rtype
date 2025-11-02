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
#include <common/systems/CollisionSystem.h>
#include <common/components/Score.h>
#include <common/components/Explosion.h>
#include <common/components/Shield.h>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "senders.h"
#include "components/LinkedRoom.h"

namespace rtype::server::systems {

// Helper function to get score points for enemy type
static int getScoreForEnemyType(rtype::common::components::EnemyType type) {
    switch (type) {
        // Basic enemies
        case rtype::common::components::EnemyType::Basic:
            return 10;
        case rtype::common::components::EnemyType::Snake:
            return 15;
        case rtype::common::components::EnemyType::Suicide:
            return 20;
        case rtype::common::components::EnemyType::Pata:
            return 25;

        // Advanced enemies
        case rtype::common::components::EnemyType::Shielded:
            return 50;
        case rtype::common::components::EnemyType::Flanker:
            return 40;
        case rtype::common::components::EnemyType::Bomber:
            return 45;
        case rtype::common::components::EnemyType::Waver:
            return 55;

        // Boss enemies
        case rtype::common::components::EnemyType::TankDestroyer:
            return 500;
        case rtype::common::components::EnemyType::Serpent:
            return 750;
        case rtype::common::components::EnemyType::Fortress:
            return 1000;
        case rtype::common::components::EnemyType::Core:
            return 2000;

        default:
            return 10;
    }
}

void ServerCollisionSystem::Update(ECS::World& world, float deltaTime) {
    rtype::common::systems::CollisionHandlers handlers;
    std::vector<ECS::EntityID> toDestroy;

    handlers.onPlayerVsEnemy = [this, &toDestroy](ECS::EntityID player, ECS::EntityID enemy, ECS::World& world) {
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemy);
        if (!playerHealth || !enemyHealth) return;

        if (playerHealth->invulnerable || !playerHealth->isAlive || playerHealth->currentHp <= 0) return;

        // Check if player has active shield - no damage if shield is active
        auto* shield = world.GetComponent<rtype::common::components::Shield>(player);
        if (shield && shield->isActive) {
            std::cout << "[COLLISION] Player " << player << " hit enemy " << enemy << " - BLOCKED by shield!" << std::endl;
            return;
        }

        std::cout << "[COLLISION] Player " << player << " hit enemy " << enemy << " - HP: " << playerHealth->currentHp << " -> " << (playerHealth->currentHp - 1) << std::endl;

        playerHealth->currentHp -= 1;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        if (playerHealth->currentHp <= 0) {
            playerHealth->isAlive = false;
            std::cout << "[COLLISION] Player " << player << " DIED from enemy contact" << std::endl;
        }

        broadcastPlayerStateImmediate(world, player);
    };

    handlers.onPlayerProjectileVsEnemy = [this, &toDestroy](ECS::EntityID proj, ECS::EntityID enemy, ECS::World& world) {
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemy);
        if (!projData || !enemyHealth) return;

        std::cout << "[COLLISION] Projectile " << proj << " (damage=" << projData->damage << ") hit enemy " << enemy 
                  << " - Enemy HP: " << enemyHealth->currentHp << " -> " << (enemyHealth->currentHp - projData->damage) << std::endl;

        enemyHealth->currentHp -= projData->damage;

        // Check if projectile has explosion component (EmeraldTitan weapons)
        auto* explosion = world.GetComponent<rtype::common::components::Explosion>(proj);
        if (explosion && !explosion->triggered) {
            explosion->triggered = true;
            
            // Get projectile position for AoE center
            auto* projPos = world.GetComponent<rtype::common::components::Position>(proj);
            if (projPos) {
                std::cout << "[EXPLOSION] Projectile " << proj << " exploded at (" << projPos->x << ", " << projPos->y << ") - Radius: " << explosion->radius << std::endl;
                
                // Apply AoE damage to all enemies in range
                auto* allEnemyHealth = world.GetAllComponents<rtype::common::components::Health>();
                if (allEnemyHealth) {
                    for (auto& [targetEntity, targetHealth] : *allEnemyHealth) {
                        if (targetEntity == enemy) continue; // Already damaged this one
                        
                        // Only affect enemies
                        auto* targetTeam = world.GetComponent<rtype::common::components::Team>(targetEntity);
                        if (!targetTeam || targetTeam->team != rtype::common::components::TeamType::Enemy) continue;
                        
                        auto* targetPos = world.GetComponent<rtype::common::components::Position>(targetEntity);
                        if (!targetPos) continue;
                        
                        // Calculate distance from explosion center
                        float dx = targetPos->x - projPos->x;
                        float dy = targetPos->y - projPos->y;
                        float distance = std::sqrt(dx * dx + dy * dy);
                        
                        if (distance <= explosion->radius) {
                            // Linear interpolation between center and edge damage
                            float ratio = distance / explosion->radius;
                            int aoeDamage = explosion->centerDamage + 
                                          static_cast<int>(ratio * (explosion->edgeDamage - explosion->centerDamage));
                            
                            targetHealth->currentHp -= aoeDamage;
                            std::cout << "[EXPLOSION] Hit enemy " << targetEntity << " (dist: " << distance << ") for " << aoeDamage << " damage" << std::endl;
                            
                            if (targetHealth->currentHp <= 0) {
                                targetHealth->isAlive = false;
                                toDestroy.push_back(targetEntity);
                                
                                // Award points to shooter
                                ECS::EntityID shooter = projData->ownerId;
                                auto* shooterPlayer = world.GetComponent<rtype::common::components::Player>(shooter);
                                auto* shooterScore = world.GetComponent<rtype::common::components::Score>(shooter);
                                if (shooterPlayer && shooterScore) {
                                    auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(targetEntity);
                                    int points = enemyType ? getScoreForEnemyType(enemyType->type) : 10;
                                    shooterScore->points += points;
                                    shooterScore->kills++;
                                    network::senders::send_player_score(shooter, shooterPlayer->serverId, shooterScore->points);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (enemyHealth->currentHp <= 0) {
            enemyHealth->isAlive = false;
            std::cout << "[COLLISION] Enemy " << enemy << " DESTROYED" << std::endl;

            // Award score to the player who shot the projectile
            ECS::EntityID shooter = projData->ownerId;
            auto* shooterPlayer = world.GetComponent<rtype::common::components::Player>(shooter);
            auto* shooterScore = world.GetComponent<rtype::common::components::Score>(shooter);

            if (shooterPlayer && shooterScore) {
                // Get enemy type to determine points
                auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(enemy);
                int points = enemyType ? getScoreForEnemyType(enemyType->type) : 10;

                shooterScore->points += points;
                shooterScore->kills++;

                std::cout << "[SCORE] Player " << shooter << " earned " << points << " points (Total: " << shooterScore->points << ")" << std::endl;

                // Send score update to the player
                network::senders::send_player_score(shooter, shooterPlayer->serverId, shooterScore->points);
            }

            toDestroy.push_back(enemy);
        }

        if (!projData->piercing) {
            toDestroy.push_back(proj);
        }
    };

    handlers.onEnemyProjectileVsPlayer = [this, &toDestroy](ECS::EntityID proj, ECS::EntityID player, ECS::World& world) {
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        if (!projData || !playerHealth) return;

        // Check if player has active shield - no damage if shield is active
        auto* shield = world.GetComponent<rtype::common::components::Shield>(player);
        if (shield && shield->isActive) {
            std::cout << "[COLLISION] Enemy projectile " << proj << " hit player " << player << " - BLOCKED by shield!" << std::endl;
            toDestroy.push_back(proj); // Destroy projectile but no damage
            return;
        }

        std::cout << "[COLLISION] Enemy projectile " << proj << " hit player " << player << " - Player HP: " << playerHealth->currentHp << " -> " << (playerHealth->currentHp - projData->damage) << std::endl;

        playerHealth->currentHp -= projData->damage;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        if (playerHealth->currentHp <= 0) {
            playerHealth->isAlive = false;
            std::cout << "[COLLISION] Player " << player << " DIED from enemy projectile" << std::endl;
        }

        broadcastPlayerStateImmediate(world, player);
        toDestroy.push_back(proj);
    };

    handlers.onSuicideExplosion = [this, &toDestroy](ECS::EntityID suicideEnemy, ECS::World& world) {
        auto* enemyPos = world.GetComponent<rtype::common::components::Position>(suicideEnemy);
        if (!enemyPos) return;

        const float EXPLOSION_RADIUS = 100.0f;
        const int EXPLOSION_DAMAGE = 2;

        auto* players = world.GetAllComponents<rtype::common::components::Player>();
        if (players) {
            for (auto& [playerEntity, playerPtr] : *players) {
                auto* playerPos = world.GetComponent<rtype::common::components::Position>(playerEntity);
                auto* playerHealth = world.GetComponent<rtype::common::components::Health>(playerEntity);

                if (!playerPos || !playerHealth || playerHealth->invulnerable) continue;

                float dx = playerPos->x - enemyPos->x;
                float dy = playerPos->y - enemyPos->y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance <= EXPLOSION_RADIUS) {
                    // Check if player has active shield - no damage if shield is active
                    auto* shield = world.GetComponent<rtype::common::components::Shield>(playerEntity);
                    if (shield && shield->isActive) {
                        std::cout << "[COLLISION] Suicide explosion on player " << playerEntity << " - BLOCKED by shield!" << std::endl;
                        continue;
                    }

                    playerHealth->currentHp -= EXPLOSION_DAMAGE;
                    playerHealth->invulnerable = true;
                    playerHealth->invulnerabilityTimer = 1.0f;

                    if (playerHealth->currentHp <= 0) {
                        playerHealth->isAlive = false;
                    }

                    broadcastPlayerStateImmediate(world, playerEntity);
                }
            }
        }

        toDestroy.push_back(suicideEnemy);
    };

    rtype::common::systems::CollisionSystem::update(world, deltaTime, handlers);

    std::sort(toDestroy.begin(), toDestroy.end());
    toDestroy.erase(std::unique(toDestroy.begin(), toDestroy.end()), toDestroy.end());

    for (auto entity : toDestroy) {
        broadcastEntityDestroyToAllRooms(world, entity);
        world.DestroyEntity(entity);
    }
}

void ServerCollisionSystem::broadcastEntityDestroyToAllRooms(
    ECS::World& world,
    ECS::EntityID entityId) {

    // Check if entity has a LinkedRoom component (projectiles, enemies)
    auto* linkedRoom = world.GetComponent<rtype::server::components::LinkedRoom>(entityId);
    if (linkedRoom) {
        // Entity belongs to a specific room - only broadcast to that room
        rtype::server::network::senders::broadcast_entity_destroy(linkedRoom->room_id, static_cast<uint32_t>(entityId), 1);
        return;
    }

    // For entities without LinkedRoom (like players), check if they have a PlayerConn
    auto* pconn = world.GetComponent<rtype::server::components::PlayerConn>(entityId);
    if (pconn && pconn->room_code != 0) {
        // Player entity - find their room and broadcast to it
        auto* roomProps = world.GetAllComponents<rtype::server::components::RoomProperties>();
        if (roomProps) {
            for (const auto& [roomEntity, roomPtr] : *roomProps) {
                if (roomPtr->joinCode == pconn->room_code) {
                    rtype::server::network::senders::broadcast_entity_destroy(roomEntity, static_cast<uint32_t>(entityId), 1);
                    return;
                }
            }
        }
    }

    // Fallback: entity doesn't belong to any specific room (shouldn't happen normally)
    std::cerr << "WARNING: Entity " << entityId << " destroyed but has no room association" << std::endl;
}

void ServerCollisionSystem::broadcastPlayerStateImmediate(ECS::World& world, ECS::EntityID playerId) {
    auto* pos = world.GetComponent<rtype::common::components::Position>(playerId);
    auto* health = world.GetComponent<rtype::common::components::Health>(playerId);
    auto* proom = world.GetComponent<rtype::server::components::LinkedRoom>(playerId);

    if (!pos || !health || !proom) return;

    auto* allPlayers = world.GetAllComponents<rtype::common::components::Player>();
    if (!allPlayers) return;

    for (auto& [otherPid, playerPtr] : *allPlayers) {
        auto otherRoom = world.GetComponent<rtype::server::components::LinkedRoom>(otherPid);
        if (!otherRoom || otherRoom->room_id != proom->room_id) continue;

        network::senders::send_player_state(otherPid, playerId, pos->x, pos->y, pos->rotation, static_cast<uint16_t>(health->currentHp), health->isAlive);
    }
}

} // namespace rtype::server::systems
