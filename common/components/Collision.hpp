#ifndef COMMON_COLLISION_HPP
#define COMMON_COLLISION_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Collision : public ECS::Component<Collision> {
    public:
        float width;
        float height;
        float offsetX;
        float offsetY;
        bool isTrigger;

        Collision(float width = 32.0f,
                  float height = 32.0f,
                  float offsetX = 0.0f,
                  float offsetY = 0.0f,
                  bool isTrigger = false)
            : width(width),
              height(height),
              offsetX(offsetX),
              offsetY(offsetY),
              isTrigger(isTrigger) {}
    };
}

#endif // COMMON_COLLISION_HPP
