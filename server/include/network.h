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
    /**
     * @brief Loop to receive UDP packets and store them in the PacketManager.
     * @param fd  File descriptor of the UDP socket
     * If the ip source is already assigned to a player, redirect the packet to the player's personal PacketManager.
     * @return void
     */
    void loop_recv(int fd);

    /**
     * @brief Loop to send UDP packets from the PacketManager.
     * @param fd File descriptor of the UDP socket
     * @return void
     */
    void loop_send(int fd);

    /**
     * @brief Setup a UDP server socket on the given port.
     * @param port
     * @return  File descriptor of the created UDP socket
     */
    int setupUDPServer(int port);
}

#endif //NETWORK_H
