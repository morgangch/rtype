#pragma once

#include "Types.hpp"
#include "Component.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "World.hpp"

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
