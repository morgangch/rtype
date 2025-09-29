#ifndef COMMON_LIFETIME_HPP
#define COMMON_LIFETIME_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Lifetime : public ECS::Component<Lifetime> {
    public:
        float maxLifetime;
        float currentLifetime;
        bool shouldDestroy;

        Lifetime(float maxLifetime = 5.0f);
    };
}

#endif // COMMON_LIFETIME_HPP
