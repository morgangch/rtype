/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "rtype.h"
#include <iostream>
#include <cstring>

// Platform-specific network headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    // Windows doesn't have unistd.h or MSG_DONTWAIT
    #define close closesocket
    #define MSG_DONTWAIT 0
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include "network.h"
#include "packets.h"
#include "../../../common/components/Player.h"
#include "components/PlayerConn.h"
#include "services/PlayerService.h"


namespace rtype::common::components {
    class Player;
}

namespace rtype::server::components {
    class PlayerConn;
}

#ifdef _WIN32
// Initialize Winsock on Windows
static bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "[ERROR] WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

// Cleanup Winsock on Windows
static void cleanupWinsock() {
    WSACleanup();
}
#endif

int rtype::server::network::setupUDPServer(int port) {
#ifdef _WIN32
    // Initialize Winsock on Windows
    static bool winsockInitialized = false;
    if (!winsockInitialized) {
        if (!initializeWinsock()) {
            return -1;
        }
        winsockInitialized = true;
    }
#endif

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "[ERROR] Failed to create socket: ";
#ifdef _WIN32
        std::cerr << WSAGetLastError() << std::endl;
#else
        perror("socket");
#endif
        return -1;
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "[ERROR] Failed to bind socket: ";
#ifdef _WIN32
        std::cerr << WSAGetLastError() << std::endl;
#else
        perror("bind");
#endif
        close(sockfd);
        return -1;
    }
    std::cout << "[INFO] UDP server listening on port " << port << " and fd " << sockfd << std::endl;
    return sockfd;
}

void rtype::server::network::loop_send(int udp_server_fd) {
    std::vector<std::unique_ptr<packet_t> > packets = root.packetManager.fetchPacketsToSend();
    auto *players = root.world.GetAllComponents<rtype::server::components::PlayerConn>();

    for (const auto &pair: *players) {
        auto *p = root.world.GetComponent<components::PlayerConn>(pair.first);
        if (p) {
            std::vector<std::unique_ptr<packet_t> > player_packets = p->packet_manager.fetchPacketsToSend();
            // Force the ip address to each packet
            for (auto &packet: player_packets) {
                std::string addr = p->address;
                int port = p->port;
                packet->header.client_addr[0] = std::stoi(addr.substr(0, addr.find('.')));
                addr = addr.substr(addr.find('.') + 1);
                packet->header.client_addr[1] = std::stoi(addr.substr(0, addr.find('.')));
                addr = addr.substr(addr.find('.') + 1);
                packet->header.client_addr[2] = std::stoi(addr.substr(0, addr.find('.')));
                addr = addr.substr(addr.find('.') + 1);
                packet->header.client_addr[3] = std::stoi(addr);
                packet->header.client_port = port;
            }
            packets.insert(packets.end(),
                           std::make_move_iterator(player_packets.begin()),
                           std::make_move_iterator(player_packets.end()));
        }
    }

    for (auto &packet: packets) {
        std::vector<uint8_t> serialized = PacketManager::serializePacket(*packet);

        // Extract client address and port from packet header
        struct sockaddr_in clientaddr;
        clientaddr.sin_family = AF_INET;

        // Convert client_addr[4] bytes to IP address - simply copy the bytes directly
        memcpy(&clientaddr.sin_addr.s_addr, packet->header.client_addr, 4);

        // Set client port (convert from host to network byte order)
        clientaddr.sin_port = htons(packet->header.client_port);

        // Send the serialized packet to the client
#ifdef _WIN32
        int bytes_sent = sendto(udp_server_fd, (const char*)serialized.data(), serialized.size(), 0,
                                (struct sockaddr *) &clientaddr, sizeof(clientaddr));
#else
        int bytes_sent = sendto(udp_server_fd, serialized.data(), serialized.size(), 0,
                                (struct sockaddr *) &clientaddr, sizeof(clientaddr));
#endif

        if (bytes_sent < 0) {
            std::cerr << "[ERROR] Failed to send UDP packet to client" << std::endl;
#ifdef _WIN32
            std::cerr << "Error code: " << WSAGetLastError() << std::endl;
#else
            perror("sendto");
#endif
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
#ifdef _WIN32
    int n = recvfrom(udp_server_fd, (char*)buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *) &cliaddr, &len);
#else
    int n = recvfrom(udp_server_fd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *) &cliaddr, &len);
#endif

    if (n > 0) {
        std::cout << "[INFO] Received UDP packet of size " << n << std::endl;

        // Redirect to the appropriate player or to the global packet manager
        auto pid = rtype::server::services::player_service::findPlayerByNetwork(cliaddr);
        if (pid) {
            std::cout << "[INFO] Packet associated with player ID " << pid << std::endl;
            auto p = root.world.GetComponent<components::PlayerConn>(pid);
            p->packet_manager.handlePacketBytes(buffer, n, cliaddr);
        } else {
            std::cout << "[INFO] Packet not associated with any player, handling globally" << std::endl;
            root.packetManager.handlePacketBytes(buffer, n, cliaddr);
        }
    } else if (n < 0) {
        // Check if it's just no data available (non-blocking behavior)
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "[ERROR] UDP receive error: " << strerror(errno) << std::endl;
        }
        // Don't spam debug messages for normal EAGAIN/EWOULDBLOCK errors
    }
}
