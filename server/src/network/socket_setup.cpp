/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "rtype.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "network.h"
#include "packets.h"


int rtype::server::network::setupUDPServer(int port) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    std::cout << "[INFO] UDP server listening on port " << port << " and fd " << sockfd << std::endl;
    return sockfd;
}

void rtype::server::network::loop_recv(int udp_server_fd) {
    uint8_t buffer[MAX_PACKET_SIZE];
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    packet_t packet;
    int n = recvfrom(udp_server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &len);

    if (n > 0)
        std::cout << "[INFO] Received UDP packet of size " << n << std::endl;
    PacketManager::deserializePacket(buffer, n, packet);
    std::cout << "[INFO] Packet seqid: " << packet.header.seqid << ", type: " << static_cast<int>(packet.header.type) <<
            std::endl;
    root.packetManager.handlePacketBytes(buffer, n, cliaddr);
}
