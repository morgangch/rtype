/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packetmanager.h"

PacketManager::PacketManager() : _send_seqid(0), _recv_seqid(0) {
}

void PacketManager::clean() {
    _history_sent.clear();
    _missed_packets.clear();
    _buffer_received.clear();
    _buffer_send.clear();
    _send_seqid = 0;
    _recv_seqid = 0;
}

void PacketManager::ackMissing() {
    packet_header_t header;
    packet_t packet;
    header.seqid = 0;
    header.type = 0;
    header.auth = _auth_key;

    for (auto seqid: _missed_packets) {
        header.ack = seqid;
        packet.header = header;
        packet.data = nullptr;
        _buffer_send.push_back(packet);
    }
}


bool PacketManager::resendPacket(uint32_t seqid) {
    for (auto &packet: _history_sent) {
        if (packet.header.seqid == seqid) {
            _buffer_send.push_back(packet);
            return true;
        }
    }
    return false;
}

void PacketManager::handlePacket(packet_t packet) {
    // If this is a missed packet, remove it from the missed list
    _missed_packets.erase(std::remove(_missed_packets.begin(), _missed_packets.end(), packet.header.seqid),
                          _missed_packets.end());
    // Declare as missed all previous packets not received
    for (uint32_t i = _recv_seqid + 1; i < packet.header.seqid; i++) {
        _missed_packets.push_back(i);
    }
    _recv_seqid = packet.header.seqid;
    // Acknowledge all missed packets
    ackMissing();
    _buffer_received.push_back(packet);
}
