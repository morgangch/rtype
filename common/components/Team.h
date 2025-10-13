#ifndef COMMON_COMPONENTS_TEAM_HPP
#define COMMON_COMPONENTS_TEAM_HPP
#include <ECS/ECS.h>

namespace rtype::common::components {
    enum class TeamType {
        Neutral = 0, Player, Enemy, Environment
    };

    class Team : public ECS::Component<Team> {
    public:
        TeamType team;
        bool friendlyFire;

        Team(TeamType team = TeamType::Neutral, bool friendlyFire = false)
            : team(team), friendlyFire(friendlyFire) {}
    };
}
#endif // COMMON_COMPONENTS_TEAM_HPP
