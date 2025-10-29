/**
 * @file EnemyType.h
 * @brief Enemy type component for different enemy behaviors
 * 
 * This component distinguishes between different enemy types
 * (basic enemies, shooter enemies, etc.)
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_ENEMY_TYPE_H
#define COMMON_COMPONENTS_ENEMY_TYPE_H

namespace rtype::common::components {
    
    /**
     * @enum EnemyType
     * @brief Different types of enemies with varying behaviors
     */
    enum class EnemyType {
        // Basic enemy
        Basic,      ///< Basic enemy - moves left, shoots straight
        Snake,     ///< Snake enemy - moves in a sine wave pattern
        Suicide,    ///< Suicide enemy - moves towards player and explodes on contact or death
        Turret,     ///< Turret enemy - stays in place and shoots at player

        // Advanced enemies
        // to do add 3 advanced enemies
        Shooter,    ///< shooter enemy - moves left and shoots at player
        Shielded,    ///< shielded enemy - protected by a shield; only damaged by charged shots

        // Boss enemies
        // to do add 3 boss enemies
        TankDestroyer,        ///< Tank destroyer boss - heavily armored, fires spread projectiles
    };
    
    /**
     * @struct EnemyTypeComponent
     * @brief Component to store enemy type information
     */
    struct EnemyTypeComponent {
        EnemyType type;
        float lifeTime; ///< Time since spawn (for movement patterns)

        EnemyTypeComponent(EnemyType t = EnemyType::Basic) : type(t), lifeTime(0.0f) {}
    };
    
} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_ENEMY_TYPE_H
