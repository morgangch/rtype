/**
 * @file ECS.h
 * @brief Main header file for the Entity Component System library
 * @author R-Type Team
 * @date 2025
 *
 * This is the primary include file for the ECS library. It provides a single
 * point of access to all core ECS functionality including entities, components,
 * and world management. Include this file to access the complete ECS API.
 *
 * @example Basic ECS Usage
 * ```cpp
 * #include <ECS/ECS.h>
 *
 * // Define a component
 * class Position : public ECS::Component<Position> {
 * public:
 *     float x, y;
 *     Position(float x = 0, float y = 0) : x(x), y(y) {}
 * };
 *
 * // Use the ECS
 * ECS::World world;
 * auto entity = world.CreateEntity();
 * world.AddComponent<Position>(entity, 10.0f, 20.0f);
 * auto pos = world.GetComponent<Position>(entity);
 * ```
 */

#ifndef ECS_ECS_HPP
#define ECS_ECS_HPP

#include "Types.h"
#include "Component.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "System.h"
#include "SystemManager.h"
#include "World.h"

/**
 * @namespace ECS
 * @brief Entity Component System namespace
 *
 * Contains all ECS-related classes, types, and functionality for the R-Type game.
 * The ECS architecture separates data (Components) from behavior (Systems) and
 * uses Entities as unique identifiers to associate components together.
 *
 * Core concepts:
 * - **Entities**: Unique identifiers (EntityID) that represent game objects
 * - **Components**: Data containers that hold specific attributes (Position, Health, etc.)
 * - **Systems**: Logic processors that operate on entities with specific component combinations
 * - **World**: Central manager that coordinates entities, components, and systems
 */

#endif // ECS_ECS_HPP
