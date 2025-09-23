#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    class Transform : public ECS::Component<Transform> {
    public:
        float x, y, z;
        float rotation;
        float scaleX, scaleY;

        Transform(float x = 0.0f, float y = 0.0f, float rotation = 0.0f);
    };
}
