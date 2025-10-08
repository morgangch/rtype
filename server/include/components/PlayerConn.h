/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef NETWORKADDRESS_H
#define NETWORKADDRESS_H

#include <string>
#include "ECS/Component.h"

namespace rtype::server::components {
    class PlayerConn : public ECS::Component<PlayerConn> {
    public:
        /**
        * @brief The packet manager for the player.
        */
        PacketManager packet_manager;

        /**
         * @brief The packet handler for the player.
         */
        PacketHandler packet_handler;
        /**
         * @brief The IP address of the player.
         */
        std::string address;

        /**
         * @brief The port of the player.
         */
        int port;

        /**
         * @brief The room code the player is currently in.
         */
        unsigned int room_code;

        PlayerConn(std::string address = "", int port = 0, unsigned int room_code = 0)
            : address(address), port(port) , room_code(room_code) {
        };
    };
}

#endif //NETWORKADDRESS_H
