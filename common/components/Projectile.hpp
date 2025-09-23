#pragma once
#include <ECS/ECS.hpp>

namespace Common {
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

        Projectile(int damage = 10, float speed = 200.0f, ProjectileType type = ProjectileType::Basic);
    };
}
