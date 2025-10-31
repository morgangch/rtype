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
#include <common/systems/CollisionSystem.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <functional>

namespace rtype::client::gui {

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

void GameState::updateEnemyAISystem(float deltaTime) {
    // Local prediction callback - creates enemy projectiles immediately on client
    auto createProjectile = [this](ECS::EntityID shooter, float x, float y, float vx, float vy) {
        createEnemyProjectile(x, y, vx, vy);
    };

    // Use shared EnemyAI logic for local prediction (server will confirm via SPAWN_PROJECTILE)
    rtype::common::systems::EnemyAISystem::update(m_world, deltaTime, createProjectile);
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

void GameState::updateCollisionSystem() {
    rtype::common::systems::CollisionHandlers handlers;
    std::vector<ECS::EntityID> toDestroy;

    handlers.onPlayerVsEnemy = [this](ECS::EntityID player, ECS::EntityID enemy, ECS::World& world) {
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        if (!playerHealth || playerHealth->invulnerable) return;

        playerHealth->currentHp -= 1;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        // Check for game over
        if (playerHealth->currentHp <= 0) {
            showInGameMenu(true); // Game Over
        } else {
            if (m_soundManager.has(AudioFactory::SfxId::LoseLife)) {
                m_soundManager.play(AudioFactory::SfxId::LoseLife);
            }
        }
    };

    handlers.onPlayerProjectileVsEnemy = [this, &toDestroy](ECS::EntityID proj, ECS::EntityID enemy, ECS::World& world) {
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        auto* enemyHealth = world.GetComponent<rtype::common::components::Health>(enemy);
        if (!projData || !enemyHealth) return;

        enemyHealth->currentHp -= projData->damage;

        if (enemyHealth->currentHp <= 0) {
            toDestroy.push_back(enemy);
        }

        if (!projData->piercing) {
            toDestroy.push_back(proj);
        }
    };

    handlers.onEnemyProjectileVsPlayer = [this, &toDestroy](ECS::EntityID proj, ECS::EntityID player, ECS::World& world) {
        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(player);
        auto* projData = world.GetComponent<rtype::common::components::Projectile>(proj);
        if (!playerHealth || !projData || playerHealth->invulnerable) return;

        playerHealth->currentHp -= projData->damage;
        playerHealth->invulnerable = true;
        playerHealth->invulnerabilityTimer = 1.0f;

        // Check for game over
        if (playerHealth->currentHp <= 0) {
            showInGameMenu(true); // Game Over
        } else {
            if (m_soundManager.has(AudioFactory::SfxId::LoseLife)) {
                m_soundManager.play(AudioFactory::SfxId::LoseLife);
            }
        }

        toDestroy.push_back(proj);
    };

    handlers.onSuicideExplosion = [this, &toDestroy](ECS::EntityID suicideEnemy, ECS::World& world) {
        auto* enemyPos = world.GetComponent<rtype::common::components::Position>(suicideEnemy);
        if (!enemyPos) return;

        const float EXPLOSION_RADIUS = 100.0f;
        const int EXPLOSION_DAMAGE = 2;

        auto* playerHealth = world.GetComponent<rtype::common::components::Health>(m_playerEntity);
        auto* playerPos = world.GetComponent<rtype::common::components::Position>(m_playerEntity);

        if (playerHealth && playerPos && !playerHealth->invulnerable) {
            float dx = playerPos->x - enemyPos->x;
            float dy = playerPos->y - enemyPos->y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= EXPLOSION_RADIUS) {
                playerHealth->currentHp -= EXPLOSION_DAMAGE;
                playerHealth->invulnerable = true;
                playerHealth->invulnerabilityTimer = 1.0f;

                // Check for game over
                if (playerHealth->currentHp <= 0) {
                    showInGameMenu(true); // Game Over
                } else {
                    if (m_soundManager.has(AudioFactory::SfxId::LoseLife)) {
                        m_soundManager.play(AudioFactory::SfxId::LoseLife);
                    }
                }
            }
        }

        toDestroy.push_back(suicideEnemy);
    };

    rtype::common::systems::CollisionSystem::update(m_world, 0.0f, handlers);

    for (auto entity : toDestroy) {
        // Determine if this was a boss or regular enemy
        auto* enemyType = m_world.GetComponent<rtype::common::components::EnemyTypeComponent>(entity);
        if (enemyType) {
            if (enemyType->type == rtype::common::components::EnemyType::TankDestroyer) {
                // Play boss death sound
                if (m_soundManager.has(AudioFactory::SfxId::BossDeath)) {
                    m_soundManager.play(AudioFactory::SfxId::BossDeath);
                }
                // Restore level background music after boss death
                loadLevelMusic();
            } else if (m_soundManager.has(AudioFactory::SfxId::EnemyDeath)) {
                // Regular enemy death
                m_soundManager.play(AudioFactory::SfxId::EnemyDeath);
            }
        }

        // Clean up server entity mapping if this was a server-owned entity
        for (auto it = m_serverEntityMap.begin(); it != m_serverEntityMap.end(); ) {
            if (it->second == entity) {
                std::cout << "[GameState] Cleaning up server mapping for locally destroyed entity: clientId=" << entity << " serverId=" << it->first << std::endl;
                it = m_serverEntityMap.erase(it);
                break;
            } else {
                ++it;
            }
        }

        m_world.DestroyEntity(entity);
    }
}

void GameState::handlePlayerFire() {
    if (m_playerEntity == 0) return;
    
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
    if (!pos) return;
    
    createPlayerProjectile(pos->x, pos->y);
}

} // namespace rtype::client::gui
