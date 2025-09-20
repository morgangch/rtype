/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packetmanager.h"

PacketManager::PacketManager() : _send_seqid(0), _recv_seqid(0) {
}

packet_t deserializePacket(const uint8_t *data, size_t size, packet_t &packet) {
    if (size < sizeof(packet_header_t)) {
        throw std::runtime_error("Data size is smaller than packet header size");
    }
    std::memcpy(&packet.header, data, sizeof(packet_header_t));
    if (size > sizeof(packet_header_t)) {
        packet.data = new uint8_t[size - sizeof(packet_header_t)];
        std::memcpy(packet.data, data + sizeof(packet_header_t), size - sizeof(packet_header_t));
    } else {
        packet.data = nullptr;
    }
    return packet;
}

std::vector<uint8_t> serializePacket(const packet_t &packet) {
    size_t data_size = packet.data ? sizeof(packet.data) : 0;
    std::vector<uint8_t> buffer(sizeof(packet_header_t) + data_size);
    std::memcpy(buffer.data(), &packet.header, sizeof(packet_header_t));
    if (data_size > 0) {
        std::memcpy(buffer.data() + sizeof(packet_header_t), packet.data, data_size);
    }
    return buffer;
}


void PacketManager::handlePacketBytes(const uint8_t *data, size_t size) {
    try {
        // Deserialize the packet and store it in unique_ptr<packet_t>
        std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();
        deserializePacket(data, size, *packet);
        _handlePacket(std::move(packet));
    } catch (const std::exception &e) {
        // Invalid packet, ignore it
        return;
    }
}

void PacketManager::sendPacketBytes(void **data, size_t *size, uint8_t packet_type) {
    packet_header_t header;
    std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();

    header.seqid = ++_send_seqid;
    header.ack = 0;
    header.type = packet_type;
    header.auth = _auth_key;

    packet->header = header;
    packet->data = *data;

    // Serialize the packet
    std::vector<uint8_t> serialized_packet = serializePacket(*packet);

    // Prepare the output data
    *size = serialized_packet.size();
    *data = new uint8_t[*size];
    std::memcpy(*data, serialized_packet.data(), *size);

    // Store the packet in the send buffer
    _buffer_send.push_back(std::move(packet));
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
        _buffer_send.push_back(std::make_unique<packet_t>(packet));
    }
}


bool PacketManager::_resendPacket(uint32_t seqid) {
    for (std::unique_ptr<packet_t> &packet: _buffer_send) {
        if (packet->header.seqid == seqid) {
            _buffer_send.push_back(std::make_unique<packet_t>(*packet));
            return true;
        }
    }
    return false;
}


void PacketManager::_handlePacket(std::unique_ptr<packet_t> packet) {
    // If this is a missed packet, remove it from the missed list
    _missed_packets.erase(std::remove(_missed_packets.begin(), _missed_packets.end(), packet->header.seqid),
                          _missed_packets.end());
    // Declare as missed all previous packets not received
    for (uint32_t i = _recv_seqid + 1; i < packet->header.seqid; i++) {
        _missed_packets.push_back(i);
    }
    _recv_seqid = packet->header.seqid;
    // Acknowledge all missed packets
    ackMissing();

    // Check if this is a ack packet, then resend the asked packet
    if (packet->header.ack != 0) {
        _resendPacket(packet->header.ack);
        return;
    }
    // Otherwise, store the packet in the received buffer
    _buffer_received.push_back(std::move(packet));

    // Sort the received buffer by seqid
    std::sort(_buffer_received.begin(), _buffer_received.end(), [](const std::unique_ptr<packet_t> &a, const std::unique_ptr<packet_t> &b) {
        return a->header.seqid < b->header.seqid;
    });
}

std::vector<std::unique_ptr<packet_t> > PacketManager::fetchReceivedPackets() {
    std::vector<std::unique_ptr<packet_t> > tmp = std::move(_buffer_received);
    _buffer_received.clear();

    // Fill the packets history
    for (auto &packet: tmp) {
        if (_history_sent.size() >= PACKET_HISTORY_SIZE) {
            _history_sent.erase(_history_sent.begin());
        }
        // Create a copy of the packet to store in history
        std::unique_ptr<packet_t> packet_copy = std::make_unique<packet_t>();
        packet_copy->header = packet->header;
        if (packet->data) {
            size_t data_size = sizeof(packet->data);
            packet_copy->data = new uint8_t[data_size];
            std::memcpy(packet_copy->data, packet->data, data_size);
        } else {
            packet_copy->data = nullptr;
        }
        _history_sent.push_back(*packet_copy);
    }

    return tmp;
}
