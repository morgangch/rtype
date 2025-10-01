/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef NETWORKADDRESS_H
#define NETWORKADDRESS_H

#include <string>
#include "ECS/Component.hpp"

namespace rtype::server::components {
    class NetworkAddress : public ECS::Component<NetworkAddress> {
    public:
        std::string address;
        int port;
        NetworkAddress(std::string address = "", int port = 0)
            : address(address), port(port) {
        };
    };
}

#endif //NETWORKADDRESS_H
