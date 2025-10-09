/**
 * @file GameLogicSystems.cpp
 * @brief ECS systems implementation for game logic
 * 
 * This file contains all ECS system update logic:
 * - Movement System: Updates positions based on velocity
 * - Input System: Processes player keyboard input
 * - Fire Rate System: Manages shooting cooldowns
 * - Invulnerability System: Manages damage immunity
 * - Enemy Spawn System: Spawns enemies periodically
 * - Enemy AI System: Enemy shooting logic
 * - Cleanup System: Removes off-screen entities
 * - Collision System: Detects and handles collisions
 * 
 * Part of the modular GameState implementation.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include <cmath>
#include <vector>

namespace rtype::client::gui {

// =============================================================================
// MOVEMENT SYSTEM
// =============================================================================

void GameState::updateMovementSystem(float deltaTime) {
    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;
    
    for (auto& [entity, posPtr] : *positions) {
        auto& pos = *posPtr;
        auto* vel = m_world.GetComponent<rtype::common::components::Velocity>(entity);
        
        if (!vel) continue;
        
        // Update position
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

// =============================================================================
// INPUT SYSTEM
// =============================================================================

void GameState::updateInputSystem(float deltaTime) {
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
        
        // Clamp position to screen bounds (with sprite size consideration)
        const float halfSize = 16.0f; // Half of player size (32/2)
        if (pos->x < halfSize) pos->x = halfSize;
        if (pos->x > SCREEN_WIDTH - halfSize) pos->x = SCREEN_WIDTH - halfSize;
        if (pos->y < halfSize) pos->y = halfSize;
        if (pos->y > SCREEN_HEIGHT - halfSize) pos->y = SCREEN_HEIGHT - halfSize;
        
        // Handle firing
        if (m_keyFire && fireRate && fireRate->canFire()) {
            handlePlayerFire();
            fireRate->shoot();
        }
    }
}

// =============================================================================
// FIRE RATE SYSTEM
// =============================================================================

void GameState::updateFireRateSystem(float deltaTime) {
    auto* fireRates = m_world.GetAllComponents<rtype::common::components::FireRate>();
    if (!fireRates) return;
    
    for (auto& [entity, fireRatePtr] : *fireRates) {
        fireRatePtr->update(deltaTime);
    }
}

// =============================================================================
// INVULNERABILITY SYSTEM - Uses Health.invulnerable
// =============================================================================

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

// =============================================================================
// ENEMY SPAWN SYSTEM
// =============================================================================

void GameState::updateEnemySpawnSystem(float deltaTime) {
    m_enemySpawnTimer += deltaTime;
    
    if (m_enemySpawnTimer >= ENEMY_SPAWN_INTERVAL) {
        // Count current enemies
        size_t enemyCount = 0;
        auto* teams = m_world.GetAllComponents<rtype::common::components::Team>();
        if (teams) {
            for (auto& [entity, teamPtr] : *teams) {
                if (teamPtr->team == rtype::common::components::TeamType::Enemy) {
                    // Check if it's actually an enemy (has Health, not a projectile)
                    if (m_world.GetComponent<rtype::common::components::Health>(entity)) {
                        enemyCount++;
                    }
                }
            }
        }
        
        // Spawn if under limit
        if (enemyCount < MAX_ENEMIES) {
            float randomY = 50.0f + static_cast<float>(rand() % static_cast<int>(SCREEN_HEIGHT - 100.0f));
            
            // Alternate between basic enemies and shooter enemies
            // 40% chance for shooter enemy, 60% for basic enemy
            int randomType = rand() % 100;
            if (randomType < 40) {
                createShooterEnemy(SCREEN_WIDTH + 28.0f, randomY);
            } else {
                createEnemy(SCREEN_WIDTH + 24.0f, randomY);
            }
            
            m_enemySpawnTimer = 0.0f;
        }
    }
}

// =============================================================================
// ENEMY AI SYSTEM
// =============================================================================

void GameState::updateEnemyAISystem(float deltaTime) {
    auto* teams = m_world.GetAllComponents<rtype::common::components::Team>();
    if (!teams) return;
    
    // Find player position first
    sf::Vector2f playerPos(0.0f, 0.0f);
    bool playerFound = false;
    auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
    if (players) {
        for (auto& [playerEntity, playerPtr] : *players) {
            auto* pos = m_world.GetComponent<rtype::common::components::Position>(playerEntity);
            if (pos) {
                playerPos = sf::Vector2f(pos->x, pos->y);
                playerFound = true;
                break;
            }
        }
    }
    
    for (auto& [entity, teamPtr] : *teams) {
        // Only process enemies (not enemy projectiles)
        if (teamPtr->team != rtype::common::components::TeamType::Enemy) continue;
        if (!m_world.GetComponent<rtype::common::components::Health>(entity)) continue;
        
        auto* enemyType = m_world.GetComponent<rtype::common::components::EnemyTypeComponent>(entity);
        auto* fireRate = m_world.GetComponent<rtype::common::components::FireRate>(entity);
        auto* pos = m_world.GetComponent<rtype::common::components::Position>(entity);
        
        if (!fireRate || !pos || !fireRate->canFire()) continue;
        
        if (enemyType && enemyType->type == rtype::common::components::EnemyType::Shooter) {
            // Shooter enemy: aim at player
            if (playerFound) {
                // Calculate direction to player
                float dx = playerPos.x - pos->x;
                float dy = playerPos.y - pos->y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance > 0.0f) {
                    // Normalize and apply speed
                    const float PROJECTILE_SPEED = 300.0f;
                    float vx = (dx / distance) * PROJECTILE_SPEED;
                    float vy = (dy / distance) * PROJECTILE_SPEED;
                    
                    createEnemyProjectile(pos->x, pos->y, vx, vy);
                    fireRate->shoot();
                }
            }
        } else {
            // Basic red enemy: shoot straight left
            createEnemyProjectile(pos->x, pos->y, -300.0f, 0.0f);
            fireRate->shoot();
        }
    }
}

// =============================================================================
// CLEANUP SYSTEM
// =============================================================================

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

// =============================================================================
// COLLISION SYSTEM
// =============================================================================

void GameState::updateCollisionSystem() {
    std::vector<ECS::EntityID> toDestroy;
    
    // Get all positions for collision checks
    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;
    
    // Helper lambda for AABB collision
    auto checkAABB = [](const sf::FloatRect& a, const sf::FloatRect& b) -> bool {
        return a.intersects(b);
    };
    
    // Helper lambda to get entity bounds
    auto getBounds = [this](ECS::EntityID entity, const rtype::common::components::Position& pos) -> sf::FloatRect {
        auto* sprite = m_world.GetComponent<rtype::client::components::Sprite>(entity);
        if (!sprite) return sf::FloatRect(pos.x, pos.y, 1.0f, 1.0f);
        
        return sf::FloatRect(
            pos.x - sprite->size.x * 0.5f,
            pos.y - sprite->size.y * 0.5f,
            sprite->size.x,
            sprite->size.y
        );
    };
    
    // 1. PLAYER vs ENEMIES collision
    if (m_playerEntity != 0) {
        auto* playerPos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
        auto* playerHealth = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
        
        if (playerPos && playerHealth && !playerHealth->invulnerable) {
            sf::FloatRect playerBounds = getBounds(m_playerEntity, *playerPos);
            
            for (auto& [entity, posPtr] : *positions) {
                if (entity == m_playerEntity) continue;
                
                auto* team = m_world.GetComponent<rtype::common::components::Team>(entity);
                auto* health = m_world.GetComponent<rtype::common::components::Health>(entity);
                
                // Check collision with enemies
                if (team && health && team->team == rtype::common::components::TeamType::Enemy) {
                    sf::FloatRect enemyBounds = getBounds(entity, *posPtr);
                    
                    if (checkAABB(playerBounds, enemyBounds)) {
                        damagePlayer(1);
                        // Ne PAS détruire l'ennemi - il continue à vivre
                    }
                }
            }
        }
    }
    
    // 2. PLAYER PROJECTILES vs ENEMIES collision
    for (auto& [projEntity, projPosPtr] : *positions) {
        auto* projTeam = m_world.GetComponent<rtype::common::components::Team>(projEntity);
        auto* projData = m_world.GetComponent<rtype::common::components::Projectile>(projEntity);
        
        // Skip if not a player projectile
        if (!projTeam || !projData) continue;
        if (projTeam->team != rtype::common::components::TeamType::Player) continue;
        
        sf::FloatRect projBounds = getBounds(projEntity, *projPosPtr);
        
        for (auto& [enemyEntity, enemyPosPtr] : *positions) {
            if (enemyEntity == projEntity) continue;
            
            auto* enemyTeam = m_world.GetComponent<rtype::common::components::Team>(enemyEntity);
            auto* enemyHealth = m_world.GetComponent<rtype::common::components::Health>(enemyEntity);
            
            // Check collision with enemies
            if (enemyTeam && enemyHealth && enemyTeam->team == rtype::common::components::TeamType::Enemy) {
                sf::FloatRect enemyBounds = getBounds(enemyEntity, *enemyPosPtr);
                
                if (checkAABB(projBounds, enemyBounds)) {
                    // Damage enemy
                    enemyHealth->currentHp -= projData->damage;
                    toDestroy.push_back(projEntity); // Destroy projectile
                    
                    if (enemyHealth->currentHp <= 0) {
                        enemyHealth->isAlive = false;
                        toDestroy.push_back(enemyEntity); // Destroy enemy
                    }
                    break; // Projectile can only hit one target
                }
            }
        }
    }
    
    // 3. ENEMY PROJECTILES vs PLAYER collision
    if (m_playerEntity != 0) {
        auto* playerPos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
        auto* playerHealth = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
        
        if (playerPos && playerHealth && !playerHealth->invulnerable) {
            sf::FloatRect playerBounds = getBounds(m_playerEntity, *playerPos);
            
            for (auto& [projEntity, projPosPtr] : *positions) {
                auto* projTeam = m_world.GetComponent<rtype::common::components::Team>(projEntity);
                auto* projData = m_world.GetComponent<rtype::common::components::Projectile>(projEntity);
                
                // Check if it's an enemy projectile
                if (projTeam && projData && projTeam->team == rtype::common::components::TeamType::Enemy) {
                    sf::FloatRect projBounds = getBounds(projEntity, *projPosPtr);
                    
                    if (checkAABB(playerBounds, projBounds)) {
                        damagePlayer(projData->damage);
                        toDestroy.push_back(projEntity); // Destroy projectile
                    }
                }
            }
        }
    }
    
    // Destroy all marked entities
    for (auto entity : toDestroy) {
        m_world.DestroyEntity(entity);
    }
}

// =============================================================================
// PLAYER FIRE HANDLER
// =============================================================================

void GameState::handlePlayerFire() {
    if (m_playerEntity == 0) return;
    
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(m_playerEntity);
    if (!pos) return;
    
    createPlayerProjectile(pos->x, pos->y);
}

} // namespace rtype::client::gui
