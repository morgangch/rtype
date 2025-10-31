/**
 * @file EnemyAISystem.h
 * @brief AI system for enemy shooting behavior (serveur)
 * 
 * Ce système gère les patterns de tir des ennemis selon leur type.
 * Différents types d'ennemis ont des comportements de tir différents.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H
#define SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Team.h>
#include <common/components/Player.h>
#include <common/components/Health.h>
#include <common/components/FireRate.h>
#include <common/components/EnemyType.h>

#include <functional>
#include <cmath>

namespace rtype::server::systems {

/**
 * @class EnemyAISystem
 * @brief System responsible for enemy shooting AI
 *
 * Handles enemy shooting patterns based on enemy type (basic, shooter, boss).
 * Requires a callback to create projectiles.
 */
class EnemyAISystem : public ECS::System {
public:
    /**
     * @brief Callback type for projectile creation
     * Parameters: shooterEntity, x, y, vx, vy, world
     */
    using ProjectileCallback = std::function<void(ECS::EntityID, float, float, float, float, ECS::World&)>;
private:
    ProjectileCallback m_createProjectile;
public:
    /**
     * @brief Constructor for EnemyAISystem
     * Sets system name and priority.
     */
    EnemyAISystem();

    /**
     * @brief Sets the projectile creation callback
     * @param callback Function to create enemy projectiles
     */
    void SetProjectileCallback(ProjectileCallback callback);

    /**
     * @brief Main update loop for enemy AI
     * @param world Reference to the ECS world
     * @param deltaTime Time elapsed since last update
     * Handles shooting logic for all enemy types.
     */
    void Update(ECS::World& world, float deltaTime) override;

    /**
     * @brief Initializes the system (optional)
     * @param world Reference to the ECS world
     */
    void Initialize(ECS::World& world) override;

    /**
     * @brief Cleans up the system
     * @param world Reference to the ECS world
     */
    void Cleanup(ECS::World& world) override;

    // Note: Shooting logic now delegated to common/systems/EnemyAISystem
    // This wrapper system calls the shared implementation for consistency
};
}

#endif // SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H
