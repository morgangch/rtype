#pragma once
#include <ECS/ECS.hpp>

namespace Common {
    enum class TeamType {
        Neutral = 0, Player, Enemy, Environment
    };

    class Team : public ECS::Component<Team> {
    public:
        TeamType team;
        bool friendlyFire;

        Team(TeamType team = TeamType::Neutral, bool friendlyFire = false);
    };
}
