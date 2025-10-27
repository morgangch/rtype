/**
 * @file ChargedShot.h
 * @brief Component for charged shot mechanic
 * 
 * This component tracks the charge state and timing for weapons
 * that can be charged up for more powerful shots.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_CHARGED_SHOT_H
#define COMMON_COMPONENTS_CHARGED_SHOT_H

#include <ECS/ECS.h>

namespace rtype::common::components {
    /**
     * @class ChargedShot
     * @brief Component for managing charged shot mechanics
     * 
     * This component allows entities to charge up their shots
     * for more powerful attacks. It tracks:
     * - Charge state (charging or not)
     * - Charge timer (how long has been charging)
     * - Fully charged state
     * 
     * Usage:
     * - Call startCharge() when fire button is pressed
     * - Call release() when fire button is released
     * - Check isFullyCharged to determine shot power
     */
    class ChargedShot : public ECS::Component<ChargedShot> {
    public:
        /** @brief Whether currently charging */
        bool isCharging;
        
        /** @brief Current charge time in seconds */
        float chargeTimer;
        
        /** @brief Time required for full charge (seconds) */
        float maxChargeTime;
        
        /** @brief Whether charge is at maximum */
        bool isFullyCharged;
        
        /**
         * @brief Constructor
         * @param maxTime Maximum charge time in seconds (default: 1.5s)
         */
        ChargedShot(float maxTime = 1.5f)
            : isCharging(false),
              chargeTimer(0.0f),
              maxChargeTime(maxTime),
              isFullyCharged(false) {}
        
        /**
         * @brief Start charging
         */
        void startCharge() {
            isCharging = true;
            chargeTimer = 0.0f;
            isFullyCharged = false;
        }
        
        /**
         * @brief Release the charge and fire
         * @return True if shot was fully charged
         */
        bool release() {
            bool wasFull = isFullyCharged;
            isCharging = false;
            chargeTimer = 0.0f;
            isFullyCharged = false;
            return wasFull;
        }
        
        /**
         * @brief Update charge timer (called by ChargedShotSystem)
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) {
            if (isCharging) {
                chargeTimer += deltaTime;
                if (chargeTimer >= maxChargeTime) {
                    chargeTimer = maxChargeTime;
                    isFullyCharged = true;
                }
            }
        }
        
        /**
         * @brief Get charge progress as percentage (0.0 to 1.0)
         * @return Charge percentage
         */
        float getChargePercentage() const {
            if (maxChargeTime <= 0.0f) return 0.0f;
            return chargeTimer / maxChargeTime;
        }
    };
}

#endif // COMMON_COMPONENTS_CHARGED_SHOT_H
