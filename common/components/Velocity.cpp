#include "common/components/Velocity.hpp"

namespace rtype::common::components {
    Velocity::Velocity(float vx, float vy, float maxSpeed)
        : vx(vx), vy(vy), maxSpeed(maxSpeed) {}
}
