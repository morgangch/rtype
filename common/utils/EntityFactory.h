/**
 * @file EntityFactory.h
 * @brief Factory functions for creating common game entities
 * 
 * This provides modular helper functions to create entities with common
 * component combinations. Can be used by both client and server.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_ENTITY_FACTORY_H
#define COMMON_ENTITY_FACTORY_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>

namespace rtype::common::factory {
    /**
     * @brief Create a basic entity with position and velocity
     * @param world ECS world
     * @param x Initial X position
     * @param y Initial Y position
     * @param vx Initial X velocity
     * @param vy Initial Y velocity
     * @param maxSpeed Maximum speed (0 = unlimited)
     * @return EntityID of created entity
     */
    inline ECS::EntityID createMovingEntity(
        ECS::World& world,
        float x, float y,
        float vx, float vy,
        float maxSpeed = 0.0f
    ) {
        auto entity = world.CreateEntity();
        world.AddComponent<components::Position>(entity, x, y, 0.0f);
        world.AddComponent<components::Velocity>(entity, vx, vy, maxSpeed);
        return entity;
    }
    
    /**
     * @brief Create an entity with position, velocity, and health
     * @param world ECS world
     * @param x Initial X position
     * @param y Initial Y position
     * @param vx Initial X velocity
     * @param vy Initial Y velocity
     * @param maxSpeed Maximum speed
     * @param health Initial health points
     * @return EntityID of created entity
     */
    inline ECS::EntityID createLivingEntity(
        ECS::World& world,
        float x, float y,
        float vx, float vy,
        float maxSpeed,
        int health
    ) {
        auto entity = createMovingEntity(world, x, y, vx, vy, maxSpeed);
        world.AddComponent<components::Health>(entity, health);
        return entity;
    }
    
    /**
     * @brief Create a player entity (common components only)
     * @param world ECS world
     * @param x Initial X position
     * @param y Initial Y position
     * @param lives Number of lives/health
     * @return EntityID of created player
     * 
     * Note: Client-specific components (rendering, input) should be added separately
     */
    inline ECS::EntityID createPlayer(
        ECS::World& world,
        float x = 100.0f,
        float y = 360.0f,
        int lives = 3
    ) {
        auto entity = world.CreateEntity();
        world.AddComponent<components::Position>(entity, x, y, 0.0f);
        world.AddComponent<components::Velocity>(entity, 0.0f, 0.0f, 300.0f);
        world.AddComponent<components::Health>(entity, lives);
        return entity;
    }
    
    /**
     * @brief Create an enemy entity (common components only)
     * @param world ECS world
     * @param x Initial X position
     * @param y Initial Y position
     * @return EntityID of created enemy
     */
    inline ECS::EntityID createEnemy(
        ECS::World& world,
        float x = 1280.0f,
        float y = 360.0f
    ) {
        auto entity = world.CreateEntity();
        world.AddComponent<components::Position>(entity, x, y, 0.0f);
        world.AddComponent<components::Velocity>(entity, -100.0f, 0.0f, 100.0f);
        world.AddComponent<components::Health>(entity, 1);
        return entity;
    }
    
    /**
     * @brief Create a projectile entity (common components only)
     * @param world ECS world
     * @param x Initial X position
     * @param y Initial Y position
     * @param vx X velocity
     * @param speed Speed of projectile
     * @return EntityID of created projectile
     */
    inline ECS::EntityID createProjectile(
        ECS::World& world,
        float x, float y,
        float vx,
        float speed = 500.0f
    ) {
        auto entity = world.CreateEntity();
        world.AddComponent<components::Position>(entity, x, y, 0.0f);
        world.AddComponent<components::Velocity>(entity, vx, 0.0f, speed);
        return entity;
    }
}

#endif // COMMON_ENTITY_FACTORY_H
