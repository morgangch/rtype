#ifndef COMMON_CORE_ENTITYTYPE_HPP
#define COMMON_CORE_ENTITYTYPE_HPP
#include <cstdint>

namespace Type {
    enum class Entity : uint8_t {
        None = 0,
        Player,
        Enemy,
        Projectile,
        PowerUp,
        Wall
    };

    enum class Projectile : uint8_t {
        Basic = 0,
        Laser,
        Missile,
        Spread,
        Piercing
    };
}
#endif // COMMON_CORE_ENTITYTYPE_HPP
