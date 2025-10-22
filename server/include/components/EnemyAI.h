#ifndef COMMON_ENEMY_AI_HPP
#define COMMON_ENEMY_AI_HPP

#include <ECS/ECS.h>

/**
 * @file EnemyAI.h
 * @brief Server-side enemy AI component
 *
 * This component holds the AI parameters for enemy entities on the server.
 * The server uses these parameters in enemy update systems to drive movement
 * patterns, targeting, and firing behavior. Keeping AI on the server ensures
 * consistent and authoritative behavior across clients.
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

namespace rtype::server::components {
    /**
     * @brief Pre-defined AI movement and behavior patterns
     *
     * @details Used by the server-side enemy logic to select how an enemy moves
     * and behaves. The patterns are interpreted by the AI system and can be
     * extended with custom logic.
     */
    enum class AIPattern {
        Straight,   /**< Move straight along a direction */
        Sine,       /**< Sine-wave vertical oscillation */
        Zigzag,     /**< Zig-zag pattern */
        Circle,     /**< Circular orbiting movement */
        Follow,     /**< Follow a target entity (player) */
        Random,     /**< Random/unpredictable movement */
        Stationary  /**< Does not move, may still fire */
    };

    /**
     * @brief Component that stores AI configuration for an enemy
     *
     * Members are intentionally simple POD types to allow efficient access
     * during the AI update loop. Systems should read and update the
     * mutable fields (like lastFireTime and patternTimer) as the enemy acts.
     */
    class EnemyAI : public ECS::Component<EnemyAI> {
    public:
        /**
         * @brief Selected AI pattern for movement/behavior
         */
        AIPattern pattern;

        /**
         * @brief How aggressive the AI is [0.0..1.0]
         * @details Higher values make enemies pursue and fire more aggressively
         */
        float aggressiveness;

        /**
         * @brief Detection range in pixels for target acquisition
         */
        float detectionRange;

        /**
         * @brief Desired fire rate in seconds between shots
         */
        float fireRate;

        /**
         * @brief Time since last shot (seconds)
         * @details Systems will update this field as time progresses and reset
         * it when a shot is fired.
         */
        float lastFireTime;

        /**
         * @brief Timer used by movement pattern calculations (seconds)
         */
        float patternTimer;

        /**
         * @brief Base movement speed in pixels/second
         */
        float baseSpeed;

        /**
         * @brief Optional target entity ID (e.g., player) used by Follow pattern
         */
        ECS::EntityID targetEntity;

        /**
         * @brief Construct a new EnemyAI component with default parameters
         * @param pattern AI pattern to use (default: Straight)
         * @param aggressiveness Aggressiveness value in [0.0..1.0] (default: 0.5)
         */
        EnemyAI(AIPattern pattern = AIPattern::Straight, float aggressiveness = 0.5f);
    };
}

#endif // COMMON_ENEMY_AI_HPP
