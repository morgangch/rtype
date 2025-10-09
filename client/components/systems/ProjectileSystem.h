/**
 * @file ProjectileSystem.h
 * @brief System for managing projectile lifecycle and spawning
 * 
 * This system handles projectile firing, movement bounds checking,
 * and automatic cleanup of off-screen projectiles.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_PROJECTILE_SYSTEM_H
#define CLIENT_SYSTEMS_PROJECTILE_SYSTEM_H

#include <ECS/ECS.hpp>
#include <SFML/Graphics.hpp>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Projectile.h>
#include <common/components/Team.h>
#include <client/components/Sprite.h>
#include <client/components/FireRate.h>
#include <client/components/Controllable.h>
#include <client/components/systems/InputSystem.h>
#include <vector>

namespace rtype::client::systems {
    /**
     * @class ProjectileSystem
     * @brief Manages projectile spawning and cleanup
     * 
     * The ProjectileSystem handles:
     * - Player projectile spawning based on input
     * - Enemy projectile spawning (AI-controlled)
     * - Off-screen projectile cleanup
     * - FireRate cooldown updates
     * 
     * Required Components (for shooters):
     * - Position: Spawn location
     * - FireRate: Cooldown management
     * - Team: Determines projectile team
     */
    class ProjectileSystem {
    public:
        /** @brief Screen width for bounds checking */
        float screenWidth;
        
        /** @brief Screen height for bounds checking */
        float screenHeight;
        
        /** @brief Offset for enemy projectile spawn */
        static constexpr float ENEMY_PROJECTILE_SPAWN_OFFSET = 12.0f;
        
        /**
         * @brief Constructor
         * @param width Screen width
         * @param height Screen height
         */
        ProjectileSystem(float width = 1280.0f, float height = 720.0f)
            : screenWidth(width), screenHeight(height) {}
        
        /**
         * @brief Spawn a player projectile
         * @param world ECS world
         * @param position Spawn position
         * @param team Team identifier
         */
        void spawnPlayerProjectile(ECS::World& world, const sf::Vector2f& position, rtype::common::components::TeamType team = rtype::common::components::TeamType::Player) {
            auto projectile = world.CreateEntity();
            
            // Position (right edge of player)
            world.AddComponent<rtype::common::components::Position>(
                projectile, position.x, position.y, 0.0f);
            
            // Velocity (move right at 500 px/s)
            world.AddComponent<rtype::common::components::Velocity>(
                projectile, 500.0f, 0.0f, 500.0f);
            
            // Sprite (yellow, small rectangle)
            world.AddComponent<rtype::client::components::Sprite>(
                projectile, sf::Vector2f(12.0f, 4.0f), sf::Color::Yellow);
            
            // Projectile data
            world.AddComponent<rtype::common::components::Projectile>(projectile, 1);
            
            // Team
            world.AddComponent<rtype::common::components::Team>(projectile, team);
        }
        
        /**
         * @brief Spawn an enemy projectile
         * @param world ECS world
         * @param position Spawn position
         * @param team Team identifier
         */
        void spawnEnemyProjectile(ECS::World& world, const sf::Vector2f& position, rtype::common::components::TeamType team = rtype::common::components::TeamType::Enemy) {
            auto projectile = world.CreateEntity();
            
            // Position (left of enemy)
            world.AddComponent<rtype::common::components::Position>(
                projectile, 
                position.x - ENEMY_PROJECTILE_SPAWN_OFFSET, 
                position.y, 
                0.0f);
            
            // Velocity (move left at 300 px/s)
            world.AddComponent<rtype::common::components::Velocity>(
                projectile, -300.0f, 0.0f, 300.0f);
            
            // Sprite (red, small rectangle)
            world.AddComponent<rtype::client::components::Sprite>(
                projectile, sf::Vector2f(10.0f, 4.0f), sf::Color(255, 100, 100));
            
            // Projectile data
            world.AddComponent<rtype::common::components::Projectile>(projectile, 1);
            
            // Team
            world.AddComponent<rtype::common::components::Team>(projectile, team);
        }
        
        /**
         * @brief Update projectile system - cleanup off-screen projectiles
         * @param world ECS world
         * @param deltaTime Time elapsed since last update
         * @param destroyCallback Callback to destroy entities (EntityID)
         */
        template<typename DestroyFunc>
        void update(ECS::World& world, float deltaTime, DestroyFunc destroyCallback) {
            // Cleanup off-screen projectiles
            auto* positions = world.GetAllComponents<rtype::common::components::Position>();
            if (!positions) return;
            
            std::vector<ECS::EntityID> toDestroy;
            for (auto& [entity, posPtr] : *positions) {
                // Check if this entity is a projectile
                auto* proj = world.GetComponent<rtype::common::components::Projectile>(entity);
                if (!proj) continue;
                
                auto& pos = *posPtr;
                
                // Remove if off-screen
                if (pos.x < -50.0f || pos.x > screenWidth + 50.0f ||
                    pos.y < -50.0f || pos.y > screenHeight + 50.0f) {
                    toDestroy.push_back(entity);
                }
            }
            
            for (auto entity : toDestroy) {
                destroyCallback(entity);
            }
        }
    };
}

#endif // CLIENT_SYSTEMS_PROJECTILE_SYSTEM_H
