#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    enum class PowerUpType {
        Health, WeaponUpgrade, Shield, SpeedBoost, MultiShot, ScoreBonus
    };

    class PowerUp : public ECS::Component<PowerUp> {
    public:
        PowerUpType type;
        float duration;
        int value;
        bool consumed;

        PowerUp(PowerUpType type = PowerUpType::Health, float duration = 0.0f, int value = 25);
    };
}
