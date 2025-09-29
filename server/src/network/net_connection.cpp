/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "network.h"
#include "rtype.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

using namespace rtype::server::network;

void NetConnection::_updateLastSendTimestamp() {
    _lastPacketTimestamp = static_cast<uint64_t>(time(nullptr));
}

void NetConnection::sendPacketBytes(const packet_t *packet) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    std::vector<uint8_t> serializedPacket = PacketManager::serializePacket(*packet);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(_port);
    servaddr.sin_addr.s_addr = inet_addr(_ip.c_str());

    sendto(sockfd, serializedPacket.data(), serializedPacket.size(), 0,
           (const struct sockaddr *) &servaddr, sizeof(servaddr));
    return 0;
}
