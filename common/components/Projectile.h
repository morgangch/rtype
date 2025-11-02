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
        bool piercing;     ///< If true, projectile passes through enemies (charged shot)
        bool serverOwned;  ///< If true, projectile is server-authoritative (client-side collision is prediction only)

        Projectile(int damage = 10,
                   bool piercing = false,
                   bool serverOwned = false,
                   float speed = 200.0f,
                   ProjectileType type = ProjectileType::Basic)
            : damage(damage), speed(speed), maxDistance(1000.0f), 
              distanceTraveled(0.0f), type(type), ownerId(0), piercing(piercing), serverOwned(serverOwned) {}
    };
}

#endif // COMMON_PROJECTILE_HPP
