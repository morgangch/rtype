/**
 * @file ECSSystems.h
 * @brief ECS system declarations for game logic
 * 
 * This file contains declarations for all ECS systems that update game state.
 * Systems process entities with specific component combinations each frame.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_ECS_SYSTEMS_HPP
#define CLIENT_ECS_SYSTEMS_HPP

#include <ECS/ECS.h>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

// Forward declarations
namespace rtype::common::components {
    struct Position;
}

namespace rtype::client::systems {
    
    /**
     * @brief Movement System - Update entity positions based on velocity
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * @param screenHeight Height of the screen for boss bounce logic
     * 
     * Updates all entities with Position and Velocity components.
     * Handles boss vertical bouncing at screen edges.
     */
    void updateMovementSystem(ECS::World& world, float deltaTime, float screenHeight);
    
    /**
     * @brief Input System - Process player input and movement
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * @param keyUp True if up key is pressed
     * @param keyDown True if down key is pressed
     * @param keyLeft True if left key is pressed
     * @param keyRight True if right key is pressed
     * @param screenWidth Width of the screen for boundary clamping
     * @param screenHeight Height of the screen for boundary clamping
     * 
     * Applies velocity based on keyboard input.
     * Clamps player position to screen bounds.
     */
    void updateInputSystem(ECS::World& world, float deltaTime,
                          bool keyUp, bool keyDown, bool keyLeft, bool keyRight,
                          float screenWidth, float screenHeight);
    
    /**
     * @brief Fire Rate System - Update shooting cooldowns
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * 
     * Decreases cooldown timers for all entities with FireRate component.
     */
    void updateFireRateSystem(ECS::World& world, float deltaTime);
    
    /**
     * @brief Charged Shot System - Update charge accumulation
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * 
     * Updates charge time for all entities with ChargedShot component.
     */
    void updateChargedShotSystem(ECS::World& world, float deltaTime);
    
    /**
     * @brief Invulnerability System - Update invulnerability timers
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * 
     * Decreases invulnerability timers and disables when expired.
     */
    void updateInvulnerabilitySystem(ECS::World& world, float deltaTime);
    
    /**
     * @brief Enemy Spawn System - Spawn enemies periodically
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * @param enemySpawnTimer Current enemy spawn timer
     * @param bossSpawnTimer Current boss spawn timer
     * @param isBossActive Function to check if boss is alive
     * @param maxEnemies Maximum number of enemies allowed
     * @param enemySpawnInterval Time between enemy spawns
     * @param bossSpawnInterval Time between boss spawns
     * @param screenWidth Width of screen for spawn position
     * @param screenHeight Height of screen for spawn position
     * 
     * Spawns basic/shooter enemies at intervals.
     * Spawns boss every 3 minutes if not already active.
     */
    void updateEnemySpawnSystem(ECS::World& world, float deltaTime,
                               float& enemySpawnTimer, float& bossSpawnTimer,
                               std::function<bool()> isBossActive,
                               size_t maxEnemies, float enemySpawnInterval,
                               float bossSpawnInterval,
                               float screenWidth, float screenHeight);
    
    /**
     * @brief Enemy AI System - Enemy shooting logic
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * @param createEnemyProjectile Function to create enemy projectiles
     * 
     * Handles three enemy types:
     * - Basic: Shoots straight left
     * - Shooter: Aims at player position
     * - Boss: Shoots 3-projectile spread pattern
     */
    void updateEnemyAISystem(ECS::World& world, float deltaTime,
                            std::function<ECS::EntityID(float, float, float, float)> createEnemyProjectile);
    
    /**
     * @brief Cleanup System - Remove off-screen entities
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last frame
     * @param playerEntity ID of the player entity (excluded from cleanup)
     * @param screenWidth Width of screen for boundary checking
     * 
     * Destroys entities that moved too far off-screen.
     */
    void updateCleanupSystem(ECS::World& world, float deltaTime,
                            ECS::EntityID playerEntity, float screenWidth);
    
} // namespace rtype::client::systems

#endif // CLIENT_ECS_SYSTEMS_HPP
