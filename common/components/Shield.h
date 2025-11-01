/**
 * @file Shield.h
 * @brief Component for shield/damage reduction mechanics
 * 
 * This component provides temporary invulnerability or damage reduction.
 * Used by Solar Guardian's charged shot ability.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_SHIELD_H
#define COMMON_COMPONENTS_SHIELD_H

#include <ECS/ECS.h>
#include <cmath>

namespace rtype::common::components {

    /**
     * @class Shield
     * @brief Component for shield/barrier mechanics
     * 
     * Entities with this component have a protective shield that
     * reduces or blocks damage for a limited time.
     */
    class Shield : public ECS::Component<Shield> {
    public:
        /** @brief Whether the shield is currently active */
        bool isActive;
        
        /** @brief Duration of the shield in seconds */
        float duration;
        
        /** @brief Time remaining on the shield */
        float timeRemaining;
        
        /** @brief Damage reduction percentage (1.0 = 100% = invulnerable) */
        float damageReduction;
        
        /** @brief Cooldown time before shield can be used again */
        float cooldownDuration;
        
        /** @brief Time remaining on cooldown */
        float cooldownRemaining;
        
        /** @brief Visual scale for shield effect (pulses) */
        float visualScale;
        
        /**
         * @brief Constructor
         * @param shieldDuration Duration in seconds (default: 3.0f)
         * @param reduction Damage reduction 0.0-1.0 (default: 1.0 = 100%)
         * @param cooldown Cooldown in seconds (default: 6.0f)
         */
        Shield(float shieldDuration = 3.0f, float reduction = 1.0f, float cooldown = 6.0f)
            : isActive(false),
              duration(shieldDuration),
              timeRemaining(0.0f),
              damageReduction(reduction),
              cooldownDuration(cooldown),
              cooldownRemaining(0.0f),
              visualScale(1.0f) {}
        
        /**
         * @brief Update shield and cooldown timers
         * @param deltaTime Time elapsed since last frame
         */
        void update(float deltaTime) {
            if (isActive) {
                timeRemaining -= deltaTime;
                if (timeRemaining <= 0.0f) {
                    deactivate();
                }
                
                // Pulse effect for visual
                visualScale = 1.0f + 0.1f * std::sin(timeRemaining * 10.0f);
            } else if (cooldownRemaining > 0.0f) {
                cooldownRemaining -= deltaTime;
                if (cooldownRemaining < 0.0f) {
                    cooldownRemaining = 0.0f;
                }
            }
        }
        
        /**
         * @brief Activate the shield
         * @return True if shield was activated, false if on cooldown
         */
        bool activate() {
            if (cooldownRemaining > 0.0f || isActive) {
                return false;
            }
            
            isActive = true;
            timeRemaining = duration;
            visualScale = 1.0f;
            return true;
        }
        
        /**
         * @brief Deactivate the shield and start cooldown
         */
        void deactivate() {
            isActive = false;
            timeRemaining = 0.0f;
            cooldownRemaining = cooldownDuration;
        }
        
        /**
         * @brief Check if shield can be activated
         * @return True if not on cooldown and not already active
         */
        bool canActivate() const {
            return cooldownRemaining <= 0.0f && !isActive;
        }
        
        /**
         * @brief Calculate damage after shield reduction
         * @param incomingDamage Original damage amount
         * @return Damage after reduction (0 if fully blocked)
         */
        int applyDamageReduction(int incomingDamage) const {
            if (!isActive) return incomingDamage;
            
            float reducedDamage = incomingDamage * (1.0f - damageReduction);
            return static_cast<int>(reducedDamage);
        }
        
        /**
         * @brief Get shield strength percentage
         * @return 0.0-1.0 based on time remaining
         */
        float getStrength() const {
            if (!isActive) return 0.0f;
            return timeRemaining / duration;
        }
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_SHIELD_H
