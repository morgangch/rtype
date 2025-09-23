#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    class Velocity : public ECS::Component<Velocity> {
    public:
        float vx, vy;
        float maxSpeed;

        Velocity(float vx = 0.0f, float vy = 0.0f, float maxSpeed = 100.0f);
    };
}
