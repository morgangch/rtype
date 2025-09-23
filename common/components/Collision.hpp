#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    enum class CollisionMask : uint32_t {
        None = 0,
        Player = 1 << 0,
        Enemy = 1 << 1,
        Projectile = 1 << 2,
        PowerUp = 1 << 3,
        Wall = 1 << 4
    };

    class Collision : public ECS::Component<Collision> {
    public:
        float width, height;
        float offsetX, offsetY;
        CollisionMask mask;
        bool isTrigger;

        Collision(float width = 32.0f, float height = 32.0f, CollisionMask mask = CollisionMask::None);
    };
}
