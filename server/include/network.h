/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Network utility functions for UDP server setup and packet handling
*/
#ifndef NETWORK_H
#define NETWORK_H
#include <string>
#include "packet.h"


namespace rtype::server::network {
    /**
     * @brief Receive UDP packets and route them to appropriate PacketManagers
     * 
     * Continuously receives UDP packets on the server socket and:
     * - Identifies the source player by IP/port
     * - Routes packets to the player's personal PacketManager
     * - Stores unassigned packets in the global PacketManager
     * 
     * This function blocks and should run in a dedicated thread.
     * 
     * @param fd File descriptor of the UDP socket
     */
    void loop_recv(int fd);

    /**
     * @brief Send queued UDP packets from all PacketManagers
     * 
     * Processes outgoing packets from:
     * - Global PacketManager (broadcast packets)
     * - Individual player PacketManagers
     * 
     * This function blocks and should run in a dedicated thread.
     * 
     * @param fd File descriptor of the UDP socket
     */
    void loop_send(int fd);

    /**
     * @brief Create and configure a UDP server socket
     * 
     * Sets up a non-blocking UDP socket bound to the specified port,
     * ready to receive and send packets.
     * 
     * @param port Port number to bind the server to
     * @return File descriptor of the created UDP socket, or -1 on error
     */
    int setupUDPServer(int port);
}

#endif //NETWORK_H
