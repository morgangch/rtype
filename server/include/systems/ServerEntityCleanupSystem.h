/*
** ServerEntityCleanupSystem: detect dead entities and notify clients
*/
#ifndef SERVER_ENTITY_CLEANUP_SYSTEM_H
#define SERVER_ENTITY_CLEANUP_SYSTEM_H

#include "ECS/System.h"

class ServerEntityCleanupSystem : public ECS::System {
public:
    ServerEntityCleanupSystem() : ECS::System("ServerEntityCleanupSystem", 4) {}
    void Update(ECS::World &world, float deltaTime) override;
};

#endif // SERVER_ENTITY_CLEANUP_SYSTEM_H
