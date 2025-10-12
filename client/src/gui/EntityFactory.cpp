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
#include <cstdlib>
#include <cmath>

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

ECS::EntityID GameState::createShooterEnemy(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x, y, 0.0f);
    
    // Velocity - Moves left at 80 px/s (slower than basic enemy)
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, -80.0f, 0.0f, 80.0f);
    
    // Health - 2 HP (more resistant)
    m_world.AddComponent<rtype::common::components::Health>(entity, 2);
    
    // Sprite - Shooter enemy with texture (first frame from BASICENEMY_2)
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
    
    // EnemyType - Shooter enemy (actively shoots)
    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Shooter);
    
    // FireRate - Shooter enemy shoots every 1.5 seconds (faster than basic)
    // Random initial cooldown to stagger shots
    const float SHOOTER_FIRE_INTERVAL = 1.5f;
    float randomCooldown = static_cast<float>(rand() % 1000) / 1000.0f * SHOOTER_FIRE_INTERVAL;
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(
        entity, SHOOTER_FIRE_INTERVAL);
    fireRate->cooldown = randomCooldown;
    
    return entity;
}

ECS::EntityID GameState::createBoss(float x, float y) {
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
    
    // Sprite - Boss enemy with texture (first frame from BASICENEMY_4)
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        rtype::client::assets::enemies::BASIC_ENEMY_4,
        sf::Vector2f(33.0f, 36.0f),
        true,
        sf::IntRect(0, 0, 33, 36),  // First frame
        5.0f);  // Scale 5x for boss (33*5 = 165 pixels - LARGE!)
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // EnemyType - Boss (shoots in spread pattern)
    m_world.AddComponent<rtype::common::components::EnemyTypeComponent>(
        entity, rtype::common::components::EnemyType::Boss);
    
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
    
    // Sprite - Simple yellow rectangle for player projectile
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        sf::Vector2f(20.0f, 5.0f),
        sf::Color::Yellow);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile - 1 damage
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1);
    
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
    
    // Sprite - Simple red rectangle for enemy projectile
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        sf::Vector2f(15.0f, 5.0f),
        sf::Color::Red);
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // Projectile - 1 damage
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1, 300.0f, 
        rtype::common::components::ProjectileType::Basic, false);
    
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
    
    // Sprite - Simple cyan rectangle for charged projectile (bigger)
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity, 
        sf::Vector2f(30.0f, 8.0f),
        sf::Color::Cyan);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile - 2 damage, PIERCING
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 2, 600.0f, 
        rtype::common::components::ProjectileType::Piercing, true);
    
    return entity;
}

} // namespace rtype::client::gui
