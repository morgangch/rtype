/**
 * @file FortressShieldSystem.h
 * @brief System to manage Fortress boss shield mechanics
 * 
 * This system handles the Fortress boss shield logic:
 * - Manages RED shield that requires 2 charged shots to break
 * - Provides shield status checks for damage prevention
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_FORTRESS_SHIELD_SYSTEM_H
#define COMMON_SYSTEMS_FORTRESS_SHIELD_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Shield.h>
#include <common/components/Health.h>
#include <common/components/EnemyType.h>
#include <iostream>

namespace rtype::common::systems {

class FortressShieldSystem {
public:
    /**
     * @brief Update fortress shields (placeholder - shield is managed by collision system)
     * @param world ECS world reference
     * @param deltaTime Time elapsed since last update
     * 
     * The Fortress boss now has a RED shield that requires 2 charged shots to break.
     * Shield hits are tracked in the ShieldComponent's chargedHitsRequired field.
     */
    static void update(ECS::World& world, float deltaTime) {
        // Fortress shield is now managed directly by the collision system
        (void)world;
        (void)deltaTime;
    }

    /**
     * @brief Check if an entity is protected by a shield
     * @param entity Entity to check
     * @param world ECS world reference
     * @return true if entity has an active shield
     */
    static bool isShielded(ECS::EntityID entity, ECS::World& world) {
        auto* shield = world.GetComponent<components::ShieldComponent>(entity);
        return shield && shield->isActive;
    }

    /**
     * @brief Get shield type for an entity
     * @param entity Entity to check
     * @param world ECS world reference
     * @return ShieldType (None if no shield)
     */
    static components::ShieldType getShieldType(ECS::EntityID entity, ECS::World& world) {
        auto* shield = world.GetComponent<components::ShieldComponent>(entity);
        if (!shield || !shield->isActive) return components::ShieldType::None;
        return shield->type;
    }
};

} // namespace rtype::common::systems

#endif // COMMON_SYSTEMS_FORTRESS_SHIELD_SYSTEM_H
