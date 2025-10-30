/**
 * @file FireRateSystem.h
 * @brief Fire rate cooldown system for updating shooting timers
 *
 * This system is shared between client and server for consistent firing behavior.
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H
#define COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/FireRate.h>
#include <iostream>

namespace rtype::common::systems {
    /**
     * @class FireRateSystem
     * @brief Updates fire rate cooldowns for all entities
     *
     * This system is used by both client (for prediction) and server (authoritative).
     * Decrements the cooldown timer for all FireRate components.
     *
     * CRITICAL: This fixes the server bug where cooldowns were never updated,
     * causing enemies to only shoot once.
     */
    class FireRateSystem {
    public:
        /**
         * @brief Update all entities with FireRate components
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         *
         * Decrements cooldown timer for each FireRate component.
         */
        static void update(ECS::World& world, float deltaTime) {
            auto* fireRates = world.GetAllComponents<components::FireRate>();
            if (!fireRates) return;

            for (auto& [entity, fireRatePtr] : *fireRates) {
                fireRatePtr->update(deltaTime);
            }
        }
    };
}

#endif // COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H
