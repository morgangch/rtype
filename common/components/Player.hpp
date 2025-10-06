#ifndef COMMON_PLAYER_HPP
#define COMMON_PLAYER_HPP

#include <ECS/ECS.hpp>

namespace rtype::common::components {
    class Player : public ECS::Component<Player> {
    public:
        /**
         * @brief The room code the player is currently in.
         */
        unsigned int room_code;
        std::string name;

        /**
         * @brief The packet manager for the player.
         */
        PacketManager packet_manager;

        /**
         * @brief The packet handler for the player.
         */
        PacketHandler packet_handler;

    public:
        Player(std::string name, unsigned int room_code) : room_code(room_code) {
            this->name = name;

        }
    };
}

#endif // COMMON_PLAYER_HPP
