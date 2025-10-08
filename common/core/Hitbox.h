#ifndef COMMON_CORE_HITBOX_HPP
#define COMMON_CORE_HITBOX_HPP
#include <cstdint>

namespace Common {
    class Hitbox {
    public:
        float x, y;
        float width, height;

        Hitbox(float w = 32.0f, float h = 32.0f, float offsetX = 0.0f, float offsetY = 0.0f)
            : x(offsetX), y(offsetY), width(w), height(h) {}

        bool intersects(const Hitbox& other, float ax, float ay, float bx, float by) const {
            return !(ax + x + width  < bx + other.x ||
                     ax + x          > bx + other.x + other.width ||
                     ay + y + height < by + other.y ||
                     ay + y          > by + other.y + other.height);
        }
    };
}
#endif // COMMON_CORE_HITBOX_HPP
