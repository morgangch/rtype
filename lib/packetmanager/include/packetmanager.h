/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <vector>
#include "packet.h"

#define PACKET_HISTORY_SIZE 512

class PacketManager {
public:
    PacketManager();
    void clean();
    bool resendPacket(uint32_t seqid);
    void handlePacket(packet_t packet);
    void ackMissing();
    std::vector<packet_t> getPacketsToSend();

private:
    uint32_t _send_seqid = 0;
    uint32_t _recv_seqid = 0;
    uint32_t _auth_key = 0;

    std::vector<packet_t> _history_sent;
    std::vector<uint32_t> _missed_packets;

    std::vector<packet_t> _buffer_received;
    std::vector<packet_t> _buffer_send;

    void putToHistory(packet_t packet);
};

#endif //PACKETMANAGER_H
