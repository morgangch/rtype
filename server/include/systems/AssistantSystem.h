/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** AssistantSystem - Server-side AI helper system
*/

#ifndef SERVER_ASSISTANT_SYSTEM_H
#define SERVER_ASSISTANT_SYSTEM_H

#include "ECS/System.h"
#include "ECS/World.h"

namespace rtype::server::systems {

/**
 * @file AssistantSystem.h
 * @brief Server-side system that controls AI assistant vessels
 *
 * The AssistantSystem is responsible for updating entities tagged with
 * `server::components::Assistant`. Responsibilities include:
 *  - decrementing the assistant's firing cooldown
 *  - finding nearby hostile entities in the same room
 *  - steering the assistant (simple Y tracking + minor X corrections)
 *  - firing basic projectiles when appropriate and resetting cooldowns
 *
 * This system runs on the server only and performs authoritative gameplay
 * logic; clients are informed of resulting entity state via existing
 * network broadcasts.
 */
class AssistantSystem : public ECS::System {
public:
    /**
     * @brief Construct a new AssistantSystem
     *
     * Provides a human-readable name and optional priority to the ECS base
     * class. Priority controls ordering among systems (higher runs earlier).
     */
    AssistantSystem();

    /**
     * @brief Main update loop called every frame by the ECS world
     * @param world Reference to the ECS world containing entities/components
     * @param deltaTime Time elapsed since last update (seconds)
     *
     * The method iterates assistant-tagged entities, updates cooldowns,
     * computes movement vectors, and issues projectile spawns when needed.
     */
    void Update(ECS::World &world, float deltaTime) override;
};

} // namespace rtype::server::systems

#endif // SERVER_ASSISTANT_SYSTEM_H
