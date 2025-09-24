#ifndef COMMON_PLAYER_HPP
#define COMMON_PLAYER_HPP

#include <ECS/ECS.hpp>

namespace Common {
    class Player : public ECS::Component<Player> {
    public:
        Player() = default;
    };
}

#endif // COMMON_PLAYER_HPP
