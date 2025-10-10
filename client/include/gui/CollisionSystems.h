/**
 * @file CollisionSystems.h
 * @brief Collision detection systems for game entities
 * 
 * This file contains collision detection subsystems that check for
 * AABB (Axis-Aligned Bounding Box) collisions between different entity types.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_COLLISION_SYSTEMS_HPP
#define CLIENT_COLLISION_SYSTEMS_HPP

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

// Forward declarations
namespace rtype::common::components {
    struct Position;
}

namespace rtype::client::collision {
    
    /**
     * @brief Check player vs enemies collision
     * @param world Reference to the ECS world
     * @param positions Map of all entity positions
     * @param getBounds Lambda to get entity bounding box
     * @param playerEntity ID of the player entity
     * @param damagePlayer Function to damage the player
     * 
     * Detects collisions between player and enemies.
     * Applies 1 damage to player on contact.
     * Respects player invulnerability state.
     */
    void checkPlayerVsEnemiesCollision(
        ECS::World& world,
        ECS::ComponentArray<rtype::common::components::Position>& positions,
        const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
        ECS::EntityID playerEntity,
        std::function<void(int)> damagePlayer);
    
    /**
     * @brief Check player projectiles vs enemies collision
     * @param world Reference to the ECS world
     * @param positions Map of all entity positions
     * @param getBounds Lambda to get entity bounding box
     * @param toDestroy Vector to collect entities to destroy
     * 
     * Detects collisions between player projectiles and enemies.
     * Applies damage to enemies.
     * Normal projectiles are destroyed on impact.
     * Piercing projectiles continue through enemies.
     */
    void checkPlayerProjectilesVsEnemiesCollision(
        ECS::World& world,
        ECS::ComponentArray<rtype::common::components::Position>& positions,
        const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
        std::vector<ECS::EntityID>& toDestroy);
    
    /**
     * @brief Check enemy projectiles vs player collision
     * @param world Reference to the ECS world
     * @param positions Map of all entity positions
     * @param getBounds Lambda to get entity bounding box
     * @param playerEntity ID of the player entity
     * @param toDestroy Vector to collect entities to destroy
     * @param damagePlayer Function to damage the player
     * 
     * Detects collisions between enemy projectiles and player.
     * Applies damage to player based on projectile damage value.
     * Destroys projectiles on impact.
     * Respects player invulnerability state.
     */
    void checkEnemyProjectilesVsPlayerCollision(
        ECS::World& world,
        ECS::ComponentArray<rtype::common::components::Position>& positions,
        const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
        ECS::EntityID playerEntity,
        std::vector<ECS::EntityID>& toDestroy,
        std::function<void(int)> damagePlayer);
    
    /**
     * @brief Main collision system orchestrator
     * @param world Reference to the ECS world
     * @param playerEntity ID of the player entity
     * @param damagePlayer Function to damage the player
     * 
     * Orchestrates all collision detection subsystems:
     * 1. Player vs Enemies
     * 2. Player Projectiles vs Enemies
     * 3. Enemy Projectiles vs Player
     * 
     * Collects all entities to destroy and processes destruction at the end.
     */
    void updateCollisionSystem(ECS::World& world,
                               ECS::EntityID playerEntity,
                               std::function<void(int)> damagePlayer);
    
} // namespace rtype::client::collision

#endif // CLIENT_COLLISION_SYSTEMS_HPP
