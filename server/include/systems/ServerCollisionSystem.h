/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Server-side collision detection system
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
 * @brief Server-authoritative collision detection system
 * 
 * Detects collisions between:
 * - Player projectiles vs enemies
 * - Enemy projectiles vs players
 * 
 * When collisions are detected, broadcasts ENTITY_DESTROY to all clients in the room.
 */
class ServerCollisionSystem : public ECS::System {
public:
    ServerCollisionSystem() : ECS::System("ServerCollisionSystem", 5) {}
    void Update(ECS::World& world, float deltaTime) override;

private:
    /**
     * @brief Check projectile vs enemy collisions
     * @param world The ECS world
     * @param toDestroy Entities to mark for destruction
     */
    void checkProjectileVsEnemyCollisions(ECS::World& world, std::vector<ECS::EntityID>& toDestroy);
    
    /**
     * @brief Simple AABB collision test
     * @param x1,y1 Center of first entity
     * @param w1,h1 Size of first entity
     * @param x2,y2 Center of second entity
     * @param w2,h2 Size of second entity
     * @return True if entities overlap
     */
    bool checkAABB(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);
    
    /**
     * @brief Broadcast entity destruction to all active game rooms
     * @param world The ECS world
     * @param entityId The entity to destroy
     */
    void broadcastEntityDestroyToAllRooms(ECS::World& world, ECS::EntityID entityId);
};

} // namespace rtype::server::systems

#endif // SERVER_COLLISION_SYSTEM_H
