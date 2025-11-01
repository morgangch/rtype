/**
 * @file Shield.h
 * @brief Shield component for invincibility and visual effects
 * 
 * This component provides shield functionality with different colors
 * and invincibility states for enemies and bosses.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_SHIELD_H
#define COMMON_COMPONENTS_SHIELD_H

namespace rtype::common::components {
    
    /**
     * @enum ShieldType
     * @brief Different types of shields with varying colors and behaviors
     */
    enum class ShieldType {
        None,       ///< No shield active
        Blue,       ///< Blue shield - can be destroyed
        Red,        ///< Red shield - invincible until conditions met
        Cyclic      ///< Cyclic shield - alternates on/off (for Shielded enemy)
    };
    
    /**
     * @struct ShieldComponent
     * @brief Component to manage entity shield state
     */
    struct ShieldComponent {
        ShieldType type;        ///< Type of shield
        bool isActive;          ///< Whether shield is currently active
        float cycleTimer;       ///< Timer for cyclic shields (Shielded enemy)
        float cycleDuration;    ///< Duration of shield cycle (on/off periods)
        
        ShieldComponent(ShieldType t = ShieldType::None, bool active = true) 
            : type(t), isActive(active), cycleTimer(0.0f), cycleDuration(2.0f) {}
    };
    
} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_SHIELD_H
