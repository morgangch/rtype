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
    /**
     * @brief Loop to receive packets from the server and handle it on the PacketManager.
     * @warning Call that function only if the UDP socket is initialized.
     */
    void loop_recv();

    /**
     * @brief Loop to send packets from the PacketManager to the server.
     * @warning Call that function only if the UDP socket is initialized.
     */
    void loop_send();

    /**
     * @brief Start a connection to the server to get a room list.
     *  The function will initialize the UDP socket by calling init_udp_socket() and sending a RoomJoinPacket.
     * @param server_ip The server IP address.
     * @param server_port The server port.
     * @param player_name The player name.
     * @param room_code The room code to join.
     * @return fd of the UDP socket, or -1 on error.
     */
    int start_room_connection(const std::string &server_ip, int server_port, const std::string &player_name,
                              uint32_t room_code);

    /**
     * @brief Initializes a UDP socket for communication with the server.
     * @param server_ip The IP address of the server.
     * @param server_port The port number of the server.
     * @return The file descriptor of the UDP socket on success, or -1 on error.
     */
    int init_udp_socket(const std::string &server_ip, int server_port);

    extern PacketManager pm;
    extern PacketHandler ph;
    extern int udp_fd;
}

#endif //NETWORK_H
