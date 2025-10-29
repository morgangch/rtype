#ifndef COMMON_SYSTEMS_COLLISION_SYSTEM_H
#define COMMON_SYSTEMS_COLLISION_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/Projectile.h>
#include <common/components/Player.h>
#include <common/components/EnemyType.h>
#include <functional>
#include <vector>

namespace rtype::common::systems {

struct CollisionHandlers {
    std::function<void(ECS::EntityID player, ECS::EntityID enemy, ECS::World&)> onPlayerVsEnemy;
    std::function<void(ECS::EntityID projectile, ECS::EntityID enemy, ECS::World&)> onPlayerProjectileVsEnemy;
    std::function<void(ECS::EntityID projectile, ECS::EntityID player, ECS::World&)> onEnemyProjectileVsPlayer;
    std::function<void(ECS::EntityID suicideEnemy, ECS::World&)> onSuicideExplosion;
};

class CollisionSystem {
public:
    static bool checkAABB(float x1, float y1, float w1, float h1,
                         float x2, float y2, float w2, float h2) {
        float left1 = x1 - w1 / 2.0f;
        float right1 = x1 + w1 / 2.0f;
        float top1 = y1 - h1 / 2.0f;
        float bottom1 = y1 + h1 / 2.0f;

        float left2 = x2 - w2 / 2.0f;
        float right2 = x2 + w2 / 2.0f;
        float top2 = y2 - h2 / 2.0f;
        float bottom2 = y2 + h2 / 2.0f;

        return !(right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2);
    }

    static void getEntitySize(ECS::EntityID entity, ECS::World& world, float& width, float& height) {
        width = 33.0f;
        height = 17.0f;

        auto* team = world.GetComponent<components::Team>(entity);
        if (!team) return;

        if (team->team == components::TeamType::Player) {
            width = 33.0f;
            height = 17.0f;
        } else if (team->team == components::TeamType::Enemy) {
            auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(entity);
            if (enemyType && enemyType->type == components::EnemyType::TankDestroyer) {
                width = 33.0f * 5.0f;
                height = 36.0f * 5.0f;
            } else {
                width = 33.0f;
                height = 36.0f;
            }
        }
    }

    static void update(ECS::World& world, float deltaTime, const CollisionHandlers& handlers) {
        checkPlayerVsEnemies(world, handlers);
        checkPlayerProjectilesVsEnemies(world, handlers);
        checkEnemyProjectilesVsPlayers(world, handlers);
    }

private:
    static void checkPlayerVsEnemies(ECS::World& world, const CollisionHandlers& handlers) {
        if (!handlers.onPlayerVsEnemy) return;

        auto* players = world.GetAllComponents<components::Player>();
        if (!players) return;

        for (auto& [player, playerPtr] : *players) {
            auto* playerPos = world.GetComponent<components::Position>(player);
            auto* playerHealth = world.GetComponent<components::Health>(player);
            if (!playerPos || !playerHealth || playerHealth->invulnerable) continue;

            float playerW, playerH;
            getEntitySize(player, world, playerW, playerH);

            // Check vs all enemies
            auto* enemies = world.GetAllComponents<components::Team>();
            if (!enemies) continue;

            for (auto& [enemy, teamPtr] : *enemies) {
                if (enemy == player || teamPtr->team != components::TeamType::Enemy) continue;

                auto* enemyPos = world.GetComponent<components::Position>(enemy);
                auto* enemyHealth = world.GetComponent<components::Health>(enemy);
                if (!enemyPos || !enemyHealth) continue;

                float enemyW, enemyH;
                getEntitySize(enemy, world, enemyW, enemyH);

                if (checkAABB(playerPos->x, playerPos->y, playerW, playerH,
                             enemyPos->x, enemyPos->y, enemyW, enemyH)) {
                    handlers.onPlayerVsEnemy(player, enemy, world);

                    auto* enemyType = world.GetComponent<components::EnemyTypeComponent>(enemy);
                    if (enemyType && enemyType->type == components::EnemyType::Suicide && handlers.onSuicideExplosion) {
                        handlers.onSuicideExplosion(enemy, world);
                    }
                }
            }
        }
    }

    static void checkPlayerProjectilesVsEnemies(ECS::World& world, const CollisionHandlers& handlers) {
        if (!handlers.onPlayerProjectileVsEnemy) return;

        auto* projectiles = world.GetAllComponents<components::Projectile>();
        if (!projectiles) return;

        for (auto& [proj, projData] : *projectiles) {
            auto* projTeam = world.GetComponent<components::Team>(proj);
            auto* projPos = world.GetComponent<components::Position>(proj);
            if (!projTeam || !projPos || projTeam->team != components::TeamType::Player) continue;

            if (projData->distanceTraveled < 1.0f) continue;

            float projW = 20.0f, projH = 10.0f;

            auto* enemies = world.GetAllComponents<components::Team>();
            if (!enemies) continue;

            for (auto& [enemy, teamPtr] : *enemies) {
                if (teamPtr->team != components::TeamType::Enemy) continue;

                auto* enemyPos = world.GetComponent<components::Position>(enemy);
                auto* enemyHealth = world.GetComponent<components::Health>(enemy);
                if (!enemyPos || !enemyHealth) continue;

                float enemyW, enemyH;
                getEntitySize(enemy, world, enemyW, enemyH);

                if (checkAABB(projPos->x, projPos->y, projW, projH,
                             enemyPos->x, enemyPos->y, enemyW, enemyH)) {
                    handlers.onPlayerProjectileVsEnemy(proj, enemy, world);
                    if (!projData->piercing) break;
                }
            }
        }
    }

    static void checkEnemyProjectilesVsPlayers(ECS::World& world, const CollisionHandlers& handlers) {
        if (!handlers.onEnemyProjectileVsPlayer) return;

        auto* projectiles = world.GetAllComponents<components::Projectile>();
        if (!projectiles) return;

        for (auto& [proj, projData] : *projectiles) {
            auto* projTeam = world.GetComponent<components::Team>(proj);
            auto* projPos = world.GetComponent<components::Position>(proj);
            if (!projTeam || !projPos || projTeam->team != components::TeamType::Enemy) continue;

            if (projData->distanceTraveled < 1.0f) continue;

            float projW = 20.0f, projH = 10.0f;

            auto* players = world.GetAllComponents<components::Player>();
            if (!players) continue;

            for (auto& [player, playerPtr] : *players) {
                auto* playerPos = world.GetComponent<components::Position>(player);
                auto* playerHealth = world.GetComponent<components::Health>(player);
                if (!playerPos || !playerHealth || playerHealth->invulnerable) continue;

                float playerW, playerH;
                getEntitySize(player, world, playerW, playerH);

                if (checkAABB(projPos->x, projPos->y, projW, projH,
                             playerPos->x, playerPos->y, playerW, playerH)) {
                    handlers.onEnemyProjectileVsPlayer(proj, player, world);
                    break;
                }
            }
        }
    }
};

} // namespace rtype::common::systems

#endif // COMMON_SYSTEMS_COLLISION_SYSTEM_H
