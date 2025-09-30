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
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "[ERROR] Failed to create socket: ";
        perror("socket");
        return -1;
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "[ERROR] Failed to bind socket: ";
        perror("bind");
        close(sockfd);
        return -1;
    }
    std::cout << "[INFO] UDP server listening on port " << port << " and fd " << sockfd << std::endl;
    return sockfd;
}

void rtype::server::network::loop_send(int udp_server_fd) {
    std::vector<std::unique_ptr<packet_t> > packets = root.packetManager.fetchPacketsToSend();

    for (auto& packet : packets) {
        std::vector<uint8_t> serialized = PacketManager::serializePacket(*packet);


        // Extract client address and port from packet header
        struct sockaddr_in clientaddr;
        clientaddr.sin_family = AF_INET;

        // Convert client_addr[4] bytes to IP address
        clientaddr.sin_addr.s_addr = (packet->header.client_addr[0] << 0) |
                                     (packet->header.client_addr[1] << 8) |
                                     (packet->header.client_addr[2] << 16) |
                                     (packet->header.client_addr[3] << 24);

        // Set client port (convert from host to network byte order)
        clientaddr.sin_port = htons(packet->header.client_port);

        // Send the serialized packet to the client
        int bytes_sent = sendto(udp_server_fd, serialized.data(), serialized.size(), 0,
                                (struct sockaddr *) &clientaddr, sizeof(clientaddr));

        if (bytes_sent < 0) {
            std::cerr << "[ERROR] Failed to send UDP packet to client" << std::endl;
        } else {
            std::cout << "[INFO] Sent UDP packet of size " << serialized.size()
                    << " to " << (int) packet->header.client_addr[0] << "."
                    << (int) packet->header.client_addr[1] << "."
                    << (int) packet->header.client_addr[2] << "."
                    << (int) packet->header.client_addr[3] << ":"
                    << packet->header.client_port << std::endl;
        }
    }
}

void rtype::server::network::loop_recv(int udp_server_fd) {
    uint8_t buffer[MAX_PACKET_SIZE];
    struct sockaddr_in cliaddr{};
    socklen_t len = sizeof(cliaddr);
    packet_t packet;
    int n = recvfrom(udp_server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &len);

    if (n > 0) {
        std::cout << "[INFO] Received UDP packet of size " << n << std::endl;
        PacketManager::deserializePacket(buffer, n, packet);
        std::cout << "[INFO] Packet seqid: " << packet.header.seqid << ", type: " << static_cast<int>(packet.header.type) <<
                std::endl;
        root.packetManager.handlePacketBytes(buffer, n, cliaddr);
    }
}
