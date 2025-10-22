/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerCollisionSystem - Server-authoritative collision detection and resolution
*/

#ifndef SERVER_COLLISION_SYSTEM_H
#define SERVER_COLLISION_SYSTEM_H

#include "ECS/System.h"
#include "ECS/World.h"
#include "common/components/Position.h"
#include "common/components/Velocity.h"
#include "common/components/Health.h"
#include "common/components/Team.h"
#include "common/components/Projectile.h"
#include "packets.h"
#include <vector>

namespace rtype::server::systems {

/**
 * @brief Server-authoritative collision detection and damage system
 * 
 * Handles all collision detection on the server side to ensure consistency
 * across all clients. Detects collisions between:
 * - Player projectiles vs enemies (damage enemies)
 * - Enemy projectiles vs players (damage players)
 * 
 * When collisions occur, broadcasts ENTITY_DESTROY packets to all clients
 * in the affected rooms, ensuring synchronized entity destruction.
 * 
 * @note Priority: 5 (medium priority, runs during gameplay loop)
 * @note Uses simple AABB (Axis-Aligned Bounding Box) collision detection
 */
class ServerCollisionSystem : public ECS::System {
public:
    ServerCollisionSystem() : ECS::System("ServerCollisionSystem", 5) {}
    
    /**
     * @brief Update cycle - performs collision detection and resolution
     * 
     * Main collision detection loop:
     * 1. Checks projectile vs enemy collisions
     * 2. Applies damage to hit entities
     * 3. Destroys dead entities
     * 4. Broadcasts destruction to all clients
     * 
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (unused)
     */
    void Update(ECS::World& world, float deltaTime) override;

private:
    /**
     * @brief Checks collisions between projectiles and enemies
     * 
     * Iterates through all projectiles and enemies, performing AABB
     * collision tests. When a hit is detected:
     * - Applies damage to the enemy
     * - Destroys non-piercing projectiles
     * - Marks dead entities for destruction
     * 
     * @param world The ECS world
     * @param toDestroy Vector to accumulate entities to destroy
     */
    void checkProjectileVsEnemyCollisions(ECS::World& world, std::vector<ECS::EntityID>& toDestroy);
    
    /**
     * @brief Simple Axis-Aligned Bounding Box (AABB) collision test
     * 
     * Tests if two rectangular entities overlap using their center
     * positions and dimensions.
     * 
     * @param x1,y1 Center coordinates of first entity
     * @param w1,h1 Width and height of first entity
     * @param x2,y2 Center coordinates of second entity
     * @param w2,h2 Width and height of second entity
     * @return True if the entities overlap, false otherwise
     */
    bool checkAABB(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);
    
    /**
     * @brief Broadcasts entity destruction to all players in active games
     * 
     * Sends ENTITY_DESTROY packet to all players in rooms where the
     * game has started, ensuring clients remove the entity locally.
     * 
     * @param world The ECS world
     * @param entityId The ID of the entity being destroyed
     */
    void broadcastEntityDestroyToAllRooms(ECS::World& world, ECS::EntityID entityId);
};

} // namespace rtype::server::systems

#endif // SERVER_COLLISION_SYSTEM_H
