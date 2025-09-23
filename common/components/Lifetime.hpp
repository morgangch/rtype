#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    class Lifetime : public ECS::Component<Lifetime> {
    public:
        float maxLifetime;
        float currentLifetime;
        bool shouldDestroy;

        Lifetime(float maxLifetime = 5.0f);
    };
}
