/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef RTYPE_H
#define RTYPE_H
#include "packethandler.h"
#include "packetmanager.h"
#include "ECS/World.h"

namespace rtype::server {
    class Rtype {
    private:

    public:
        PacketManager packetManager;
        PacketHandler packetHandler;
        ECS::World world;

        int udp_server_fd;
        /**
         * @brief Looping into the network to receive and handle, and send packets.
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void loop(float deltaTime);
    };
};

/**
 * Global instance of the Rtype server
 */
inline rtype::server::Rtype root;

#endif //RTYPE_H
