#include "systems/EnemyAISystem.h"
#include <common/systems/FireRateSystem.h>
#include <common/systems/EnemyAISystem.h>

namespace rtype::server::systems {

EnemyAISystem::EnemyAISystem() : ECS::System("EnemyAISystem", 40) {}

void EnemyAISystem::SetProjectileCallback(ProjectileCallback callback) {
    m_createProjectile = callback;
}

void EnemyAISystem::Update(ECS::World& world, float deltaTime) {
    if (!m_createProjectile) return;

    rtype::common::systems::FireRateSystem::update(world, deltaTime);

    auto createProjectile = [this, &world](ECS::EntityID shooter, float x, float y, float vx, float vy) {
        m_createProjectile(shooter, x, y, vx, vy, world);
    };

    rtype::common::systems::EnemyAISystem::update(world, deltaTime, createProjectile);
}

// Legacy shooting handlers removed - now using common/systems/EnemyAISystem

void EnemyAISystem::Initialize(ECS::World& world) {}

void EnemyAISystem::Cleanup(ECS::World& world) {
    m_createProjectile = nullptr;
}

} // namespace rtype::server::systems
