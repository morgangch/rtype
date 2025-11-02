/**
 * @file HomingSystem.h
 * @brief System for homing projectile behavior
 * 
 * This system handles projectiles that track and follow targets.
 * Updates velocity to steer towards the nearest enemy.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_HOMING_SYSTEM_H
#define COMMON_SYSTEMS_HOMING_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Homing.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Team.h>
#include <cmath>
#include <vector>

namespace rtype::common::systems {

    /**
     * @class HomingSystem
     * @brief Manages homing projectile tracking behavior
     */
    class HomingSystem {
    public:
        /**
         * @brief Update all homing projectiles
         * @param world ECS world
         * @param deltaTime Time elapsed since last frame
         */
        static void update(ECS::World& world, float deltaTime) {
            auto* homingComponents = world.GetAllComponents<components::Homing>();
            if (!homingComponents) return;
            
            for (auto& [entity, homing] : *homingComponents) {
                auto* pos = world.GetComponent<components::Position>(entity);
                auto* vel = world.GetComponent<components::Velocity>(entity);
                auto* team = world.GetComponent<components::Team>(entity);
                
                if (!pos || !vel || !team) continue;
                
                // Find or update target
                if (!homing->hasTarget() || homing->shouldRetarget(deltaTime)) {
                    findNearestTarget(world, entity, homing.get(), pos, team);
                }
                
                // Steer towards target if we have one
                if (homing->hasTarget()) {
                    steerTowardsTarget(world, entity, homing.get(), pos, vel, deltaTime);
                }
            }
        }
        
    private:
        /**
         * @brief Find the nearest enemy target
         */
        static void findNearestTarget(ECS::World& world, ECS::EntityID projectile,
                                      components::Homing* homing, components::Position* projPos,
                                      components::Team* projTeam) {
            // Determine what we're targeting based on projectile team
            components::TeamType targetTeam = (projTeam->team == components::TeamType::Player) 
                                              ? components::TeamType::Enemy 
                                              : components::TeamType::Player;
            
            float nearestDist = homing->detectionRange;
            ECS::EntityID nearestTarget = 0;
            
            // Search all entities with position and team
            auto* allTeams = world.GetAllComponents<components::Team>();
            if (!allTeams) return;
            
            for (auto& [entity, team] : *allTeams) {
                if (entity == projectile) continue;
                if (team->team != targetTeam) continue;
                
                auto* targetPos = world.GetComponent<components::Position>(entity);
                if (!targetPos) continue;
                
                // Calculate distance
                float dx = targetPos->x - projPos->x;
                float dy = targetPos->y - projPos->y;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < nearestDist) {
                    nearestDist = dist;
                    nearestTarget = entity;
                }
            }
            
            if (nearestTarget != 0) {
                homing->setTarget(nearestTarget);
            } else {
                homing->clearTarget();
            }
        }
        
        /**
         * @brief Steer projectile towards its target
         */
        static void steerTowardsTarget(ECS::World& world, ECS::EntityID projectile,
                                       components::Homing* homing, components::Position* projPos,
                                       components::Velocity* vel, float deltaTime) {
            auto* targetPos = world.GetComponent<components::Position>(homing->targetId);
            
            // If target no longer exists, clear it
            if (!targetPos) {
                homing->clearTarget();
                return;
            }
            
            // Calculate direction to target
            float dx = targetPos->x - projPos->x;
            float dy = targetPos->y - projPos->y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < 1.0f) return; // Too close, don't adjust
            
            // Desired direction (normalized)
            float desiredDx = dx / dist;
            float desiredDy = dy / dist;
            
            // Current direction (normalized)
            float currentSpeed = std::sqrt(vel->vx * vel->vx + vel->vy * vel->vy);
            if (currentSpeed < 1.0f) currentSpeed = homing->maxSpeed;
            
            float currentDx = vel->vx / currentSpeed;
            float currentDy = vel->vy / currentSpeed;
            
            // Calculate angle difference
            float dot = currentDx * desiredDx + currentDy * desiredDy;
            float cross = currentDx * desiredDy - currentDy * desiredDx;
            
            // Turn towards target (limited by turn speed)
            float maxTurn = homing->turnSpeed * deltaTime;
            float angleToTarget = std::atan2(cross, dot);
            
            // Clamp turn angle
            if (angleToTarget > maxTurn) angleToTarget = maxTurn;
            if (angleToTarget < -maxTurn) angleToTarget = -maxTurn;
            
            // Apply rotation
            float cosAngle = std::cos(angleToTarget);
            float sinAngle = std::sin(angleToTarget);
            
            float newDx = currentDx * cosAngle - currentDy * sinAngle;
            float newDy = currentDx * sinAngle + currentDy * cosAngle;
            
            // Update velocity (maintain speed)
            vel->vx = newDx * homing->maxSpeed;
            vel->vy = newDy * homing->maxSpeed;
        }
    };

} // namespace rtype::common::systems

#endif // COMMON_SYSTEMS_HOMING_SYSTEM_H
