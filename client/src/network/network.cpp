/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "network.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include "packets/packets.h"

using namespace rtype::client;

// Define the global variables in this source file only
namespace rtype::client::network {
    PacketManager pm;
    PacketHandler ph;
    int udp_fd = -1;
}

void network::loop_recv() {
    uint8_t buffer[MAX_PACKET_SIZE];
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    packet_t packet;

    // Non-blocking receive - returns immediately if no data available
    int n = recvfrom(udp_fd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *) &cliaddr, &len);

    if (n > 0) {
        std::cout << "[INFO] Received UDP packet of size " << n << std::endl;
        PacketManager::deserializePacket(buffer, n, packet);
        std::cout << "[INFO] Packet seqid: " << packet.header.seqid << ", type: " << static_cast<int>(packet.header.type) <<
                std::endl;
        pm.handlePacketBytes(buffer, n, cliaddr);
    } else if (n < 0) {
        // Check if it's just no data available (non-blocking behavior)
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "[ERROR] UDP receive error: " << strerror(errno) << std::endl;
        }
        // If errno is EAGAIN or EWOULDBLOCK, it just means no data is available - this is normal
    }
    // n == 0 means the connection was closed gracefully (shouldn't happen with UDP)
}

void network::loop_send() {
    std::vector<std::unique_ptr<packet_t> > packets = pm.fetchPacketsToSend();

    for (auto& packet : packets) {
        std::vector<uint8_t> serialized = PacketManager::serializePacket(*packet);

        // For client, we send packets directly to the connected server
        // Since we used connect() in init_udp_socket, we can use send() instead of sendto()
        int bytes_sent = send(udp_fd, serialized.data(), serialized.size(), 0);

        if (bytes_sent < 0) {
            std::cerr << "[ERROR] Failed to send UDP packet to server: " << strerror(errno) << std::endl;
            std::cerr << "[DEBUG] bytes_sent: " << bytes_sent << ", errno: " << errno << std::endl;
        } else {
            std::cout << "[INFO] Sent UDP packet of size " << serialized.size()
                      << " bytes to server (packet type: " << (int)packet->header.type << ")" << std::endl;
        }
    }
}

int network::init_udp_socket(const std::string &server_ip, int server_port) {
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    if (connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Connection to the server failed");
        close(sockfd);
        return -1;
    }
    udp_fd = sockfd;
    return udp_fd;
}

int network::start_room_connection(const std::string &ip, int port, const std::string &player_name, uint32_t room_code) {
    init_udp_socket(ip, port);

    // Build and send the JoinRoomPacket
    JoinRoomPacket p{};
    p.joinCode = room_code;

    // Secure the player name to avoid overflow
    strncpy(p.name, player_name.c_str(), 31);
    p.name[31] = '\0';

    pm.sendPacketBytesSafe(&p, sizeof(JoinRoomPacket), JOIN_ROOM, nullptr, true);
    return 0;
}

bool network::is_udp_connected() {
    return udp_fd != -1;
}