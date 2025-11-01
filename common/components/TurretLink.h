/**
 * @file TurretLink.h
 * @brief Component to link turrets to their parent boss
 * 
 * This component creates a relationship between boss entities and their
 * defensive turrets, allowing mechanics like "destroy all turrets to
 * disable boss shield".
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_TURRET_LINK_H
#define COMMON_COMPONENTS_TURRET_LINK_H

#include <ECS/ECS.h>
#include <vector>

namespace rtype::common::components {
    
    /**
     * @struct TurretLinkComponent
     * @brief Links turrets to their parent boss entity
     */
    struct TurretLinkComponent {
        ECS::EntityID parentBoss;           ///< The boss entity this turret is linked to
        std::vector<ECS::EntityID> turrets; ///< Turret entities linked to this boss (only used by boss entity)
        bool isTurret;                      ///< True if this entity is a turret, false if it's the boss
        
        // Constructor for turret
        TurretLinkComponent(ECS::EntityID boss) 
            : parentBoss(boss), isTurret(true) {}
        
        // Constructor for boss
        TurretLinkComponent() 
            : parentBoss(0), isTurret(false) {}
    };
    
} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_TURRET_LINK_H
