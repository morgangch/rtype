#pragma once
#include "common/ecs/World.hpp"

class CollisionSystem {
public:
    void update(World& world);

private:
    bool checkCollision(const Collision& a, const Collision& b);
    void resolveCollision(ComponentManager& cm, EntityID a, EntityID b);
};
