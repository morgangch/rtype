#pragma once
#include <ECS/ECS.hpp>

namespace Common {
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
