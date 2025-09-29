#ifndef COMMON_HEALTH_HPP
#define COMMON_HEALTH_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Health : public ECS::Component<Health> {
    public:
        int currentHp;
        int maxHp;
        bool isAlive;
        bool invulnerable;
        float invulnerabilityTimer;

        Health(int maxHp = 100);
    };
}

#endif // COMMON_HEALTH_HPP
