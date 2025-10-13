/**
 * @file Health.h
 * @brief Health component for entity damage and life management
 * 
 * This file defines the Health component which manages an entity's health points,
 * alive state, and invulnerability mechanics. Used by combat and damage systems.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_HEALTH_HPP
#define COMMON_HEALTH_HPP

#include <ECS/ECS.h>

namespace rtype::common::components {
    /**
     * @class Health
     * @brief Component managing entity health and damage state
     * 
     * The Health component tracks an entity's current health, maximum health,
     * alive state, and temporary invulnerability. It is used by:
     * - Damage systems to apply damage
     * - AI systems to check if entities should be removed
     * - UI systems to display health bars
     * - Collision systems for invulnerability frames
     * 
     * Features:
     * - Current and maximum health tracking
     * - Alive/dead state management
     * - Temporary invulnerability (e.g., after taking damage)
     * - Invulnerability timer for time-based immunity
     * 
     * Example usage:
     * @code
     * // Create player with 100 HP
     * auto& health = player.addComponent<Health>(100);
     * 
     * // Take damage
     * if (!health.invulnerable) {
     *     health.currentHp -= 25;
     *     if (health.currentHp <= 0) {
     *         health.currentHp = 0;
     *         health.isAlive = false;
     *     }
     *     // Grant brief invulnerability
     *     health.invulnerable = true;
     *     health.invulnerabilityTimer = 2.0f;  // 2 seconds
     * }
     * 
     * // In update loop
     * if (health.invulnerable) {
     *     health.invulnerabilityTimer -= deltaTime;
     *     if (health.invulnerabilityTimer <= 0.0f) {
     *         health.invulnerable = false;
     *     }
     * }
     * @endcode
     */
    class Health : public ECS::Component<Health> {
    public:
        /** @brief Current health points (0 = dead) */
        int currentHp;
        
        /** @brief Maximum health capacity */
        int maxHp;
        
        /** @brief Whether the entity is alive (false = dead/destroyed) */
        bool isAlive;
        
        /** @brief Whether the entity is currently invulnerable to damage */
        bool invulnerable;
        
        /** @brief Remaining invulnerability time in seconds */
        float invulnerabilityTimer;

        /**
         * @brief Construct a new Health component
         * @param maxHp Maximum health points (default: 100)
         * 
         * Initializes health with:
         * - Current HP = Max HP (full health)
         * - isAlive = true
         * - invulnerable = false
         * - invulnerabilityTimer = 0.0
         */
        Health(int maxHp = 100)
            : currentHp(maxHp)
            , maxHp(maxHp)
            , isAlive(true)
            , invulnerable(false)
            , invulnerabilityTimer(0.0f) {}
    };
}

#endif // COMMON_HEALTH_HPP

