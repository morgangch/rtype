#ifndef SERVER_COLLISION_SYSTEM_HPP
#define SERVER_COLLISION_SYSTEM_HPP

#include "common/ecs/World.hpp"

class CollisionSystem {
public:
    void update(World& world);

private:
    bool checkCollision(const Collision& a, const Collision& b);
    void resolveCollision(ComponentManager& cm, EntityID a, EntityID b);
};

#endif // SERVER_COLLISION_SYSTEM_HPP
