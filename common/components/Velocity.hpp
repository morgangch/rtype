#ifndef COMMON_COMPONENTS_VELOCITY_HPP
#define COMMON_COMPONENTS_VELOCITY_HPP
#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Velocity : public ECS::Component<Velocity> {
    public:
        float vx, vy;
        float maxSpeed;

        Velocity(float vx = 0.0f, float vy = 0.0f, float maxSpeed = 100.0f);
    };
}
#endif // COMMON_COMPONENTS_VELOCITY_HPP
