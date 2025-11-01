/**
 * @file EntityFactory.cpp
 * @brief Entity factory methods for creating game entities
 * 
 * This file contains all entity creation logic using ECS components 
 * for player, enemies, and projectiles.
 * 
 * Part of the modular GameState implementation.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "gui/AssetPaths.h"
#include "gui/TextureCache.h"
#include <common/components/VesselClass.h>
#include <common/components/Explosion.h>
#include <common/components/Homing.h>
#include <common/components/Shield.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

ECS::EntityID GameState::createPlayer(rtype::common::components::VesselType vesselType) {
    auto entity = m_world.CreateEntity();
    
    // Create VesselClass component with selected type
    auto vesselClass = rtype::common::components::VesselClass(vesselType);
    m_world.AddComponent<rtype::common::components::VesselClass>(entity, vesselClass);
    
    // Position - Center-left of screen
    m_world.AddComponent<rtype::common::components::Position>(
        entity, 100.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
    
    // Velocity - Modified by vessel stats (base 300 px/s)
    float baseSpeed = 300.0f;
    float effectiveSpeed = vesselClass.getEffectiveSpeed(baseSpeed);
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, 0.0f, 0.0f, effectiveSpeed);
    
    // Health - Modified by vessel stats (base 3 HP)
    int baseHealth = 3;
    int effectiveHealth = vesselClass.getEffectiveMaxHealth(baseHealth);
    m_world.AddComponent<rtype::common::components::Health>(entity, effectiveHealth);
    
    // Sprite - Player ship using PLAYER.gif sprite sheet with correct row
    // PLAYER.gif is 166x86 pixels: 5 frames × 5 rows (each frame is 33x17)
    // Different vessels are on different rows of the sprite sheet
    int spriteRow = vesselClass.getSpriteSheetRow();  // Y offset (0, 17, 34, or 51)
    
    // Preload texture to avoid first-frame hitch
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::player::PLAYER_SPRITE);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity, 
        rtype::client::assets::player::PLAYER_SPRITE,  // Always use PLAYER.gif
        sf::Vector2f(33.0f, 17.0f),
        true,
        sf::IntRect(0, spriteRow, 33, 17),  // Select correct row for vessel type
        3.0f);  // Scale 3x for better visibility (33*3 = 99 pixels)
    
    // Animation - 5 frames, 0.08s per frame (smooth animation when moving up)
    m_world.AddComponent<rtype::client::components::Animation>(
        entity,
        5,      // 5 frames total
        0.08f,  // 0.08s per frame (fast animation)
        33,     // Frame width
        17);    // Frame height
    
    // Player - Marks as player-controlled with vessel type
    m_world.AddComponent<rtype::common::components::Player>(entity, "Player1", 0, vesselType);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // FireRate - Modified by vessel stats (base 0.2s cooldown)
    float baseCooldown = FIRE_COOLDOWN;
    float effectiveCooldown = vesselClass.getEffectiveFireCooldown(baseCooldown);
    m_world.AddComponent<rtype::common::components::FireRate>(entity, effectiveCooldown);
    
    // ChargedShot - Enable charged shooting mechanic with vessel-specific charge time
    auto chargedShot = rtype::common::components::ChargedShot(vesselClass.chargeTime);
    m_world.AddComponent<rtype::common::components::ChargedShot>(entity, chargedShot);
    
    // Shield - Add shield component for Solar Guardian
    if (vesselType == rtype::common::components::VesselType::SolarGuardian) {
        m_world.AddComponent<rtype::common::components::Shield>(entity, 3.0f, 1.0f, 6.0f);
    }
    
    return entity;
}

ECS::EntityID GameState::createEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x, y, 0.0f);
    
    // Velocity - Moves left at 100 px/s
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, -100.0f, 0.0f, 100.0f);
    
    // Health - 1 HP (dies in one hit)
    m_world.AddComponent<rtype::common::components::Health>(entity, 1);
    
    // Sprite - Basic enemy with texture (first frame: 33x36 from 533x36 spritesheet)
    // Enemy spritesheet has ~16 frames: 533/33 = ~16 frames
    // Preload enemy sprite
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BASIC_ENEMY_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BASIC_ENEMY_1,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),  // First frame
        2.5f);  // Scale 2.5x (33*2.5 = 82 pixels)
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // EnemyType - Basic enemy (shoots straight)
    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Basic);
    
    // FireRate - Basic enemy shoots every 2.5 seconds
    // Random initial cooldown to stagger shots
    float randomCooldown = static_cast<float>(rand() % 1000) / 1000.0f * ENEMY_FIRE_INTERVAL;
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(
        entity, ENEMY_FIRE_INTERVAL);
    fireRate->cooldown = randomCooldown;
    
    return entity;
}

ECS::EntityID GameState::createSnakeEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x, y, 0.0f);
    
    // Velocity - Moves left at 120 px/s, vertical oscillation
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, -120.0f, 0.0f, 120.0f);
    
    // Health - 1 HP (dies in one hit)
    m_world.AddComponent<rtype::common::components::Health>(entity, 1);
    
    // TODO: Replace with a unique snake enemy sprite and animation
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BASIC_ENEMY_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BASIC_ENEMY_2,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),  // First frame
        2.5f);  // Scale 2.5x
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // EnemyType - Snake enemy (moves in sine wave)
    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Snake);
    
    return entity;
}

ECS::EntityID GameState::createSuicideEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -150.0f, 0.0f, 200.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 1);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BASIC_ENEMY_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BASIC_ENEMY_2,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Suicide);

    return entity;
}

ECS::EntityID GameState::createTankDestroyer(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x, y, 0.0f);
    
    // Velocity - Moves vertically (oscillates up and down)
    // Starts moving down, will bounce at screen edges
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, 0.0f, 50.0f, 50.0f);
    
    // Health - 20 HP (BOSS - very tough!)
    m_world.AddComponent<rtype::common::components::Health>(entity, 20);
    
    // Sprite - Boss enemy with texture (first frame from BOSS_ENEMY_1)
    // Preload boss sprite
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BOSS_ENEMY_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BOSS_ENEMY_1,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),  // First frame
        5.0f);  // Scale 5x for boss (33*5 = 165 pixels - LARGE!)
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // EnemyType - TankDestroyer boss
    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::TankDestroyer);
    
    // FireRate - Boss shoots every 0.8 seconds (rapid fire!)
    const float BOSS_FIRE_INTERVAL = 0.8f;
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(
        entity, BOSS_FIRE_INTERVAL);
    fireRate->cooldown = 0.0f;  // Can shoot immediately
    
    return entity;
}

ECS::EntityID GameState::createPlayerProjectile(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position - Spawn at player's position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x + 16.0f, y, 0.0f); // +16 to spawn from right edge of player
    
    // Velocity - Moves right at 500 px/s
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, 500.0f, 0.0f, 500.0f);
    
    // Sprite - PROJECTILE_2, frame 2, première ligne seulement
    // Preload projectile textures
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::projectiles::PROJECTILE_1,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 0, 81, 17),
        0.5f);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile - 1 damage, non-piercing, client-owned
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1, false /* piercing */, false /* serverOwned */);
    
    return entity;
}

ECS::EntityID GameState::createEnemyProjectile(float x, float y, float vx, float vy) {
    auto entity = m_world.CreateEntity();
    
    // Position - Spawn at enemy's position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x - 12.0f, y, 0.0f); // -12 to spawn from left edge of enemy
    
    // Velocity - Custom direction (can aim at player or shoot straight)
    float speed = std::sqrt(vx * vx + vy * vy);
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, vx, vy, speed);
    
    // Sprite - PROJECTILE_1 (orange), frame 2, première ligne
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::projectiles::PROJECTILE_2,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 0, 81, 17),  // Frame 2, ligne 1: orange
        0.4f);  // Scale 0.4x (81*0.4 = 32px de large, 17*0.4 = 7px de haut)
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // Projectile - 1 damage, non-piercing
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1, false /* piercing */, false /* serverOwned */, 300.0f, rtype::common::components::ProjectileType::Basic);
    
    return entity;
}

ECS::EntityID GameState::createChargedProjectile(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x, y, 0.0f);
    
    // Velocity - Faster than normal (600 vs 500)
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, 600.0f, 0.0f, 600.0f);
    
    // Sprite - PROJECTILE_4 (rose/magenta), frame 2, ligne 2 (plus dense et imposant)
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_4);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::projectiles::PROJECTILE_4,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 17, 81, 17),  // Frame 2, ligne 2: plus dense (31.1% vs 15.2%)
        0.6f);  // Scale 0.6x (81*0.6 = 49px de large, 17*0.6 = 10px de haut) - plus gros
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile - 2 damage, PIERCING, client-owned
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 2, true /* piercing */, false /* serverOwned */, 600.0f, rtype::common::components::ProjectileType::Piercing);
    
    return entity;
}

ECS::EntityID GameState::createPataEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -100.0f, 0.0f, 100.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 2);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BASIC_ENEMY_3);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BASIC_ENEMY_3,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Pata);

    // Pata fires double shots every 3.5s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 3.5f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 3.5f;

    return entity;
}

ECS::EntityID GameState::createShieldedEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -90.0f, 0.0f, 90.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 4);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::ADVANCED_ENEMY_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::ADVANCED_ENEMY_1,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Shielded);

    // Shielded fires every 5.0s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 5.0f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 5.0f;

    return entity;
}

ECS::EntityID GameState::createFlankerEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -90.0f, 0.0f, 120.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 3);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::ADVANCED_ENEMY_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::ADVANCED_ENEMY_2,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Flanker);

    // Flanker fires perpendicular shots every 4.5s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 4.5f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 4.5f;

    return entity;
}

ECS::EntityID GameState::createBomberEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -70.0f, 0.0f, 70.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 3);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::ADVANCED_ENEMY_3);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::ADVANCED_ENEMY_3,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Bomber);

    // Bomber drops mines every 6.0s (handled by AI system - mines to be implemented)
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 6.0f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 6.0f;

    return entity;
}

ECS::EntityID GameState::createWaverEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -110.0f, 0.0f, 150.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 4);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::ADVANCED_ENEMY_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::ADVANCED_ENEMY_1,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        2.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Waver);

    // Waver fires triple bursts every 4.0s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 4.0f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 4.0f;

    return entity;
}

ECS::EntityID GameState::createSerpentBoss(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -60.0f, 0.0f, 60.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 60);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BOSS_ENEMY_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BOSS_ENEMY_2,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        5.0f);  // Boss size

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Serpent);

    // Serpent fires 5-spread every 1.0s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 1.0f);
    fireRate->cooldown = 0.0f;

    return entity;
}

ECS::EntityID GameState::createFortressBoss(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -20.0f, 0.0f, 30.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 80);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BOSS_ENEMY_3);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BOSS_ENEMY_3,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        6.0f);  // Largest boss

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Fortress);

    // Fortress fires 8-directional rotating every 0.7s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 0.7f);
    fireRate->cooldown = 0.0f;

    return entity;
}

ECS::EntityID GameState::createCoreBoss(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, -40.0f, 0.0f, 80.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 100);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BOSS_ENEMY_4);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BOSS_ENEMY_4,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        5.5f);  // Final boss size

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Core);

    // Core fires multi-phase patterns every 0.6s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 0.6f);
    fireRate->cooldown = 0.0f;

    return entity;
}

// ===== Weapon Mode Implementations =====

int GameState::createDualProjectiles(float x, float y) {
    // Azure Phantom - Dual rapid projectiles with vertical offset
    const float verticalOffset = 5.0f;
    
    // Upper projectile
    auto proj1 = m_world.CreateEntity();
    m_world.AddComponent<rtype::common::components::Position>(proj1, x + 16.0f, y - verticalOffset, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(proj1, 500.0f, 0.0f, 500.0f);
    m_world.AddComponent<rtype::common::components::Team>(proj1, rtype::common::components::TeamType::Player);
    m_world.AddComponent<rtype::common::components::Projectile>(proj1, 1, false, false, 500.0f, 
                                                                 rtype::common::components::ProjectileType::Basic);
    
    // Sprite for projectile
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_1);
    m_world.AddComponent<rtype::client::components::Sprite>(
        proj1,
        rtype::client::assets::projectiles::PROJECTILE_1,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 0, 81, 17),
        0.4f); // Slightly smaller than normal
    
    // Lower projectile
    auto proj2 = m_world.CreateEntity();
    m_world.AddComponent<rtype::common::components::Position>(proj2, x + 16.0f, y + verticalOffset, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(proj2, 500.0f, 0.0f, 500.0f);
    m_world.AddComponent<rtype::common::components::Team>(proj2, rtype::common::components::TeamType::Player);
    m_world.AddComponent<rtype::common::components::Projectile>(proj2, 1, false, false, 500.0f, 
                                                                 rtype::common::components::ProjectileType::Basic);
    
    m_world.AddComponent<rtype::client::components::Sprite>(
        proj2,
        rtype::client::assets::projectiles::PROJECTILE_1,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 0, 81, 17),
        0.4f);
    
    return 2; // Created 2 projectiles
}

int GameState::createHomingBurst(float x, float y, int count) {
    // Azure Phantom - Burst of homing projectiles
    // TODO: Implement homing behavior in a future system
    // For now, create projectiles with slight angle variation
    
    for (int i = 0; i < count; ++i) {
        auto proj = m_world.CreateEntity();
        
        // Stagger spawn positions slightly
        float offsetY = (i - count / 2) * 8.0f;
        float offsetX = i * 5.0f; // Slight forward offset
        
        m_world.AddComponent<rtype::common::components::Position>(proj, x + 16.0f + offsetX, y + offsetY, 0.0f);
        
        // Slightly different velocities for spread
        float vx = 450.0f + (i * 10.0f);
        m_world.AddComponent<rtype::common::components::Velocity>(proj, vx, 0.0f, vx);
        
        m_world.AddComponent<rtype::common::components::Team>(proj, rtype::common::components::TeamType::Player);
        m_world.AddComponent<rtype::common::components::Projectile>(proj, 1, false, false, vx, 
                                                                     rtype::common::components::ProjectileType::Missile);
        
        // Add homing component
        m_world.AddComponent<rtype::common::components::Homing>(proj, 400.0f, 3.0f, vx, 0.5f);
        
        // Use different sprite (purple/homing visual)
        rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_3);
        m_world.AddComponent<rtype::client::components::Sprite>(
            proj,
            rtype::client::assets::projectiles::PROJECTILE_3,
            sf::Vector2f(81.0f, 17.0f),
            true,
            sf::IntRect(185, 0, 81, 17),
            0.35f); // Small projectiles
    }
    
    return count;
}

int GameState::createSpreadShot(float x, float y, int count) {
    // Solar Guardian - Shotgun spread pattern
    const float spreadAngle = 15.0f; // degrees
    const float angleStep = (spreadAngle * 2.0f) / (count - 1);
    
    for (int i = 0; i < count; ++i) {
        auto proj = m_world.CreateEntity();
        
        // Calculate angle for this pellet (-15° to +15°)
        float angle = -spreadAngle + (angleStep * i);
        float angleRad = angle * 3.14159f / 180.0f;
        
        // Calculate velocity components
        float speed = 600.0f; // Fast, short-range
        float vx = speed * std::cos(angleRad);
        float vy = speed * std::sin(angleRad);
        
        m_world.AddComponent<rtype::common::components::Position>(proj, x + 16.0f, y, 0.0f);
        m_world.AddComponent<rtype::common::components::Velocity>(proj, vx, vy, speed);
        
        m_world.AddComponent<rtype::common::components::Team>(proj, rtype::common::components::TeamType::Player);
        
        // Each pellet does less damage, shorter range
        auto* projectile = m_world.AddComponent<rtype::common::components::Projectile>(
            proj, 1, false, false, speed, rtype::common::components::ProjectileType::Spread);
        projectile->maxDistance = 300.0f; // Short range
        
        // Small yellow pellets
        rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_1);
        m_world.AddComponent<rtype::client::components::Sprite>(
            proj,
            rtype::client::assets::projectiles::PROJECTILE_1,
            sf::Vector2f(81.0f, 17.0f),
            true,
            sf::IntRect(185, 0, 81, 17),
            0.3f); // Very small
    }
    
    return count;
}

ECS::EntityID GameState::createExplosiveProjectile(float x, float y, bool isCharged) {
    // Emerald Titan - Explosive projectile with AoE
    auto entity = m_world.CreateEntity();
    
    // Slower, heavier projectile
    float speed = isCharged ? 350.0f : 400.0f;
    int damage = isCharged ? 4 : 2;
    
    m_world.AddComponent<rtype::common::components::Position>(entity, x + 16.0f, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, speed, 0.0f, speed);
    
    m_world.AddComponent<rtype::common::components::Team>(entity, rtype::common::components::TeamType::Player);
    m_world.AddComponent<rtype::common::components::Projectile>(entity, damage, false, false, speed, 
                                                                 rtype::common::components::ProjectileType::Missile);
    
    // Add explosion component with AoE damage
    float explosionRadius = isCharged ? 80.0f : 50.0f;
    int centerDmg = isCharged ? 4 : 2;
    int edgeDmg = isCharged ? 2 : 1;
    m_world.AddComponent<rtype::common::components::Explosion>(entity, explosionRadius, centerDmg, edgeDmg, 0.3f);
    
    // Large explosive sprite (red/orange)
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::projectiles::PROJECTILE_2,
        sf::Vector2f(81.0f, 17.0f),
        true,
        sf::IntRect(185, 0, 81, 17),
        isCharged ? 0.8f : 0.6f); // Bigger if charged
    
    // TODO: Add explosion component for AoE damage on impact
    
    return entity;
}

} // namespace rtype::client::gui
