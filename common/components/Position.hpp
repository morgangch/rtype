#ifndef COMMON_POSITION_HPP
#define COMMON_POSITION_HPP

#include <ECS/ECS.hpp>

namespace Common {
    class Position : public ECS::Component<Position> {
    public:
        float x, y;
        float rotation;

        Position(float x = 0.0f, float y = 0.0f, float rotation = 0.0f);
    };
}

#endif // COMMON_POSITION_HPP
