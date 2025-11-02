/**
 * @file Homing.h
 * @brief Component for homing/tracking projectile mechanics
 * 
 * This component enables projectiles to track and follow targets.
 * Used by Azure Phantom's charged shot burst.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_HOMING_H
#define COMMON_COMPONENTS_HOMING_H

#include <ECS/ECS.h>

namespace rtype::common::components {

    /**
     * @class Homing
     * @brief Component for homing/tracking behavior
     * 
     * Projectiles with this component will automatically adjust their
     * trajectory to follow the nearest target.
     */
    class Homing : public ECS::Component<Homing> {
    public:
        /** @brief Current target entity ID (0 if no target) */
        ECS::EntityID targetId;
        
        /** @brief Maximum detection range for finding targets (pixels) */
        float detectionRange;
        
        /** @brief Turn rate in radians per second */
        float turnSpeed;
        
        /** @brief Maximum speed of the projectile */
        float maxSpeed;
        
        /** @brief Time between target re-acquisition (seconds) */
        float retargetInterval;
        
        /** @brief Time since last target search */
        float timeSinceRetarget;
        
        /**
         * @brief Constructor
         * @param range Detection range in pixels (default: 400.0f)
         * @param turn Turn speed in rad/s (default: 3.0f)
         * @param speed Max speed in px/s (default: 450.0f)
         * @param retarget Retarget interval in seconds (default: 0.5f)
         */
        Homing(float range = 400.0f, float turn = 3.0f, float speed = 450.0f, float retarget = 0.5f)
            : targetId(0),
              detectionRange(range),
              turnSpeed(turn),
              maxSpeed(speed),
              retargetInterval(retarget),
              timeSinceRetarget(0.0f) {}
        
        /**
         * @brief Update retarget timer
         * @param deltaTime Time elapsed since last frame
         * @return True if it's time to search for a new target
         */
        bool shouldRetarget(float deltaTime) {
            timeSinceRetarget += deltaTime;
            if (timeSinceRetarget >= retargetInterval) {
                timeSinceRetarget = 0.0f;
                return true;
            }
            return false;
        }
        
        /**
         * @brief Set a new target
         * @param target Entity ID of the target
         */
        void setTarget(ECS::EntityID target) {
            targetId = target;
        }
        
        /**
         * @brief Clear current target
         */
        void clearTarget() {
            targetId = 0;
        }
        
        /**
         * @brief Check if has valid target
         * @return True if targeting an entity
         */
        bool hasTarget() const {
            return targetId != 0;
        }
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_HOMING_H
