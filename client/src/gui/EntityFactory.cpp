/**
 * @file EntityFactory.cpp
 * @brief Entity factory methods for creating game entities
 * 
 * This file contains all entity creation logic using ECS components.
 * Each factory method creates an entity with the appropriate components
 * for player, enemies, and projectiles.
 * 
 * Part of the modular GameState implementation.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include <cstdlib>
#include <iostream>

namespace rtype::client::gui {

// =============================================================================
// ENTITY FACTORIES
// =============================================================================

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
    
    // Sprite - 32x32 green rectangle
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity, 
        sf::Vector2f(32.0f, 32.0f),
        sf::Color::Green,
        true);
    
    // Player - Marks as player-controlled
    m_world.AddComponent<rtype::common::components::Player>(entity, "Player1", 0);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // FireRate - 0.2s cooldown between shots
    m_world.AddComponent<rtype::common::components::FireRate>(entity, FIRE_COOLDOWN);
    
    std::cout << "[EntityFactory] Created player entity: " << entity << "\n";
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
    
    // Sprite - 24x24 red rectangle
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        sf::Vector2f(24.0f, 24.0f),
        sf::Color::Red,
        true);
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // FireRate - Enemy shoots every 2.5 seconds
    // Random initial cooldown to stagger shots
    float randomCooldown = static_cast<float>(rand() % 1000) / 1000.0f * ENEMY_FIRE_INTERVAL;
    auto* fireRate = m_world.AddComponent<rtype::common::components::FireRate>(
        entity, ENEMY_FIRE_INTERVAL);
    fireRate->cooldown = randomCooldown;
    
    std::cout << "[EntityFactory] Created enemy entity: " << entity 
              << " at (" << x << ", " << y << ")\n";
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
    
    // Sprite - 12x4 yellow rectangle
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        sf::Vector2f(12.0f, 4.0f),
        sf::Color::Yellow,
        true);
    
    // Team - Player team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile - 1 damage
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1);
    
    std::cout << "[EntityFactory] Created player projectile: " << entity << "\n";
    return entity;
}

ECS::EntityID GameState::createEnemyProjectile(float x, float y) {
    auto entity = m_world.CreateEntity();
    
    // Position - Spawn at enemy's position
    m_world.AddComponent<rtype::common::components::Position>(
        entity, x - 12.0f, y, 0.0f); // -12 to spawn from left edge of enemy
    
    // Velocity - Moves left at 300 px/s
    m_world.AddComponent<rtype::common::components::Velocity>(
        entity, -300.0f, 0.0f, 300.0f);
    
    // Sprite - 10x4 red/pink rectangle
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        sf::Vector2f(10.0f, 4.0f),
        sf::Color(255, 100, 100), // Light red/pink
        true);
    
    // Team - Enemy team
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Enemy);
    
    // Projectile - 1 damage
    m_world.AddComponent<rtype::common::components::Projectile>(entity, 1);
    
    std::cout << "[EntityFactory] Created enemy projectile: " << entity << "\n";
    return entity;
}

} // namespace rtype::client::gui
