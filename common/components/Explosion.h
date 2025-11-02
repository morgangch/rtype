/**
 * @file Explosion.h
 * @brief Component for Area of Effect (AoE) explosion mechanics
 * 
 * This component marks a projectile or entity as having explosive properties.
 * When it impacts or is destroyed, it deals damage in an area.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_EXPLOSION_H
#define COMMON_COMPONENTS_EXPLOSION_H

#include <ECS/ECS.h>

namespace rtype::common::components {

    /**
     * @class Explosion
     * @brief Component for explosion/AoE damage mechanics
     * 
     * Entities with this component will trigger an AoE explosion on impact.
     * Used by Emerald Titan's explosive projectiles.
     */
    class Explosion : public ECS::Component<Explosion> {
    public:
        /** @brief Radius of the explosion in pixels */
        float radius;
        
        /** @brief Damage at the center of the explosion */
        int centerDamage;
        
        /** @brief Damage at the edge of the explosion */
        int edgeDamage;
        
        /** @brief Whether the explosion has been triggered */
        bool triggered;
        
        /** @brief Time the explosion remains active (for visual effects) */
        float duration;
        
        /** @brief Current lifetime of the explosion */
        float lifetime;
        
        /**
         * @brief Constructor
         * @param explosionRadius Radius in pixels (default: 50.0f)
         * @param centerDmg Damage at center (default: 2)
         * @param edgeDmg Damage at edge (default: 1)
         * @param explosionDuration Duration in seconds (default: 0.3f)
         */
        Explosion(float explosionRadius = 50.0f, int centerDmg = 2, int edgeDmg = 1, float explosionDuration = 0.3f)
            : radius(explosionRadius),
              centerDamage(centerDmg),
              edgeDamage(edgeDmg),
              triggered(false),
              duration(explosionDuration),
              lifetime(0.0f) {}
        
        /**
         * @brief Update explosion lifetime
         * @param deltaTime Time elapsed since last frame
         * @return True if explosion is still active
         */
        bool update(float deltaTime) {
            if (triggered) {
                lifetime += deltaTime;
                return lifetime < duration;
            }
            return false;
        }
        
        /**
         * @brief Trigger the explosion
         */
        void trigger() {
            triggered = true;
            lifetime = 0.0f;
        }
        
        /**
         * @brief Calculate damage at a given distance from center
         * @param distance Distance from explosion center in pixels
         * @return Damage value (interpolated between center and edge)
         */
        int getDamageAtDistance(float distance) const {
            if (distance >= radius) return 0;
            if (distance <= 0.0f) return centerDamage;
            
            // Linear interpolation from center to edge
            float ratio = distance / radius;
            return static_cast<int>(centerDamage * (1.0f - ratio) + edgeDamage * ratio);
        }
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_EXPLOSION_H
