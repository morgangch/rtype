#ifndef COMMON_POWERUP_HPP
#define COMMON_POWERUP_HPP

#include <ECS/ECS.h>

namespace rtype::common::components {
    enum class PowerUpType {
        Health, WeaponUpgrade, Shield, SpeedBoost, MultiShot, ScoreBonus
    };

    class PowerUp : public ECS::Component<PowerUp> {
    public:
        PowerUpType type;
        float duration;
        int value;
        bool consumed;

        PowerUp(PowerUpType type = PowerUpType::Health,
                float duration = 0.0f, int value = 25);
    };
}

#endif // COMMON_POWERUP_HPP
