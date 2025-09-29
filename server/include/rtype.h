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

namespace rtype::server {
    class Rtype {
    private:


    public:
        PacketManager packetManager;
        PacketHandler packetHandler;

        int udp_server_fd;
        void loop();
    };
};

inline rtype::server::Rtype root;

#endif //RTYPE_H
