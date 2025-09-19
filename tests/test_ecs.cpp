#include <ECS/ECS.hpp>
#include <iostream>

// Test components
class TestPosition : public ECS::Component<TestPosition> {
public:
    float x, y;
    TestPosition(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

class TestVelocity : public ECS::Component<TestVelocity> {
public:
    float vx, vy;
    TestVelocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};

class TestHealth : public ECS::Component<TestHealth> {
public:
    int hp;
    TestHealth(int hp = 100) : hp(hp) {}
};

int main() {
    std::cout << "Running ECS Library Tests..." << std::endl;

    bool allTestsPassed = true;

    // Test 1: Entity creation and management
    {
        ECS::World world;
        auto entity1 = world.CreateEntity();
        auto entity2 = world.CreateEntity();

        if (entity1 == ECS::INVALID_ENTITY || entity2 == ECS::INVALID_ENTITY) {
            std::cout << "FAIL: Entity creation test" << std::endl;
            allTestsPassed = false;
        } else if (entity1 == entity2) {
            std::cout << "FAIL: Entity uniqueness test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Entity creation and uniqueness" << std::endl;
        }

        if (world.GetAliveEntityCount() != 2) {
            std::cout << "FAIL: Entity count test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Entity count" << std::endl;
        }
    }

    // Test 2: Component addition and retrieval
    {
        ECS::World world;
        auto entity = world.CreateEntity();

        auto* pos = world.AddComponent<TestPosition>(entity, 10.0f, 20.0f);
        auto* vel = world.AddComponent<TestVelocity>(entity, 1.0f, 2.0f);

        if (!pos || pos->x != 10.0f || pos->y != 20.0f) {
            std::cout << "FAIL: Position component test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Position component addition and retrieval" << std::endl;
        }

        if (!vel || vel->vx != 1.0f || vel->vy != 2.0f) {
            std::cout << "FAIL: Velocity component test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Velocity component addition and retrieval" << std::endl;
        }
    }

    // Test 3: Component queries (HasComponent)
    {
        ECS::World world;
        auto entity = world.CreateEntity();

        world.AddComponent<TestPosition>(entity, 5.0f, 15.0f);

        if (!world.HasComponent<TestPosition>(entity)) {
            std::cout << "FAIL: HasComponent positive test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: HasComponent positive test" << std::endl;
        }

        if (world.HasComponent<TestVelocity>(entity)) {
            std::cout << "FAIL: HasComponent negative test" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: HasComponent negative test" << std::endl;
        }
    }

    // Test 4: Component removal
    {
        ECS::World world;
        auto entity = world.CreateEntity();

        world.AddComponent<TestPosition>(entity, 0.0f, 0.0f);
        world.AddComponent<TestHealth>(entity, 50);

        world.RemoveComponent<TestPosition>(entity);

        if (world.HasComponent<TestPosition>(entity)) {
            std::cout << "FAIL: Component removal test" << std::endl;
            allTestsPassed = false;
        } else if (!world.HasComponent<TestHealth>(entity)) {
            std::cout << "FAIL: Component removal affecting other components" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Component removal" << std::endl;
        }
    }

    // Test 5: Entity destruction
    {
        ECS::World world;
        auto entity1 = world.CreateEntity();
        auto entity2 = world.CreateEntity();

        world.AddComponent<TestPosition>(entity1, 1.0f, 2.0f);
        world.AddComponent<TestPosition>(entity2, 3.0f, 4.0f);

        world.DestroyEntity(entity1);

        if (world.IsEntityAlive(entity1)) {
            std::cout << "FAIL: Entity destruction test" << std::endl;
            allTestsPassed = false;
        } else if (!world.IsEntityAlive(entity2)) {
            std::cout << "FAIL: Entity destruction affecting other entities" << std::endl;
            allTestsPassed = false;
        } else if (world.HasComponent<TestPosition>(entity1)) {
            std::cout << "FAIL: Component cleanup on entity destruction" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Entity destruction and component cleanup" << std::endl;
        }
    }

    // Test 6: Multiple component types on single entity
    {
        ECS::World world;
        auto entity = world.CreateEntity();

        world.AddComponent<TestPosition>(entity, 100.0f, 200.0f);
        world.AddComponent<TestVelocity>(entity, 10.0f, 20.0f);
        world.AddComponent<TestHealth>(entity, 75);

        auto* pos = world.GetComponent<TestPosition>(entity);
        auto* vel = world.GetComponent<TestVelocity>(entity);
        auto* health = world.GetComponent<TestHealth>(entity);

        if (!pos || !vel || !health) {
            std::cout << "FAIL: Multiple components on single entity" << std::endl;
            allTestsPassed = false;
        } else if (pos->x != 100.0f || vel->vx != 10.0f || health->hp != 75) {
            std::cout << "FAIL: Multiple component values" << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASS: Multiple components on single entity" << std::endl;
        }
    }

    if (allTestsPassed) {
        std::cout << "\nAll ECS tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome ECS tests failed!" << std::endl;
        return 1;
    }
}
