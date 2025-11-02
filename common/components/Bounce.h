/**
 * @file Bounce.h
 * @brief Component to enable simple border bounce for projectiles
 */

#ifndef COMMON_COMPONENTS_BOUNCE_H
#define COMMON_COMPONENTS_BOUNCE_H

#include <ECS/ECS.h>

namespace rtype::common::components {

    class Bounce : public ECS::Component<Bounce> {
    public:
        int remaining;   // number of reflections left (-1 for infinite)
        bool bounceX;    // reflect on left/right borders
        bool bounceY;    // reflect on top/bottom borders

        explicit Bounce(int remaining = 4, bool bx = false, bool by = true)
            : remaining(remaining), bounceX(bx), bounceY(by) {}
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_BOUNCE_H
