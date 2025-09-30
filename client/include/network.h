/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef NETWORK_H
#define NETWORK_H
#include <string>

#include "packethandler.h"
#include "packetmanager.h"

namespace rtype::client::network {
    void loop_recv();
    void loop_send();
    int start_room_connection(const std::string &server_ip, int server_port, const std::string &player_name, uint32_t room_id);
    int init_udp_socket(const std::string &server_ip, int server_port);

    extern PacketManager pm;
    extern PacketHandler ph;
    extern int udp_fd;
}

#endif //NETWORK_H
