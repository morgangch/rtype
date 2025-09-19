#include <iostream>
#include <ECS/ECS.hpp>

// Example components for demonstration (same as server for compatibility)
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

class Renderable : public ECS::Component<Renderable> {
public:
    std::string sprite;
    Renderable(const std::string& sprite = "default") : sprite(sprite) {}
};

int main() {
    std::cout << "Hello World from Client!" << std::endl;
    std::cout << "Client is connecting..." << std::endl;

    // Demonstrate ECS library usage
    std::cout << "\n=== ECS Library Demo (Client) ===" << std::endl;

    ECS::World world;

    // Create client-side entities (UI elements, local objects)
    auto localPlayer = world.CreateEntity();
    auto uiElement = world.CreateEntity();
    auto projectile = world.CreateEntity();

    std::cout << "Created client entities: LocalPlayer(" << localPlayer << "), UI(" << uiElement << "), Projectile(" << projectile << ")" << std::endl;

    // Add components for client-specific functionality
    world.AddComponent<Position>(localPlayer, 0.0f, 0.0f);
    world.AddComponent<Velocity>(localPlayer, 0.0f, 0.0f);
    world.AddComponent<Renderable>(localPlayer, "player_sprite.png");

    world.AddComponent<Position>(uiElement, 10.0f, 10.0f);
    world.AddComponent<Renderable>(uiElement, "ui_button.png");

    world.AddComponent<Position>(projectile, 15.0f, 25.0f);
    world.AddComponent<Velocity>(projectile, 5.0f, 0.0f);
    world.AddComponent<Renderable>(projectile, "bullet.png");

    std::cout << "Added components to client entities" << std::endl;

    // Simulate client-side updates
    if (auto pos = world.GetComponent<Position>(localPlayer)) {
        std::cout << "Local player position: (" << pos->x << ", " << pos->y << ")" << std::endl;
    }

    if (auto renderable = world.GetComponent<Renderable>(localPlayer)) {
        std::cout << "Local player sprite: " << renderable->sprite << std::endl;
    }

    // Iterate through all renderable objects (typical client-side rendering loop)
    auto* renderables = world.GetAllComponents<Renderable>();
    if (renderables) {
        std::cout << "Rendering objects:" << std::endl;
        for (const auto& pair : *renderables) {
            ECS::EntityID entity = pair.first;
            auto* renderable = pair.second.get();
            auto* pos = world.GetComponent<Position>(entity);

            std::cout << "  Entity " << entity << ": " << renderable->sprite;
            if (pos) {
                std::cout << " at (" << pos->x << ", " << pos->y << ")";
            }
            std::cout << std::endl;
        }
    }

    std::cout << "Total alive entities: " << world.GetAliveEntityCount() << std::endl;

    return 0;
}
