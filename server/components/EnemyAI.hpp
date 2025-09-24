#ifndef COMMON_ENEMY_AI_HPP
#define COMMON_ENEMY_AI_HPP

#include <ECS/ECS.hpp>

namespace Common {
    enum class AIPattern {
        Straight, Sine, Zigzag, Circle, Follow, Random, Stationary
    };

    class EnemyAI : public ECS::Component<EnemyAI> {
    public:
        AIPattern pattern;
        float aggressiveness;
        float detectionRange;
        float fireRate;
        float lastFireTime;
        float patternTimer;
        float baseSpeed;
        ECS::EntityID targetEntity;

        EnemyAI(AIPattern pattern = AIPattern::Straight, float aggressiveness = 0.5f);
    };
}

#endif // COMMON_ENEMY_AI_HPP
