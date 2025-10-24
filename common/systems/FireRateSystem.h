/**
 * @file FireRateSystem.h
 * @brief System for managing weapon fire rate cooldowns
 * 
 * This system updates fire rate cooldown timers for all entities
 * with a FireRate component.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H
#define COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/FireRate.h>


namespace rtype::common::systems {
    /**
     * @class FireRateSystem
     * @brief Manages weapon cooldown timers
     * 
     * This system updates the cooldown timer for all entities
     * with a FireRate component, allowing them to shoot again
     * once the cooldown has elapsed.
     */
    class FireRateSystem : public ECS::System {
    public:
        /**
         * @brief Constructor
         */
        FireRateSystem() : ECS::System("FireRateSystem", 15) {}

        /**
         * @brief Update all fire rate components
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            auto* fireRates = world.GetAllComponents<components::FireRate>();
            
            if (!fireRates)
                return;
            
            for (auto& [entity, fireRatePtr] : *fireRates) {
                // Use the component's built-in update method
                fireRatePtr->update(deltaTime);
            }
        }

        /**
         * @brief Initialize the system
         * @param world The ECS world
         */
        void Initialize(ECS::World& world) override {
            // Optional: Add initialization logic
        }

        /**
         * @brief Cleanup the system
         * @param world The ECS world
         */
        void Cleanup(ECS::World& world) override {
            // Optional: Add cleanup logic
        }
    };
}

#endif // COMMON_SYSTEMS_FIRE_RATE_SYSTEM_H
