/**
 * @file GameLogicSystems.cpp
 * @brief ECS systems implementation for game logic
 *
 * This file contains all ECS system update logic:
 * - Movement System: Now uses common/systems/MovementSystem
 * - Input System: Processes player keyboard input
 * - Fire Rate System: Now uses common/systems/FireRateSystem
 * - Invulnerability System: Manages damage immunity
 * - Enemy Spawn System: Spawns enemies periodically
 * - Enemy AI System: Now uses common/systems/EnemyAISystem
 * - Cleanup System: Removes off-screen entities
 * - Collision System: Detects and handles collisions
 *
 * Part of the modular GameState implementation.
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "packets.h"
#include "packetmanager.h"
#include "network/network.h"
#include "network/senders.h"
#include <common/systems/MovementSystem.h>
#include <common/systems/FireRateSystem.h>
#include <common/systems/EnemyAISystem.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <functional>

namespace rtype::client::gui {

void GameState::updateMovementSystem(float deltaTime) {
    // SERVER-AUTHORITATIVE: Only move player and projectiles locally.
    // Enemies are moved by server state updates (EnemyStatePacket).
    //
    // We manually update positions for:
    // - Player (local prediction)
    // - Projectiles (for smooth rendering)
    //
    // Enemies receive position updates from server.

    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;

    for (auto& [entity, posPtr] : *positions) {
        auto& pos = *posPtr;
        auto* vel = m_world.GetComponent<rtype::common::components::Velocity>(entity);
        if (!vel) continue;

        // Check if this is an enemy - skip if so (server-controlled)
        auto* team = m_world.GetComponent<rtype::common::components::Team>(entity);
        auto* health = m_world.GetComponent<rtype::common::components::Health>(entity);
        if (team && health && team->team == rtype::common::components::TeamType::Enemy) {
            // Enemy movement is controlled by server - skip
            continue;
        }

        // Update position for player and projectiles
        pos.x += vel->vx * deltaTime;
        pos.y += vel->vy * deltaTime;

        // Clamp velocity to max speed
        float speed = std::sqrt(vel->vx * vel->vx + vel->vy * vel->vy);
        if (speed > vel->maxSpeed && vel->maxSpeed > 0.0f) {
            float scale = vel->maxSpeed / speed;
            vel->vx *= scale;
            vel->vy *= scale;
        }
    }
}

void GameState::updateInputSystem(float deltaTime) {
    // Send player input to server periodically (20 times per second)
    static float inputSendTimer = 0.0f;
    static constexpr float INPUT_SEND_INTERVAL = 0.05f; // 50ms = 20Hz
    inputSendTimer += deltaTime;
    
    if (inputSendTimer >= INPUT_SEND_INTERVAL) {
        inputSendTimer = 0.0f;
        
        // Send current input state to server using sender function
        rtype::client::network::senders::send_player_input(m_keyUp, m_keyDown, m_keyLeft, m_keyRight);
    }
    
    // Find player entity (has Player component)
    auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;
    
    for (auto& [entity, playerPtr] : *players) {
        auto* vel = m_world.GetComponent<rtype::common::components::Velocity>(entity);
        auto* pos = m_world.GetComponent<rtype::common::components::Position>(entity);
        auto* fireRate = m_world.GetComponent<rtype::common::components::FireRate>(entity);
        
        if (!vel || !pos) continue;
        
        // Calculate movement direction from input
        float moveX = 0.0f, moveY = 0.0f;
        
        if (m_keyUp) moveY -= 1.0f;
        if (m_keyDown) moveY += 1.0f;
        if (m_keyLeft) moveX -= 1.0f;
        if (m_keyRight) moveX += 1.0f;
        
        // Normalize diagonal movement
        float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
        if (magnitude > 0.0f) {
            moveX /= magnitude;
            moveY /= magnitude;
        }
        
        // Apply velocity
        vel->vx = moveX * vel->maxSpeed;
        vel->vy = moveY * vel->maxSpeed;
        
        // Update player animation based on movement (extracted for better maintainability)
        auto* animation = m_world.GetComponent<rtype::client::components::Animation>(entity);
        auto* sprite = m_world.GetComponent<rtype::client::components::Sprite>(entity);
        updatePlayerAnimation(entity, animation, sprite, m_keyUp);
        
        // Clamp position to screen bounds (with sprite size consideration)
        const float halfSize = 16.0f; // Half of player size (32/2)
        if (pos->x < halfSize) pos->x = halfSize;
        if (pos->x > SCREEN_WIDTH - halfSize) pos->x = SCREEN_WIDTH - halfSize;
        if (pos->y < halfSize) pos->y = halfSize;
        if (pos->y > SCREEN_HEIGHT - halfSize) pos->y = SCREEN_HEIGHT - halfSize;
    }
}

void GameState::updatePlayerAnimation(ECS::EntityID entity,
                                      rtype::client::components::Animation* animation,
                                      rtype::client::components::Sprite* sprite,
                                      bool isMovingUp) {
    if (!animation || !sprite) return;
    
    if (isMovingUp) {
        if (!animation->isPlaying && animation->currentFrame == 0) {
            animation->isPlaying = true;
            animation->currentFrame = 0;
            animation->frameTimer = 0.0f;
            animation->loop = false;
            animation->direction = 1;
        }
    } else {
        // Not moving up: reset to frame 1
        animation->isPlaying = false;
        animation->currentFrame = 0;
        animation->frameTimer = 0.0f;
        
        if (sprite->useTexture) {
            sprite->textureRect.left = 0;
        }
    }
}

void GameState::updateAnimationSystem(float deltaTime) {
    auto* animations = m_world.GetAllComponents<rtype::client::components::Animation>();
    if (!animations) return;
    
    for (auto& [entity, animPtr] : *animations) {
        if (!animPtr->isPlaying) continue;
        
        // Update frame timer
        animPtr->frameTimer += deltaTime;
        
        // Check if it's time to advance to next frame
        if (animPtr->frameTimer >= animPtr->frameDuration) {
            animPtr->frameTimer -= animPtr->frameDuration;
            
            // Only advance if not at last frame
            if (animPtr->currentFrame < animPtr->frameCount - 1) {
                animPtr->currentFrame++;
                
                // Update sprite's textureRect based on current frame
                auto* sprite = m_world.GetComponent<rtype::client::components::Sprite>(entity);
                if (sprite && sprite->useTexture) {
                    sprite->textureRect.left = animPtr->currentFrame * animPtr->frameWidth;
                    sprite->textureRect.width = animPtr->frameWidth;
                    sprite->textureRect.height = animPtr->frameHeight;
                }
            } else {
                // Reached last frame (frame 5) - stay there
                animPtr->isPlaying = false;
            }
        }
    }
}

void GameState::updateFireRateSystem(float deltaTime) {
    // Use the common fire rate system
    rtype::common::systems::FireRateSystem::update(m_world, deltaTime);
}

void GameState::updateChargedShotSystem(float deltaTime) {
    auto* chargedShots = m_world.GetAllComponents<rtype::common::components::ChargedShot>();
    if (!chargedShots) return;
    
    for (auto& [entity, chargedShotPtr] : *chargedShots) {
        chargedShotPtr->update(deltaTime);
    }
}

void GameState::updateInvulnerabilitySystem(float deltaTime) {
    auto* healths = m_world.GetAllComponents<rtype::common::components::Health>();
    if (!healths) return;
    
    for (auto& [entity, healthPtr] : *healths) {
        if (healthPtr->invulnerable && healthPtr->invulnerabilityTimer > 0.0f) {
            healthPtr->invulnerabilityTimer -= deltaTime;
            if (healthPtr->invulnerabilityTimer <= 0.0f) {
                healthPtr->invulnerable = false;
                healthPtr->invulnerabilityTimer = 0.0f;
            }
        }
    }
}

void GameState::updateEnemyAISystem(float deltaTime) {
    // SERVER-AUTHORITATIVE: Enemy AI is now executed ONLY on the server.
    // The client no longer decides when enemies shoot or where they aim.
    // Enemy projectiles are created by the server and sent via SPAWN_PROJECTILE packets.
    //
    // This function is kept for backwards compatibility but does nothing.
    // All enemy behavior is controlled by the server to prevent cheating.

    // DO NOTHING - server handles all enemy AI
}

void GameState::updateCleanupSystem(float deltaTime) {
    std::vector<ECS::EntityID> toDestroy;
    
    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;
    
    for (auto& [entity, posPtr] : *positions) {
        auto& pos = *posPtr;
        
        // Skip player
        if (entity == m_playerEntity) continue;
        
        // Remove entities far off-screen
        bool offScreen = false;
        
        // Check if entity is moving left (enemies and enemy projectiles)
        auto* vel = m_world.GetComponent<rtype::common::components::Velocity>(entity);
        if (vel && vel->vx < 0.0f) {
            // Remove if too far left
            if (pos.x < -100.0f) offScreen = true;
        } else if (vel && vel->vx > 0.0f) {
            // Remove if too far right (player projectiles)
            if (pos.x > SCREEN_WIDTH + 100.0f) offScreen = true;
        }
        
        if (offScreen) {
            toDestroy.push_back(entity);
        }
    }
    
    // Destroy off-screen entities
    for (auto entity : toDestroy) {
        m_world.DestroyEntity(entity);
    }
}

void GameState::checkPlayerVsEnemiesCollision(
    ECS::ComponentArray<rtype::common::components::Position>& positions,
    const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds) {
    
    if (m_playerEntity == 0) return;
    
    auto* playerPos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
    auto* playerHealth = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
    
    if (!playerPos || !playerHealth || playerHealth->invulnerable) return;
    
    sf::FloatRect playerBounds = getBounds(m_playerEntity, *playerPos);
    
    for (const auto& [entity, posPtr] : positions) {
        if (entity == m_playerEntity) continue;
        
        auto* team = m_world.GetComponent<rtype::common::components::Team>(entity);
        auto* health = m_world.GetComponent<rtype::common::components::Health>(entity);
        
        // Check collision with enemies
        if (team && health && team->team == rtype::common::components::TeamType::Enemy) {
            sf::FloatRect enemyBounds = getBounds(entity, *posPtr);
            
            if (playerBounds.intersects(enemyBounds)) {
                damagePlayer(1);
                // Ne PAS détruire l'ennemi - il continue à vivre
            }
        }
    }
}

// HYBRID CLIENT-SIDE PREDICTION + SERVER AUTHORITY:
// - Client detects collisions immediately for low-latency feedback
// - Client applies damage and destroys entities locally (optimistic prediction)
// - Server also detects collisions and sends ENTITY_DESTROY for confirmation
// - Client's destroyEntityByServerId() is idempotent (handles already-destroyed entities)
// 
// For SERVER-OWNED projectiles:
//   - Client predicts collision → destroys enemy locally
//   - Client does NOT destroy projectile (server decides when projectile dies)
//   - Server confirms → sends ENTITY_DESTROY for both projectile and enemy
// 
// This gives instant feedback while maintaining server authority

void GameState::checkPlayerProjectilesVsEnemiesCollision(
    ECS::ComponentArray<rtype::common::components::Position>& positions,
    const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
    std::vector<ECS::EntityID>& toDestroy) {
    
    
    for (const auto& [projEntity, projPosPtr] : positions) {
        auto* projTeam = m_world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = m_world.GetComponent<rtype::common::components::Projectile>(projEntity);
        
        // Skip if not a player projectile
        if (!projTeam || !projData) continue;
        if (projTeam->team != rtype::common::components::TeamType::Player) continue;
        
        sf::FloatRect projBounds = getBounds(projEntity, *projPosPtr);
        
        for (const auto& [enemyEntity, enemyPosPtr] : positions) {
            if (enemyEntity == projEntity) continue;
            
            auto* enemyTeam = m_world.GetComponent<rtype::common::components::Team>(enemyEntity);
            auto* enemyHealth = m_world.GetComponent<rtype::common::components::Health>(enemyEntity);
            
            // Check collision with enemies
            if (enemyTeam && enemyHealth && enemyTeam->team == rtype::common::components::TeamType::Enemy) {
                sf::FloatRect enemyBounds = getBounds(enemyEntity, *enemyPosPtr);
                
                if (projBounds.intersects(enemyBounds)) {
                    // SERVER-AUTHORITATIVE: Collision detected for visual feedback only.
                    // The server will apply damage and send destruction packets.
                    //
                    // We keep collision detection for:
                    // - Playing hit sounds immediately
                    // - Showing particle effects
                    // - Visual feedback for responsiveness
                    //
                    // But we DO NOT:
                    // - Apply damage to enemy HP
                    // - Destroy entities locally
                    // - Mark entities for destruction
                    //
                    // The server will send ENTITY_DESTROY packets when entities die.

                    // Play hit sound for immediate feedback
                    if (m_soundManager.has(AudioFactory::SfxId::EnemyHit)) {
                        m_soundManager.play(AudioFactory::SfxId::EnemyHit);
                    }

                    // NOTE: Projectile is NOT destroyed here. Server will send destruction packet.
                    // This prevents client from cheating by manipulating collision detection.
                }
            }
        }
    }
}

void GameState::checkEnemyProjectilesVsPlayerCollision(
    ECS::ComponentArray<rtype::common::components::Position>& positions,
    const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
    std::vector<ECS::EntityID>& toDestroy) {
    
    if (m_playerEntity == 0) return;
    
    auto* playerPos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
    auto* playerHealth = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
    
    if (!playerPos || !playerHealth || playerHealth->invulnerable) return;
    
    sf::FloatRect playerBounds = getBounds(m_playerEntity, *playerPos);
    
    for (const auto& [projEntity, projPosPtr] : positions) {
        auto* projTeam = m_world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = m_world.GetComponent<rtype::common::components::Projectile>(projEntity);
        
        // Check if it's an enemy projectile
        if (projTeam && projData && projTeam->team == rtype::common::components::TeamType::Enemy) {
            sf::FloatRect projBounds = getBounds(projEntity, *projPosPtr);
            
            if (playerBounds.intersects(projBounds)) {
                // Apply damage to player (player is client-controlled)
                damagePlayer(projData->damage);

                // SERVER-AUTHORITATIVE: Do NOT destroy projectile here.
            }
        }
    }
}

void GameState::updateCollisionSystem() {
    std::vector<ECS::EntityID> toDestroy;
    
    // Get all positions for collision checks
    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;
    
    // Helper lambda to get entity bounds
    auto getBounds = [this](ECS::EntityID entity, const rtype::common::components::Position& pos) -> sf::FloatRect {
        auto* sprite = m_world.GetComponent<rtype::client::components::Sprite>(entity);
        if (!sprite) return sf::FloatRect(pos.x, pos.y, 1.0f, 1.0f);
        
        float realWidth, realHeight;
        
        if (sprite->useTexture) {
            // For textured sprites: use textureRect dimensions (actual frame size) * scale
            realWidth = sprite->textureRect.width * sprite->scale;
            realHeight = sprite->textureRect.height * sprite->scale;
        } else {
            // For colored shapes: use size directly (no scale)
            realWidth = sprite->size.x;
            realHeight = sprite->size.y;
        }
        
        return sf::FloatRect(
            pos.x - realWidth * 0.5f,
            pos.y - realHeight * 0.5f,
            realWidth,
            realHeight
        );
    };
    
    // Run collision detection subsystems
    checkPlayerVsEnemiesCollision(*positions, getBounds);
    checkPlayerProjectilesVsEnemiesCollision(*positions, getBounds, toDestroy);
    checkEnemyProjectilesVsPlayerCollision(*positions, getBounds, toDestroy);
    
    // SERVER-AUTHORITATIVE: Do NOT destroy entities locally.
    // The server will send ENTITY_DESTROY packets when entities die.
    //
    // toDestroy vector is now empty because we removed all markings.
    // This prevents client from destroying entities without server confirmation.
    //
    // Entity destruction only happens in destroyEntityByServerId() when
    // server sends ENTITY_DESTROY packet.

    // NOTE: toDestroy vector should be empty now, but we keep this for safety
    if (!toDestroy.empty()) {
        std::cout << "[GameState] WARNING: toDestroy vector is not empty! This shouldn't happen with server-authoritative model." << std::endl;
    }
}

void GameState::handlePlayerFire() {
    if (m_playerEntity == 0) return;
    
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
    if (!pos) return;
    
    createPlayerProjectile(pos->x, pos->y);
}

} // namespace rtype::client::gui
