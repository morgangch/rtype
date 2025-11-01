/**
 * @file FortressShieldSystem.h
 * @brief System to manage Fortress boss shield mechanics
 * 
 * This system handles the Fortress boss shield logic:
 * - Checks if all turrets are destroyed
 * - Disables boss shield when turrets are gone
 * - Prevents damage to shielded entities
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_FORTRESS_SHIELD_SYSTEM_H
#define COMMON_SYSTEMS_FORTRESS_SHIELD_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Shield.h>
#include <common/components/TurretLink.h>
#include <common/components/Health.h>
#include <common/components/EnemyType.h>
#include <iostream>

namespace rtype::common::systems {

class FortressShieldSystem {
public:
    /**
     * @brief Update fortress shields based on turret status
     * @param world ECS world reference
     * @param deltaTime Time elapsed since last update
     */
    static void update(ECS::World& world, float deltaTime) {
        // Find all Fortress bosses
        auto* enemyTypes = world.GetAllComponents<components::EnemyTypeComponent>();
        if (!enemyTypes) return;

        for (auto& [entity, typePtr] : *enemyTypes) {
            if (!typePtr || typePtr->type != components::EnemyType::Fortress) continue;

            auto* turretLink = world.GetComponent<components::TurretLinkComponent>(entity);
            auto* shield = world.GetComponent<components::ShieldComponent>(entity);
            
            if (!turretLink || turretLink->isTurret) continue; // Only process boss entities
            if (!shield) continue;

            // Check if any turrets are still alive
            bool anyTurretAlive = false;
            for (auto turretId : turretLink->turrets) {
                auto* turretHealth = world.GetComponent<components::Health>(turretId);
                if (turretHealth && turretHealth->isAlive && turretHealth->currentHp > 0) {
                    anyTurretAlive = true;
                    break;
                }
            }

            // Update boss shield based on turret status
            if (!anyTurretAlive && shield->isActive) {
                shield->isActive = false;
                shield->type = components::ShieldType::None;
                std::cout << "[FortressShieldSystem] 🛡️  All turrets destroyed! Boss shield disabled!" << std::endl;
            }
        }
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
