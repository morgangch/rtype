/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "network/network.h"
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

#include "network/senders.h"
#include "packets/packets.h"
#include "utils/bytes_printer.h"

using namespace rtype::client;

namespace rtype::client::network {
    int udp_fd = -1;
    PacketHandler ph;
    PacketManager pm;
}

// Global player info (shared with game_controller.cpp and lobby)
std::string g_username = "Player";
uint32_t g_playerServerId = 0;

void network::loop_recv() {
    uint8_t buffer[MAX_PACKET_SIZE];
    packet_t packet;

    // Use recv() instead of recvfrom() since we used connect() on the UDP socket
#ifdef _WIN32
    int n = recv(rtype::client::network::udp_fd, (char*)buffer, sizeof(buffer), MSG_DONTWAIT);
#else
    int n = recv(rtype::client::network::udp_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
#endif

    if (n > 0) {
        PacketManager::deserializePacket(buffer, n, packet);

        // For connected UDP socket, we need to create a dummy sockaddr_in for the packet manager
        struct sockaddr_in servaddr{};
        pm.handlePacketBytes(buffer, n, servaddr);
    } else if (n < 0) {
        // Check if it's just no data available (non-blocking behavior)
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "[ERROR] UDP receive error: " << err << std::endl;
        }
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "[ERROR] UDP receive error: " << strerror(errno) << std::endl;
        }
#endif
        // Don't spam debug messages for normal EAGAIN/EWOULDBLOCK errors
    }
    // n == 0 means the connection was closed gracefully (shouldn't happen with UDP)
}

void network::loop_send() {
    std::vector<std::unique_ptr<packet_t> > packets = pm.fetchPacketsToSend();

    for (auto& packet : packets) {
        std::vector<uint8_t> serialized = PacketManager::serializePacket(*packet);
        int bytes_sent = send(rtype::client::network::udp_fd, (const char*)serialized.data(), serialized.size(), 0);

        if (bytes_sent < 0) {
#ifdef _WIN32
            std::cerr << "[ERROR] Failed to send UDP packet to server: " << WSAGetLastError() << std::endl;
            std::cerr << "[DEBUG] bytes_sent: " << bytes_sent << std::endl;
#else
            std::cerr << "[ERROR] Failed to send UDP packet to server: " << strerror(errno) << std::endl;
            std::cerr << "[DEBUG] bytes_sent: " << bytes_sent << ", errno: " << errno << std::endl;
#endif
        }
    }
}

int network::init_udp_socket(const std::string &server_ip, int server_port) {
#ifdef _WIN32
    // Initialize Winsock on Windows
    static bool winsockInitialized = false;
    if (!winsockInitialized) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "[ERROR] WSAStartup failed: " << result << std::endl;
            return -1;
        }
        winsockInitialized = true;
    }
#endif

    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef _WIN32
        std::cerr << "[ERROR] socket creation failed: " << WSAGetLastError() << std::endl;
#else
        perror("socket creation failed");
#endif
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    if (connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
#ifdef _WIN32
        std::cerr << "[ERROR] Connection to the server failed: " << WSAGetLastError() << std::endl;
#else
        perror("Connection to the server failed");
#endif
        close(sockfd);
        return -1;
    }

#ifdef _WIN32
    // Set socket to non-blocking mode on Windows
    u_long mode = 1; // 1 to enable non-blocking
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        std::cerr << "[ERROR] Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        close(sockfd);
        return -1;
    }
#endif

    rtype::client::network::udp_fd = sockfd;
    return rtype::client::network::udp_fd;
}

int network::start_room_connection(const std::string &ip, int port, const std::string &player_name, uint32_t room_code, uint8_t vessel_type) {
    init_udp_socket(ip, port);
    
    // Store username globally so JOIN_ROOM_ACCEPTED handler can use it
    g_username = player_name;
    g_playerServerId = 0; // Reset on new connection
    
    senders::send_join_room_request(player_name, room_code, vessel_type);
    return 0;
}

bool network::is_udp_connected() {
    return rtype::client::network::udp_fd != -1;
}