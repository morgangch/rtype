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
        // Basic enemies (4 types)
        Basic,      ///< Basic enemy - moves left, shoots straight
        Snake,      ///< Snake enemy - moves in a sine wave pattern
        Suicide,    ///< Suicide enemy - moves towards player and explodes on contact or death
        Pata,       ///< Pata enemy - rapid vertical oscillation, fires double shots

        // Advanced enemies (4 types)
        Shielded,   ///< Shielded enemy - cyclic invulnerability shield, vulnerable in intervals
        Flanker,    ///< Flanker enemy - diagonal movement, fires perpendicular shots to force vertical dodging
        Turret,     ///< Turret enemy - stationary turret, fires 3-shot burst aimed at player
        Waver,      ///< Waver enemy - erratic zigzag movement, fires triple bursts

        // Boss enemies (4 types)
        TankDestroyer,  ///< Tank destroyer boss - vertical bounce, fires 3-projectile spread
        Serpent,        ///< Serpent boss - multi-segment body with wave movement, head shoots spread
        Fortress,       ///< Fortress boss - stationary core with RED shield requiring 2 charged shots to break, fires 2 charged projectiles
        Core,           ///< Core boss (FINAL) - multi-phase evolution: circular->spiral+adds->chaotic pattern
        
        // Debris (passive obstacles - no shooting)
        DebrisSmall,    ///< Small asteroid debris - slow random drift
        DebrisLarge,    ///< Large asteroid debris - medium drift with rotation
        
        // Power-ups (4 types - collectibles)
        PowerUpHealth,      ///< Health restoration power-up
        PowerUpWeapon,      ///< Weapon upgrade power-up
        PowerUpShield,      ///< Temporary shield power-up
        PowerUpSpeed,       ///< Speed boost power-up
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
