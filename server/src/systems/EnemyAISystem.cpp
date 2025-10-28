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

    // CRITICAL FIX: Update fire rate cooldowns (was missing before!)
    rtype::common::systems::FireRateSystem::update(world, deltaTime);

    // Use common enemy AI system with server's projectile creation callback
    // Lambda adapter to match the common system's signature (no ECS::World parameter)
    auto createProjectile = [this, &world](float x, float y, float vx, float vy) {
        m_createProjectile(x, y, vx, vy, world);
    };

    rtype::common::systems::EnemyAISystem::update(world, deltaTime, createProjectile);
}

// Legacy shooting handlers removed - now using common/systems/EnemyAISystem

void EnemyAISystem::Initialize(ECS::World& world) {}

void EnemyAISystem::Cleanup(ECS::World& world) {
    m_createProjectile = nullptr;
}

} // namespace rtype::server::systems
