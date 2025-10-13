/**
 * @file HealthSystem.h
 * @brief Health management system for invulnerability timers
 * 
 * This system handles health-related logic like invulnerability timers.
 * Shared between client and server.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_HEALTH_SYSTEM_H
#define COMMON_SYSTEMS_HEALTH_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Health.h>

namespace rtype::common::systems {
    /**
     * @class HealthSystem
     * @brief Manages health and invulnerability timers
     */
    class HealthSystem {
    public:
        /**
         * @brief Update all health components (invulnerability timers)
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        static void update(ECS::World& world, float deltaTime) {
            auto* healthComponents = world.GetAllComponents<components::Health>();
            
            if (!healthComponents) return;
            
            for (auto& [entity, healthPtr] : *healthComponents) {
                auto& health = *healthPtr;
                
                // Update invulnerability timer
                if (health.invulnerable && health.invulnerabilityTimer > 0.0f) {
                    health.invulnerabilityTimer -= deltaTime;
                    if (health.invulnerabilityTimer <= 0.0f) {
                        health.invulnerabilityTimer = 0.0f;
                        health.invulnerable = false;
                    }
                }
                
                // Mark dead entities
                if (health.currentHp <= 0 && health.isAlive) {
                    health.currentHp = 0;
                    health.isAlive = false;
                }
            }
        }
    };
}

#endif // COMMON_SYSTEMS_HEALTH_SYSTEM_H
