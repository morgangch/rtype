#ifndef COMMON_CORE_SYSTEM_HPP
#define COMMON_CORE_SYSTEM_HPP
#include <cstdint>
#include "Entity.hpp"
#include "../../lib/ecs/include/ECS/Component.hpp"

class ComponentManager;

class System {
public:
    virtual ~System() = default;
    virtual void update(ComponentManager& cm, float deltaTime) = 0;
};
#endif // COMMON_CORE_SYSTEM_HPP
