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

    int setupUDPServer(int port);

    class NetConnection {
    private:
        uint16_t _port;
        std::string _ip;
        uint64_t _lastPacketTimestamp;

        /**
         * @brief Update the timestamp of the last sent packet to make the connection alive
         * This function should be called each time a packet is sent to the client
         * to avoid disconnection due to inactivity
         */
        void _updateLastSendTimestamp();

    public:
        /**
         * @brief Send packet to the peer.
         * @param packet Pointer to the packet to send
         */
        void sendPacketBytes(const packet_t *packet);
    };
}

#endif //NETWORK_H
