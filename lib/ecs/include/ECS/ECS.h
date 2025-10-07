#ifndef ECS_ECS_HPP
#define ECS_ECS_HPP

#include "Types.h"
#include "Component.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "World.h"

// Main ECS namespace with all core functionality
// Usage example:
// #include <ECS/ECS.hpp>
//
// class Position : public ECS::Component<Position> {
// public:
//     float x, y;
//     Position(float x = 0, float y = 0) : x(x), y(y) {}
// };
//
// ECS::World world;
// auto entity = world.CreateEntity();
// world.AddComponent<Position>(entity, 10.0f, 20.0f);
// auto pos = world.GetComponent<Position>(entity);

#endif // ECS_ECS_HPP
