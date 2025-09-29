/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef NETWORK_H
#define NETWORK_H
#include <string>
#include "packet.h"


namespace rtype::server::network {
    void loop_recv(int fd);
    void loop_send(int fd);

    int setupUDPServer(int port);
}

#endif //NETWORK_H
