#ifndef COMMON_PLAYER_HPP
#define COMMON_PLAYER_HPP

#include <ECS/ECS.h>

namespace rtype::common::components {
    class Player : public ECS::Component<Player> {
    public:
        Player() = default;
    };
}

#endif // COMMON_PLAYER_HPP
