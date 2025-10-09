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
        Basic,      ///< Basic enemy - moves left, shoots straight
        Shooter,    ///< Shooter enemy - moves left and shoots at player
        Boss        ///< Boss enemy - high HP, shoots in spread pattern
    };
    
    /**
     * @struct EnemyTypeComponent
     * @brief Component to store enemy type information
     */
    struct EnemyTypeComponent {
        EnemyType type;
        
        EnemyTypeComponent(EnemyType t = EnemyType::Basic) : type(t) {}
    };
    
} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_ENEMY_TYPE_H
