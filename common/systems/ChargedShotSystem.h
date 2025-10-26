/**
 * @file ChargedShot.h
 * @brief Charged shot component for piercing projectiles
 * 
 * This component tracks charging time for powerful shots.
 * Charged shots deal more damage and can pierce through enemies.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_CHARGED_SHOT_H
#define COMMON_COMPONENTS_CHARGED_SHOT_H

namespace rtype::common::components {
    
    /**
     * @struct ChargedShot
     * @brief Component for charged shot state tracking
     */
    struct ChargedShot {
        float chargeTime;           ///< Current charge time accumulated
        float maxChargeTime;        ///< Time needed for full charge (default 1.0s)
        bool isCharging;            ///< Whether player is currently holding fire button
        bool isFullyCharged;        ///< Whether shot is fully charged and ready
        
        ChargedShot(float maxCharge = 1.0f) 
            : chargeTime(0.0f)
            , maxChargeTime(maxCharge)
            , isCharging(false)
            , isFullyCharged(false) {}
        
        /**
         * @brief Update charge state
         * @param deltaTime Time elapsed since last frame
         */
        void update(float deltaTime) {
            if (isCharging) {
                chargeTime += deltaTime;
                if (chargeTime >= maxChargeTime) {
                    chargeTime = maxChargeTime;
                    isFullyCharged = true;
                }
            }
        }
        
        /**
         * @brief Start charging
         */
        void startCharge() {
            isCharging = true;
        }
        
        /**
         * @brief Release charge and reset
         * @return true if shot was fully charged when released
         */
        bool release() {
            bool wasCharged = isFullyCharged;
            chargeTime = 0.0f;
            isCharging = false;
            isFullyCharged = false;
            return wasCharged;
        }
        
        /**
         * @brief Get charge percentage (0.0 to 1.0)
         */
        float getChargePercent() const {
            return chargeTime / maxChargeTime;
        }
    };
    
} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_CHARGED_SHOT_H
