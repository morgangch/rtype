/**
 * @file TimerSystem.h
 * @brief System for updating time-based components
 * 
 * This system updates all timer-based components like invulnerability,
 * fire rate cooldowns, etc.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_TIMER_SYSTEM_H
#define CLIENT_SYSTEMS_TIMER_SYSTEM_H

#include <ECS/ECS.h>
#include <client/components/Invulnerability.h>

namespace rtype::client::systems {
    /**
     * @class TimerSystem
     * @brief Updates all time-based components
     * 
     * The TimerSystem handles countdown timers for:
     * - Invulnerability duration
     * - Any other time-based effects
     * 
     * This keeps timer logic centralized and consistent.
     */
    class TimerSystem {
    public:
        /**
         * @brief Update all timers
         * @param world ECS world
         * @param deltaTime Time elapsed since last update
         */
        void update(ECS::World& world, float deltaTime) {
            // Update invulnerability timers
            auto* invulnComponents = world.GetAllComponents<rtype::client::components::Invulnerability>();
            if (!invulnComponents) return;
            
            for (auto& [entity, invulnPtr] : *invulnComponents) {
                invulnPtr->update(deltaTime);
            }
        }
    };
}

#endif // CLIENT_SYSTEMS_TIMER_SYSTEM_H
