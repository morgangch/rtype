#ifndef COMMON_COMPONENTS_TRANSFORM_HPP
#define COMMON_COMPONENTS_TRANSFORM_HPP
#include <ECS/ECS.h>

namespace rtype::common::components {
    class Transform : public ECS::Component<Transform> {
    public:
        float x, y, z;
        float rotation;
        float scaleX, scaleY;

        Transform(float x = 0.0f, float y = 0.0f, float rotation = 0.0f);
    };
}
#endif // COMMON_COMPONENTS_TRANSFORM_HPP
