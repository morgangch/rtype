/**
 * @file EntityFactories.h
 * @brief Entity factory functions for creating game entities
 * 
 * This file contains factory functions for creating all types of game entities
 * using the ECS architecture. Each factory creates an entity with the appropriate
 * components for player, enemies, and projectiles.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_ENTITY_FACTORIES_HPP
#define CLIENT_ENTITY_FACTORIES_HPP

#include <ECS/ECS.h>

namespace rtype::client::factories {
    
    /**
     * @brief Create a player entity
     * @param world Reference to the ECS world
     * @return EntityID of the created player
     * 
     * Creates player with:
     * - Position (100, screen_height/2)
     * - Velocity (0, 0) with max speed 300
     * - Health (3 HP)
     * - Sprite (32x32, green)
     * - Player component
     * - Team (Player)
     * - FireRate (0.2s cooldown)
     * - ChargedShot
     */
    ECS::EntityID createPlayer(ECS::World& world, float screenHeight);
    
    /**
     * @brief Create a basic enemy entity (red, shoots straight)
     * @param world Reference to the ECS world
     * @param x X coordinate
     * @param y Y coordinate
     * @return EntityID of the created enemy
     * 
     * Creates basic enemy with:
     * - Position (x, y)
     * - Velocity (-200, 0) - moves left
     * - Health (1 HP)
     * - Sprite (24x24, red)
     * - Team (Enemy)
     * - EnemyType (Basic)
     * - FireRate (2.0s cooldown)
     */
    ECS::EntityID createEnemy(ECS::World& world, float x, float y);
    
    /**
     * @brief Create a shooter enemy entity (orange, aims at player)
     * @param world Reference to the ECS world
     * @param x X coordinate
     * @param y Y coordinate
     * @return EntityID of the created shooter enemy
     * 
     * Creates shooter enemy with:
     * - Position (x, y)
     * - Velocity (-150, 0) - slower than basic
     * - Health (2 HP) - more durable
     * - Sprite (28x28, orange)
     * - Team (Enemy)
     * - EnemyType (Shooter)
     * - FireRate (1.5s cooldown)
     */
    ECS::EntityID createShooterEnemy(ECS::World& world, float x, float y);
    
    /**
     * @brief Create a boss enemy entity
     * @param world Reference to the ECS world
     * @param x X coordinate
     * @param y Y coordinate
     * @return EntityID of the created boss
     * 
     * Creates boss with:
     * - Position (x, y)
     * - Velocity (-100, 150) - slow horizontal, vertical bounce
     * - Health (20 HP)
     * - Sprite (80x80, magenta) - large
     * - Team (Enemy)
     * - EnemyType (Boss)
     * - FireRate (0.8s cooldown) - rapid fire
     */
    ECS::EntityID createBoss(ECS::World& world, float x, float y);
    
    /**
     * @brief Create a normal player projectile
     * @param world Reference to the ECS world
     * @param x X coordinate (player's position)
     * @param y Y coordinate (player's position)
     * @return EntityID of the created projectile
     * 
     * Creates projectile with:
     * - Position (x, y)
     * - Velocity (500, 0) - moves right
     * - Sprite (12x4, yellow)
     * - Team (Player)
     * - Projectile (1 damage, not piercing)
     */
    ECS::EntityID createPlayerProjectile(ECS::World& world, float x, float y);
    
    /**
     * @brief Create a charged player projectile (piercing)
     * @param world Reference to the ECS world
     * @param x X coordinate (player's position)
     * @param y Y coordinate (player's position)
     * @return EntityID of the created charged projectile
     * 
     * Creates charged projectile with:
     * - Position (x, y)
     * - Velocity (600, 0) - faster than normal
     * - Sprite (20x8, cyan) - larger and distinctive
     * - Team (Player)
     * - Projectile (2 damage, PIERCING)
     */
    ECS::EntityID createChargedProjectile(ECS::World& world, float x, float y);
    
    /**
     * @brief Create an enemy projectile
     * @param world Reference to the ECS world
     * @param x X coordinate (enemy's position)
     * @param y Y coordinate (enemy's position)
     * @param vx X velocity (direction and speed)
     * @param vy Y velocity (direction and speed)
     * @return EntityID of the created projectile
     * 
     * Creates projectile with:
     * - Position (x, y)
     * - Velocity (vx, vy) - custom direction
     * - Sprite (10x4, light red/pink)
     * - Team (Enemy)
     * - Projectile (1 damage)
     */
    ECS::EntityID createEnemyProjectile(ECS::World& world, float x, float y, 
                                        float vx = -300.0f, float vy = 0.0f);
    
} // namespace rtype::client::factories

#endif // CLIENT_ENTITY_FACTORIES_HPP
