#pragma once
#include <ECS/ECS.hpp>

namespace Common {
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
