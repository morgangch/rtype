#ifndef COMMON_PLAYER_HPP
#define COMMON_PLAYER_HPP

#include <ECS/ECS.hpp>
#include <utility>

namespace rtype::common::components {
    class Player : public ECS::Component<Player> {
    public:
        /**
         * @brief The name of the player.
         */
        std::string name;

        /**
         * @brief The server ID of the player.
         * Used to identify the player on the client side.
         */
        unsigned int serverId = 0;

    public:
        explicit Player(std::string name, unsigned int serverId = 0) : name(std::move(name)), serverId(serverId) {}
    };
}

#endif // COMMON_PLAYER_HPP
