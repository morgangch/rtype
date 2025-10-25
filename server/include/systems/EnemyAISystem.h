/**
 * @file EnemyAISystem.h
 * @brief AI system for enemy shooting behavior (serveur)
 * 
 * Ce système gère les patterns de tir des ennemis selon leur type.
 * Différents types d'ennemis ont des comportements de tir différents.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H
#define SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Team.h>
#include <common/components/Player.h>
#include <common/components/Health.h>
#include <common/components/FireRate.h>
#include <common/components/EnemyType.h>

#include <functional>
#include <cmath>

namespace rtype::server::systems {
    class EnemyAISystem : public ECS::System {
    public:
        using ProjectileCallback = std::function<void(float, float, float, float, ECS::World&)>;
    private:
        ProjectileCallback m_createProjectile;
    public:
        EnemyAISystem() : ECS::System("EnemyAISystem", 40) {}
        void SetProjectileCallback(ProjectileCallback callback) { m_createProjectile = callback; }
        void Update(ECS::World& world, float deltaTime) override {
            if (!m_createProjectile) return;
            auto* teams = world.GetAllComponents<components::Team>();
            if (!teams) return;
            float playerX = 0.0f, playerY = 0.0f;
            bool playerFound = false;
            auto* players = world.GetAllComponents<components::Player>();
            if (players) {
                for (auto& [playerEntity, playerPtr] : *players) {
                    auto* pos = world.GetComponent<components::Position>(playerEntity);
                    if (pos) {
                        playerX = pos->x;
                        playerY = pos->y;
                        playerFound = true;
                        break;
                    }
                }
            }
            for (auto& [entity, teamPtr] : *teams) {
                if (teamPtr->team != components::TeamType::Enemy) continue;
                if (!world.GetComponent<components::Health>(entity)) continue;
                auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
                auto* fireRate = world.GetComponent<components::FireRate>(entity);
                auto* pos = world.GetComponent<components::Position>(entity);
                if (!fireRate || !pos || !fireRate->canFire()) continue;
                if (enemyType && enemyType->type == components::EnemyType::Boss) {
                    handleBossShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
                    fireRate->shoot();
                } else if (enemyType && enemyType->type == components::EnemyType::Shooter) {
                    handleShooterShooting(pos->x, pos->y, playerX, playerY, playerFound, world);
                    fireRate->shoot();
                } else {
                    m_createProjectile(pos->x, pos->y, -300.0f, 0.0f, world);
                    fireRate->shoot();
                }
            }
        }
    private:
        void handleBossShooting(float x, float y, float targetX, float targetY, bool hasTarget, ECS::World& world) {
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
        void handleShooterShooting(float x, float y, float targetX, float targetY, bool hasTarget, ECS::World& world) {
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
    public:
        void Initialize(ECS::World& world) override {}
        void Cleanup(ECS::World& world) override { m_createProjectile = nullptr; }
    };
}

#endif // SERVER_SYSTEMS_ENEMY_AI_SYSTEM_H
