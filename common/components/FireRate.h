/**
 * @file FireRate.h
 * @brief FireRate component for shooting cooldown management
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_FIRERATE_H
#define COMMON_COMPONENTS_FIRERATE_H

#include <ECS/ECS.h>

namespace rtype::common::components {
    /**
     * @class FireRate
     * @brief Manages shooting cooldown for entities
     * 
     * This component tracks the time between shots and enforces
     * a minimum interval to prevent rapid-fire abuse.
     */
    class FireRate : public ECS::Component<FireRate> {
    public:
        /** @brief Cooldown interval in seconds */
        float interval;
        
        /** @brief Current cooldown timer */
        float cooldown;
        
        /**
         * @brief Construct a new FireRate component
         * @param interval Time between shots in seconds
         */
        FireRate(float interval = 0.5f) 
            : interval(interval), cooldown(0.0f) {}
        
        /**
         * @brief Update the cooldown timer
         * @param deltaTime Time elapsed since last frame
         */
        void update(float deltaTime) {
            if (cooldown > 0.0f) {
                cooldown -= deltaTime;
                if (cooldown < 0.0f) cooldown = 0.0f;
            }
        }
        
        /**
         * @brief Check if entity can fire
         * @return True if cooldown is ready
         */
        bool canFire() const {
            return cooldown <= 0.0f;
        }
        
        /**
         * @brief Reset cooldown after firing
         */
        void shoot() {
            cooldown = interval;
        }
    };
}

#endif // COMMON_COMPONENTS_FIRERATE_H
