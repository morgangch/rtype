/**
 * @file ChargedShot.h
 * @brief Charged shot system for player weapons
 * 
 * This system manages the charging and firing of charged shots.
 * It tracks charge time and fires when the player releases the fire button.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_CHARGED_SHOT_H
#define COMMON_SYSTEMS_CHARGED_SHOT_H

#include <ECS/ECS.h>
#include <common/components/ChargedShot.h>


namespace rtype::common::systems {
    /**
     * @class ChargedShotSystem
     * @brief Manages charged shot mechanics
     * 
     * This system updates the charge timer for all entities with
     * a ChargedShot component, allowing them to build up power
     * for more powerful projectiles.
     */
    class ChargedShotSystem : public ECS::System {
    private:
        float m_chargeRate;  ///< Rate at which charge builds up

    public:
        /**
         * @brief Constructor
         * @param chargeRate How fast charge builds (default: 1.0)
         */
        explicit ChargedShotSystem(float chargeRate = 1.0f) 
            : ECS::System("ChargedShotSystem", 15), m_chargeRate(chargeRate) {}

        /**
         * @brief Update all charged shot components
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            auto* chargedShots = world.GetAllComponents<components::ChargedShot>();
            
            if (!chargedShots)
                return;
            
            for (auto& [entity, chargedPtr] : *chargedShots) {
                auto& charged = *chargedPtr;
                
                // Update charge timer if charging
                if (charged.isCharging) {
                    charged.chargeTimer += deltaTime * m_chargeRate;
                    
                    // Clamp to max charge time
                    if (charged.chargeTimer >= charged.maxChargeTime) {
                        charged.chargeTimer = charged.maxChargeTime;
                        charged.isFullyCharged = true;
                    }
                } else {
                    // Reset charge when not charging
                    charged.chargeTimer = 0.0f;
                    charged.isFullyCharged = false;
                }
            }
        }

        /**
         * @brief Set the charge rate
         * @param rate New charge rate
         */
        void SetChargeRate(float rate) {
            m_chargeRate = rate;
        }

        /**
         * @brief Get the current charge rate
         * @return Current charge rate
         */
        float GetChargeRate() const {
            return m_chargeRate;
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

#endif // COMMON_SYSTEMS_CHARGED_SHOT_H
