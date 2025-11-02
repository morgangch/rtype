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
#include "components/ShieldVisual.h"
#include <common/components/Shield.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

ECS::EntityID GameState::createPlayer() {
    auto entity = m_world.CreateEntity();
    
    // Position - Center-left of screen
    m_world.AddComponent<rtype::common::components::Position>(
        entity, 100.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
    
    // Velocity - Start stationary, max speed 300 px/s
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, 0.0f, 0.0f, 300.0f);
    
    // Health - 3 HP (invulnerability built-in)
    m_world.AddComponent<rtype::common::components::Health>(entity, 3);
    
    // Sprite - Player ship with texture (first frame: 33x17 from 166x86 spritesheet)
    // Player spritesheet has 5 frames horizontally: 166/5 = ~33 pixels per frame
    // Preload texture to avoid first-frame hitch
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::player::PLAYER_SPRITE);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity, 
        rtype::client::assets::player::PLAYER_SPRITE,
        sf::Vector2f(33.0f, 17.0f),
        true,
        sf::IntRect(0, 0, 33, 17),  // First frame of spritesheet
        3.0f);  // Scale 3x for better visibility (33*3 = 99 pixels)
    
    // Animation - 5 frames, 0.08s per frame (smooth animation when moving up)
    m_world.AddComponent<rtype::client::components::Animation>(
        entity,
        5,      // 5 frames total
        0.08f,  // 0.08s per frame (fast animation)
        33,     // Frame width
        17);    // Frame height
    
    // Player - Marks as player-controlled
    m_world.AddComponent<rtype::common::components::Player>(entity, "Player1", 0);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // FireRate - 0.2s cooldown between shots
    m_world.AddComponent<rtype::common::components::FireRate>(entity, FIRE_COOLDOWN);
    
    // ChargedShot - Enable charged shooting mechanic
    m_world.AddComponent<rtype::common::components::ChargedShot>(entity);
    
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
    
    // FireRate - Snake enemy shoots every 3.0 seconds
    // Random initial cooldown to stagger shots
    float randomCooldown = static_cast<float>(rand() % 1000) / 1000.0f * 3.0f;
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 3.0f);
    fireRate->cooldown = randomCooldown;
    
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

    // Add cyclic shield component (alternates on/off)
    m_world.AddComponent<rtype::common::components::ShieldComponent>(
        entity, rtype::common::components::ShieldType::Cyclic, true);

    // Add visual shield effect (blue pulsing circle)
    m_world.AddComponent<rtype::client::components::ShieldVisual>(
        entity,
        50.0f,                                      // radius
        sf::Color(100, 200, 255, 120),              // light blue, semi-transparent
        3.0f,                                       // pulse speed
        3.0f);                                      // border thickness

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

ECS::EntityID GameState::createTurretEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, 0.0f, 0.0f, 0.0f);  // Stationary
    m_world.AddComponent<rtype::common::components::Health>(entity, 1);  // 1 HP but shielded

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
        entity, rtype::common::components::EnemyType::Turret);

    // Add cyclic shield (requires charged shot like Shielded enemy)
    m_world.AddComponent<rtype::common::components::ShieldComponent>(
        entity, rtype::common::components::ShieldType::Cyclic, true);

    // Add shield visual effect
    m_world.AddComponent<rtype::client::components::ShieldVisual>(
        entity,
        50.0f,                                      // radius
        sf::Color(150, 150, 255, 120),              // purple-ish blue
        2.5f,                                       // pulse speed
        3.0f);                                      // border thickness

    // Turret fires 3-shot burst aimed at player every 2.5s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 2.5f);
    fireRate->cooldown = static_cast<float>(rand() % 1000) / 1000.0f * 2.5f;

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
    m_world.AddComponent<rtype::common::components::Velocity>(entity, 0.0f, 0.0f, 0.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 80);

    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::enemies::BOSS_ENEMY_2);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BOSS_ENEMY_2,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),
        6.5f);

    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);

    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Serpent);

    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 0.7f);
    fireRate->cooldown = 0.0f;

    return entity;
}

ECS::EntityID GameState::createFortressBoss(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, 0.0f, 0.0f, 0.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 100);

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

    // Add RED shield component (server-authoritative, client will sync)
    m_world.AddComponent<rtype::common::components::ShieldComponent>(
        entity, rtype::common::components::ShieldType::Red, true);

    // Add RED shield visual effect (invincible boss shield)
    m_world.AddComponent<rtype::client::components::ShieldVisual>(
        entity,
        120.0f,                                     // large radius for boss
        sf::Color(255, 50, 50, 150),                // RED, semi-transparent
        1.5f,                                       // slow pulse
        4.0f);                                      // thick border

    // Fortress fires randomly every 0.5s
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(entity, 0.5f);
    fireRate->cooldown = 0.0f;

    return entity;
}

ECS::EntityID GameState::createCoreBoss(float x, float y) {
    auto entity = m_world.CreateEntity();

    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    m_world.AddComponent<rtype::common::components::Velocity>(entity, 0.0f, 0.0f, 0.0f);
    m_world.AddComponent<rtype::common::components::Health>(entity, 150);

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

} // namespace rtype::client::gui
