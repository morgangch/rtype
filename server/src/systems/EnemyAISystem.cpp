#include "systems/EnemyAISystem.h"

namespace rtype::server::systems {

EnemyAISystem::EnemyAISystem() : ECS::System("EnemyAISystem", 40) {}

void EnemyAISystem::SetProjectileCallback(ProjectileCallback callback) {
    m_createProjectile = callback;
}

void EnemyAISystem::Update(ECS::World& world, float deltaTime) {
    if (!m_createProjectile) return;
    auto* teams = world.GetAllComponents<rtype::common::components::Team>();
    if (!teams) return;
    float playerX = 0.0f, playerY = 0.0f;
    bool playerFound = false;
    auto* players = world.GetAllComponents<rtype::common::components::Player>();
    if (players) {
        for (auto& [playerEntity, playerPtr] : *players) {
            auto* pos = world.GetComponent<rtype::common::components::Position>(playerEntity);
            if (pos) {
                playerX = pos->x;
                playerY = pos->y;
                playerFound = true;
                break;
            }
        }
    }
    for (auto& [entity, teamPtr] : *teams) {
    if (teamPtr->team != rtype::common::components::TeamType::Enemy) continue;
    if (!world.GetComponent<rtype::common::components::Health>(entity)) continue;
    auto* enemyType = world.GetComponent<rtype::common::components::EnemyTypeComponent>(entity);
    auto* fireRate = world.GetComponent<rtype::common::components::FireRate>(entity);
    auto* pos = world.GetComponent<rtype::common::components::Position>(entity);
        if (!fireRate || !pos || !fireRate->canFire()) continue;
    if (enemyType && enemyType->type == rtype::common::components::EnemyType::TankDestroyer) {
            handleTankDestroyerShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
            fireRate->shoot();
    } else if (enemyType && enemyType->type == rtype::common::components::EnemyType::Shooter) {
            handleShooterShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
            fireRate->shoot();
    } else if (enemyType && enemyType->type == rtype::common::components::EnemyType::Snake) {
            handleSnakeShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
            fireRate->shoot();
    } else if (enemyType && enemyType->type == rtype::common::components::EnemyType::Suicide) {
            // to do
            //handleSuicideShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
            //fireRate->shoot();
        } else {
            m_createProjectile(pos->x, pos->y, -300.0f, 0.0f, world);
            fireRate->shoot();
        }
    }
}

void EnemyAISystem::handleTankDestroyerShooting(float x, float y, float targetX, float targetY, bool hasTarget, ECS::World& world) {
    if (!hasTarget) return;
    const float PROJECTILE_SPEED = 350.0f;
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance <= 0.0f) return;
    float baseVx = (dx / distance) * PROJECTILE_SPEED;
    float baseVy = (dy / distance) * PROJECTILE_SPEED;
    m_createProjectile(x, y, baseVx, baseVy, world);
    const float spreadAngle = 0.26f;
    float upperVx = baseVx * std::cos(spreadAngle) - baseVy * std::sin(spreadAngle);
    float upperVy = baseVx * std::sin(spreadAngle) + baseVy * std::cos(spreadAngle);
    m_createProjectile(x, y, upperVx, upperVy, world);
    float lowerVx = baseVx * std::cos(-spreadAngle) - baseVy * std::sin(-spreadAngle);
    float lowerVy = baseVx * std::sin(-spreadAngle) + baseVy * std::cos(-spreadAngle);
    m_createProjectile(x, y, lowerVx, lowerVy, world);
}

void EnemyAISystem::handleShooterShooting(float x, float y, float targetX, float targetY, bool hasTarget, ECS::World& world) {
    if (!hasTarget) return;
    const float PROJECTILE_SPEED = 300.0f;
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = std::sqrt(dx * dx + dy * dy);
    if (distance <= 0.0f) return;
    float vx = (dx / distance) * PROJECTILE_SPEED;
    float vy = (dy / distance) * PROJECTILE_SPEED;
    m_createProjectile(x, y, vx, vy, world);
}

void EnemyAISystem::handleSnakeShooting(float x, float y, float targetX, float targetY, bool hasTarget, ECS::World& world) {
    // Snake enemy shoots straight left
    const float PROJECTILE_SPEED = 250.0f;
    m_createProjectile(x, y, -PROJECTILE_SPEED, 0.0f, world);
}

void EnemyAISystem::Initialize(ECS::World& world) {}

void EnemyAISystem::Cleanup(ECS::World& world) {
    m_createProjectile = nullptr;
}

} // namespace rtype::server::systems
