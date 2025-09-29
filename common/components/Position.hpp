#ifndef COMMON_POSITION_HPP
#define COMMON_POSITION_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Position : public ECS::Component<Position> {
    public:
        float x, y;
        float rotation;

        Position(float x = 0.0f, float y = 0.0f, float rotation = 0.0f) 
            : x(x), y(y), rotation(rotation) {}
    };
}

#endif // COMMON_POSITION_HPP
