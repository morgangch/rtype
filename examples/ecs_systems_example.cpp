/**
 * @file ecs_systems_example.cpp
 * @brief Comprehensive example demonstrating ECS system management
 * @author R-Type Team
 * @date 2025
 *
 * This example shows how to:
 * - Create custom systems
 * - Register systems to the ECS world
 * - Enable and disable systems
 * - Set system priorities
 * - Remove systems
 * - Update systems in the game loop
 */

#include <ECS/ECS.h>
#include <iostream>
#include <chrono>
#include <thread>

// Example Components
class Position : public ECS::Component<Position> {
public:
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

class Velocity : public ECS::Component<Velocity> {
public:
    float vx, vy;
    Velocity(float vx = 0, float vy = 0) : vx(vx), vy(vy) {}
};

class Health : public ECS::Component<Health> {
public:
    int currentHealth, maxHealth;
    Health(int maxHealth = 100) : currentHealth(maxHealth), maxHealth(maxHealth) {}
};

class Name : public ECS::Component<Name> {
public:
    std::string name;
    Name(const std::string& name = "Entity") : name(name) {}
};

// Example Systems

/**
 * @brief Movement system that updates positions based on velocity
 */
class MovementSystem : public ECS::System {
public:
    MovementSystem() : ECS::System("MovementSystem", 10) {}

    void Update(ECS::World& world, float deltaTime) override {
        auto* positions = world.GetAllComponents<Position>();
        auto* velocities = world.GetAllComponents<Velocity>();

        if (!positions || !velocities) return;

        // Iterate through all entities that have both Position and Velocity
        for (ECS::EntityID entity : world.GetAllEntities()) {
            if (world.HasComponent<Position>(entity) && world.HasComponent<Velocity>(entity)) {
                auto* pos = world.GetComponent<Position>(entity);
                auto* vel = world.GetComponent<Velocity>(entity);

                pos->x += vel->vx * deltaTime;
                pos->y += vel->vy * deltaTime;
            }
        }
    }

    void Initialize(ECS::World& world) override {
        std::cout << "MovementSystem initialized!\n";
    }

    void Cleanup(ECS::World& world) override {
        std::cout << "MovementSystem cleaned up!\n";
    }
};

/**
 * @brief Health system that manages entity health
 */
class HealthSystem : public ECS::System {
private:
    float m_timer = 0.0f;

public:
    HealthSystem() : ECS::System("HealthSystem", 20) {}

    void Update(ECS::World& world, float deltaTime) override {
        m_timer += deltaTime;

        // Regenerate health every 2 seconds
        if (m_timer >= 2.0f) {
            m_timer = 0.0f;

            for (ECS::EntityID entity : world.GetAllEntities()) {
                if (world.HasComponent<Health>(entity)) {
                    auto* health = world.GetComponent<Health>(entity);
                    if (health->currentHealth < health->maxHealth) {
                        health->currentHealth = std::min(health->currentHealth + 10, health->maxHealth);
                    }
                }
            }
        }
    }

    void Initialize(ECS::World& world) override {
        std::cout << "HealthSystem initialized!\n";
    }

    void Cleanup(ECS::World& world) override {
        std::cout << "HealthSystem cleaned up!\n";
    }
};

/**
 * @brief Debug system that prints entity information
 */
class DebugSystem : public ECS::System {
private:
    float m_printTimer = 0.0f;

public:
    DebugSystem() : ECS::System("DebugSystem", 100) {} // Low priority (runs last)

    void Update(ECS::World& world, float deltaTime) override {
        m_printTimer += deltaTime;

        // Print debug info every 3 seconds
        if (m_printTimer >= 3.0f) {
            m_printTimer = 0.0f;

            std::cout << "\n=== Debug Info ===\n";
            std::cout << "Entities: " << world.GetAliveEntityCount() << "\n";
            std::cout << "Systems: " << world.GetSystemCount() << "\n";

            for (ECS::EntityID entity : world.GetAllEntities()) {
                if (world.HasComponent<Name>(entity)) {
                    auto* name = world.GetComponent<Name>(entity);
                    std::cout << "Entity: " << name->name << " (ID: " << entity << ")\n";

                    if (world.HasComponent<Position>(entity)) {
                        auto* pos = world.GetComponent<Position>(entity);
                        std::cout << "  Position: (" << pos->x << ", " << pos->y << ")\n";
                    }

                    if (world.HasComponent<Velocity>(entity)) {
                        auto* vel = world.GetComponent<Velocity>(entity);
                        std::cout << "  Velocity: (" << vel->vx << ", " << vel->vy << ")\n";
                    }

                    if (world.HasComponent<Health>(entity)) {
                        auto* health = world.GetComponent<Health>(entity);
                        std::cout << "  Health: " << health->currentHealth << "/" << health->maxHealth << "\n";
                    }
                }
            }
            std::cout << "==================\n\n";
        }
    }

    void Initialize(ECS::World& world) override {
        std::cout << "DebugSystem initialized!\n";
    }

    void Cleanup(ECS::World& world) override {
        std::cout << "DebugSystem cleaned up!\n";
    }
};

int main() {
    std::cout << "=== ECS System Management Example ===\n\n";

    // Create ECS World
    ECS::World world;

    // Create some entities with components
    std::cout << "Creating entities...\n";

    auto player = world.CreateEntity();
    world.AddComponent<Name>(player, "Player");
    world.AddComponent<Position>(player, 0.0f, 0.0f);
    world.AddComponent<Velocity>(player, 50.0f, 30.0f);
    world.AddComponent<Health>(player, 100);

    auto enemy = world.CreateEntity();
    world.AddComponent<Name>(enemy, "Enemy");
    world.AddComponent<Position>(enemy, 100.0f, 50.0f);
    world.AddComponent<Velocity>(enemy, -20.0f, 10.0f);
    world.AddComponent<Health>(enemy, 75);

    auto npc = world.CreateEntity();
    world.AddComponent<Name>(npc, "NPC");
    world.AddComponent<Position>(npc, 200.0f, 100.0f);
    world.AddComponent<Health>(npc, 50);

    std::cout << "Created " << world.GetAliveEntityCount() << " entities.\n\n";

    // Register systems
    std::cout << "Registering systems...\n";
    auto* movementSystem = world.RegisterSystem<MovementSystem>();
    auto* healthSystem = world.RegisterSystem<HealthSystem>();
    auto* debugSystem = world.RegisterSystem<DebugSystem>();

    std::cout << "Registered " << world.GetSystemCount() << " systems.\n";
    std::cout << "System names: ";
    for (const auto& name : world.GetSystemNames()) {
        std::cout << name << " ";
    }
    std::cout << "\n\n";

    // Game loop simulation
    std::cout << "Starting game loop simulation...\n";
    auto lastTime = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < 50; ++frame) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Demonstrate system management at different points
        if (frame == 10) {
            std::cout << "\n[Frame " << frame << "] Disabling MovementSystem...\n";
            world.DisableSystem("MovementSystem");
        }

        if (frame == 20) {
            std::cout << "\n[Frame " << frame << "] Re-enabling MovementSystem...\n";
            world.EnableSystem("MovementSystem");
        }

        if (frame == 30) {
            std::cout << "\n[Frame " << frame << "] Changing DebugSystem priority to 5 (higher priority)...\n";
            world.SetSystemPriority("DebugSystem", 5);
        }

        if (frame == 40) {
            std::cout << "\n[Frame " << frame << "] Removing HealthSystem...\n";
            world.RemoveSystem("HealthSystem");
        }

        // Update all systems
        world.UpdateSystems(deltaTime);

        // Simulate frame delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nFinal system count: " << world.GetSystemCount() << "\n";
    std::cout << "Remaining systems: ";
    for (const auto& name : world.GetSystemNames()) {
        std::cout << name << " ";
    }
    std::cout << "\n\n";

    // Demonstrate manual system retrieval and manipulation
    std::cout << "Manual system manipulation:\n";
    auto* movement = world.GetSystem<MovementSystem>();
    if (movement) {
        std::cout << "MovementSystem found: " << movement->GetName()
                  << " (Priority: " << movement->GetPriority()
                  << ", Enabled: " << (movement->IsEnabled() ? "Yes" : "No") << ")\n";
    }

    // Clear the world (this will clean up all systems)
    std::cout << "\nClearing world...\n";
    world.Clear();

    std::cout << "Final entity count: " << world.GetAliveEntityCount() << "\n";
    std::cout << "Final system count: " << world.GetSystemCount() << "\n";

    std::cout << "\n=== Example Complete ===\n";
    return 0;
}
