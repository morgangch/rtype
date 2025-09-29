#ifndef COMMON_SCORE_HPP
#define COMMON_SCORE_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Score : public ECS::Component<Score> {
    public:
        int points;
        int kills;
        int deaths;
        int combo;
        float comboTimer;
        int highestCombo;

        Score(int points = 0, int kills = 0, int deaths = 0);
    };
}

#endif // COMMON_SCORE_HPP
