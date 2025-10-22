/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerEntityCleanupSystem - Detects and removes dead entities from the world
*/
#ifndef SERVER_ENTITY_CLEANUP_SYSTEM_H
#define SERVER_ENTITY_CLEANUP_SYSTEM_H

#include "ECS/System.h"

/**
 * @brief System that detects dead entities and notifies all clients
 * 
 * Scans all entities with Health components, identifies those that are
 * dead (HP <= 0 or isAlive == false), and:
 * 1. Broadcasts ENTITY_DESTROY packet to all connected players
 * 2. Removes the entity from the ECS world
 * 
 * This ensures clients stay synchronized with server entity state.
 * 
 * @note Priority: 4 (runs before most systems but after collision)
 */
class ServerEntityCleanupSystem : public ECS::System {
public:
    ServerEntityCleanupSystem() : ECS::System("ServerEntityCleanupSystem", 4) {}
    
    /**
     * @brief Update cycle - cleans up dead entities
     * 
     * Process:
     * 1. Finds all entities with Health component
     * 2. Checks if HP <= 0 or isAlive == false
     * 3. Sends ENTITY_DESTROY to all players
     * 4. Destroys the entity
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (unused)
     */
    void Update(ECS::World &world, float deltaTime) override;
};

#endif // SERVER_ENTITY_CLEANUP_SYSTEM_H
