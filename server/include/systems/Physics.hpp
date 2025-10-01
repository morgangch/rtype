#ifndef SERVER_PHYSICS_HPP
#define SERVER_PHYSICS_HPP

#include "common/ecs/World.hpp"

class Physics {
public:
    void update(World& world, float dt);
};

#endif // SERVER_PHYSICS_HPP
