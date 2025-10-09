#ifndef COMMON_PROJECTILE_HPP
#define COMMON_PROJECTILE_HPP

#include <ECS/ECS.h>

namespace rtype::common::components {
    enum class ProjectileType {
        Basic, Laser, Missile, Spread, Piercing
    };

    class Projectile : public ECS::Component<Projectile> {
    public:
        int damage;
        float speed;
        float maxDistance;
        float distanceTraveled;
        ProjectileType type;
        ECS::EntityID ownerId;

        Projectile(int damage = 10,
                   float speed = 200.0f,
                   ProjectileType type = ProjectileType::Basic)
            : damage(damage), speed(speed), maxDistance(1000.0f), distanceTraveled(0.0f), type(type), ownerId(0) {}
    };
}

#endif // COMMON_PROJECTILE_HPP
