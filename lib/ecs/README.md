# RType ECS Library

A lightweight Entity Component System (ECS) library designed for game development, shared between client and server.

## Features

- **Entity Management**: Create, destroy, and track entities
- **Component System**: Type-safe component addition, removal, and querying
- **Memory Efficient**: Smart pointer-based component storage
- **Thread-Safe Type IDs**: Centralized component type registry
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Basic Usage

### 1. Include the library
```cpp
#include <ECS/ECS.hpp>
```

### 2. Define Components
```cpp
class Position : public ECS::Component<Position> {
public:
    float x, y;
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

class Velocity : public ECS::Component<Velocity> {
public:
    float vx, vy;
    Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};
```

### 3. Create World and Entities
```cpp
ECS::World world;
auto player = world.CreateEntity();
auto enemy = world.CreateEntity();
```

### 4. Add Components
```cpp
world.AddComponent<Position>(player, 10.0f, 20.0f);
world.AddComponent<Velocity>(player, 1.5f, 0.0f);
world.AddComponent<Position>(enemy, 50.0f, 30.0f);
```

### 5. Query Components
```cpp
// Get a component
auto* pos = world.GetComponent<Position>(player);
if (pos) {
    std::cout << "Player at: " << pos->x << ", " << pos->y << std::endl;
}

// Check if entity has component
if (world.HasComponent<Velocity>(player)) {
    std::cout << "Player can move!" << std::endl;
}
```

### 6. Iterate Over Components
```cpp
auto* positions = world.GetAllComponents<Position>();
for (const auto& pair : *positions) {
    ECS::EntityID entity = pair.first;
    auto* pos = pair.second.get();
    std::cout << "Entity " << entity << " at (" << pos->x << ", " << pos->y << ")" << std::endl;
}
```

### 7. Remove Components and Entities
```cpp
world.RemoveComponent<Velocity>(player);
world.DestroyEntity(enemy);
```

## Building

The ECS library is built as a static library and linked automatically to both client and server:

```bash
mkdir build && cd build
cmake ..
make
```

## Testing

Run the ECS tests to verify functionality:

```bash
# Run specific ECS tests
./tests/test_ecs

# Run all tests
ctest --verbose
```

## Architecture

- **World**: Main ECS coordinator that manages entities and components
- **EntityManager**: Handles entity creation, destruction, and lifecycle
- **ComponentManager**: Manages component storage and retrieval
- **Component<T>**: Base class for all components with type-safe IDs
- **ComponentTypeRegistry**: Centralized type ID management for thread safety

The library uses a component-based architecture where:
- **Entities** are unique identifiers (EntityID)
- **Components** are data containers that inherit from Component<T>
- **Systems** are implemented by user code that queries and processes components

## Example: Simple Game Loop

```cpp
// Game initialization
ECS::World world;
auto player = world.CreateEntity();
world.AddComponent<Position>(player, 0.0f, 0.0f);
world.AddComponent<Velocity>(player, 5.0f, 0.0f);

// Game loop
while (gameRunning) {
    // Movement system
    auto* positions = world.GetAllComponents<Position>();
    for (const auto& pair : *positions) {
        ECS::EntityID entity = pair.first;
        auto* pos = pair.second.get();
        auto* vel = world.GetComponent<Velocity>(entity);
        
        if (vel) {
            pos->x += vel->vx * deltaTime;
            pos->y += vel->vy * deltaTime;
        }
    }
    
    // Render system (client-side only)
    for (const auto& pair : *positions) {
        ECS::EntityID entity = pair.first;
        auto* pos = pair.second.get();
        auto* renderable = world.GetComponent<Renderable>(entity);
        
        if (renderable) {
            renderer.draw(renderable->sprite, pos->x, pos->y);
        }
    }
}
```

This ECS library provides a solid foundation for building scalable game architectures where game logic can be shared between client and server while maintaining clean separation of concerns.
