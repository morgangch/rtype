#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    class Position : public ECS::Component<Position> {
    public:
        float x, y;
        float rotation;

        Position(float x = 0.0f, float y = 0.0f, float rotation = 0.0f);
    };
}
