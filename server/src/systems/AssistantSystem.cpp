#include "systems/AssistantSystem.h"
#include "rtype.h"
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/EnemyType.h>
#include <common/components/Player.h>
#include <common/components/Team.h>
#include "components/Assistant.h"
#include "components/LinkedRoom.h"
#include "controllers/RoomController.h"
#include <cmath>
#include <iostream>

namespace rtype::server::systems {

AssistantSystem::AssistantSystem()
    : ECS::System("AssistantSystem", 4)
{
    // constructor initializes system name and priority for the ECS
}

void AssistantSystem::Update(ECS::World &world, float deltaTime) {
    // Iterate over all assistant-tagged entities
    auto *assistants = world.GetAllComponents<rtype::server::components::Assistant>();
    if (!assistants) return;

    // Pre-fetch enemies map to avoid multiple GetAllComponents calls inside the loop
    auto *enemies = world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();

    for (auto &pair : *assistants) {
        ECS::EntityID aid = pair.first;
        auto *assistantComp = world.GetComponent<rtype::server::components::Assistant>(aid);
        if (!assistantComp) continue;

        // Basic cooldown timer
        if (assistantComp->shootCooldown > 0.0f) {
            assistantComp->shootCooldown -= deltaTime;
            if (assistantComp->shootCooldown < 0.0f) assistantComp->shootCooldown = 0.0f;
        }

        // Get position and linked room
        auto *pos = world.GetComponent<rtype::common::components::Position>(aid);
        auto *vel = world.GetComponent<rtype::common::components::Velocity>(aid);
        auto *linked = world.GetComponent<rtype::server::components::LinkedRoom>(aid);
        auto *health = world.GetComponent<rtype::common::components::Health>(aid);
        if (!pos || !linked || !vel || !health) continue;

        // If assistant is dead, skip
        if (!health->isAlive || health->currentHp <= 0) continue;

        // Find nearest enemy in same room
        ECS::EntityID nearest = 0;
        float bestDist2 = std::numeric_limits<float>::infinity();
        if (enemies) {
            for (auto &epr : *enemies) {
                ECS::EntityID eid = epr.first;
                auto *enemyLinked = world.GetComponent<rtype::server::components::LinkedRoom>(eid);
                if (!enemyLinked) continue;
                if (enemyLinked->room_id != linked->room_id) continue;
                auto *enemyHealth = world.GetComponent<rtype::common::components::Health>(eid);
                if (!enemyHealth || !enemyHealth->isAlive || enemyHealth->currentHp <= 0) continue;
                auto *enemyPos = world.GetComponent<rtype::common::components::Position>(eid);
                if (!enemyPos) continue;
                float dx = enemyPos->x - pos->x;
                float dy = enemyPos->y - pos->y;
                float d2 = dx*dx + dy*dy;
                if (d2 < bestDist2) {
                    bestDist2 = d2;
                    nearest = eid;
                }
            }
        }

        // Simple movement: try to follow nearest enemy's Y position
        if (nearest != 0) {
            auto *enemyPos = world.GetComponent<rtype::common::components::Position>(nearest);
            if (enemyPos) {
                float diffY = enemyPos->y - pos->y;
                const float TRACK_SPEED = 120.0f; // vertical tracking speed
                if (std::abs(diffY) > 8.0f) {
                    vel->vy = (diffY > 0) ? TRACK_SPEED : -TRACK_SPEED;
                } else {
                    vel->vy = 0.0f;
                }
                // Keep assistant at a fixed X near left side
                const float TARGET_X = 120.0f;
                float diffX = TARGET_X - pos->x;
                if (std::abs(diffX) > 4.0f) {
                    vel->vx = (diffX > 0) ? 60.0f : -60.0f;
                } else {
                    vel->vx = 0.0f;
                }
            }
        } else {
            // No enemies: idle patrol vertically
            const float PATROL_SPEED = 40.0f;
            vel->vy = (std::fmod(pos->y, 200.0f) < 100.0f) ? PATROL_SPEED : -PATROL_SPEED;
            vel->vx = 0.0f;
        }

        // Shooting: if nearest enemy in range and cooldown ready, shoot basic projectile
        if (nearest != 0 && assistantComp->shootCooldown <= 0.0f) {
            // Create a server projectile and broadcast
            ECS::EntityID proj = rtype::server::controllers::room_controller::createServerProjectile(linked->room_id, aid, pos->x, pos->y, false);
            rtype::server::controllers::room_controller::broadcastProjectileSpawn(proj, aid, linked->room_id, false);
            assistantComp->shootCooldown = 0.6f; // 600ms between shots
            std::cout << "Assistant " << aid << " shot projectile " << proj << " in room " << linked->room_id << std::endl;
        }
    }
}

} // namespace rtype::server::systems
