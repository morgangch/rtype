/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef RTYPE_H
#define RTYPE_H
#include "packetmanager.h"

namespace rtype::server {
    class Rtype {
    private:


    public:
        PacketManager packetManager;
        int udp_server_fd;


    };
};


int setupUDPServer(int port);

inline rtype::server::Rtype root;

#endif //RTYPE_H
