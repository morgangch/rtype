#pragma once
#include <cstdint>
#include "Entity.hpp"
#include "Component.hpp"

class ComponentManager;

class System {
public:
    virtual ~System() = default;
    virtual void update(ComponentManager& cm, float deltaTime) = 0;
};
