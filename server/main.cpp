#include <iostream>
#include <ECS/ECS.hpp>

// Example components for demonstration
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

class Health : public ECS::Component<Health> {
public:
    int hp;
    Health(int hp = 100) : hp(hp) {}
};

int main() {
    std::cout << "Hello World from Server!" << std::endl;
    std::cout << "Server is running..." << std::endl;

    // Demonstrate ECS library usage
    std::cout << "\n=== ECS Library Demo (Server) ===" << std::endl;

    ECS::World world;

    // Create some entities
    auto player = world.CreateEntity();
    auto enemy1 = world.CreateEntity();
    auto enemy2 = world.CreateEntity();

    std::cout << "Created entities: Player(" << player << "), Enemy1(" << enemy1 << "), Enemy2(" << enemy2 << ")" << std::endl;

    // Add components to player
    world.AddComponent<Position>(player, 10.0f, 20.0f);
    world.AddComponent<Velocity>(player, 1.5f, 0.0f);
    world.AddComponent<Health>(player, 100);

    // Add components to enemies
    world.AddComponent<Position>(enemy1, 50.0f, 30.0f);
    world.AddComponent<Health>(enemy1, 50);

    world.AddComponent<Position>(enemy2, 75.0f, 45.0f);
    world.AddComponent<Velocity>(enemy2, -1.0f, 0.5f);
    world.AddComponent<Health>(enemy2, 75);

    std::cout << "Added components to entities" << std::endl;

    // Query and display component data
    if (auto pos = world.GetComponent<Position>(player)) {
        std::cout << "Player position: (" << pos->x << ", " << pos->y << ")" << std::endl;
    }

    if (auto health = world.GetComponent<Health>(player)) {
        std::cout << "Player health: " << health->hp << std::endl;
    }

    // Demonstrate component queries
    std::cout << "Player has Velocity: " << (world.HasComponent<Velocity>(player) ? "Yes" : "No") << std::endl;
    std::cout << "Enemy1 has Velocity: " << (world.HasComponent<Velocity>(enemy1) ? "Yes" : "No") << std::endl;

    std::cout << "Total alive entities: " << world.GetAliveEntityCount() << std::endl;

    return 0;
}
