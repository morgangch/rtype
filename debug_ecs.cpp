#include <ECS/ECS.hpp>
#include <iostream>

// Test components
class TestPosition : public ECS::Component<TestPosition> {
public:
    float x, y;
    TestPosition(float x = 0.0f, float y = 0.0f) : x(x), y(y) {
        std::cout << "Creating Position: (" << x << ", " << y << ")" << std::endl;
    }
};

int main() {
    std::cout << "ECS Debug Test" << std::endl;

    ECS::World world;
    auto entity = world.CreateEntity();
    std::cout << "Created entity: " << entity << std::endl;

    // Test component addition
    auto* pos = world.AddComponent<TestPosition>(entity, 10.0f, 20.0f);
    std::cout << "Added position component at address: " << pos << std::endl;

    // Test component retrieval
    auto* retrieved = world.GetComponent<TestPosition>(entity);
    std::cout << "Retrieved position component at address: " << retrieved << std::endl;

    if (retrieved) {
        std::cout << "Retrieved position values: (" << retrieved->x << ", " << retrieved->y << ")" << std::endl;
    } else {
        std::cout << "Failed to retrieve position component!" << std::endl;
    }

    // Test HasComponent
    bool hasPos = world.HasComponent<TestPosition>(entity);
    std::cout << "HasComponent<TestPosition>: " << hasPos << std::endl;

    return 0;
}
