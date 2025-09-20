/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "packetmanager.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <algorithm>

PacketManager::PacketManager() : _send_seqid(0), _recv_seqid(0) {
}

// Add destructor to clean up memory
PacketManager::~PacketManager() {
    clean();
}

packet_t PacketManager::deserializePacket(const uint8_t *data, size_t size, packet_t &packet) {
    if (size < sizeof(packet_header_t)) {
        throw std::runtime_error("Data size is smaller than packet header size");
    }
    std::memcpy(&packet.header, data, sizeof(packet_header_t));

    // Validate that the data_size field matches the actual data received
    size_t expected_total_size = sizeof(packet_header_t) + packet.header.data_size;
    if (size != expected_total_size) {
        throw std::runtime_error("Packet size mismatch: expected " + std::to_string(expected_total_size) +
                                 ", got " + std::to_string(size));
    }

    if (packet.header.data_size > 0) {
        packet.data = new uint8_t[packet.header.data_size];
        std::memcpy(packet.data, data + sizeof(packet_header_t), packet.header.data_size);
    } else {
        packet.data = nullptr;
    }
    return packet;
}

std::vector<uint8_t> PacketManager::serializePacket(const packet_t &packet) {
    std::vector<uint8_t> buffer(sizeof(packet_header_t) + packet.header.data_size);
    std::memcpy(buffer.data(), &packet.header, sizeof(packet_header_t));
    if (packet.header.data_size > 0 && packet.data) {
        std::memcpy(buffer.data() + sizeof(packet_header_t), packet.data, packet.header.data_size);
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
//
// void PacketManager::sendPacketBytes(void **data, size_t *size, uint8_t packet_type, bool important) {
//     packet_header_t header;
//     std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();
//
//     // Store the original data size before modifying the data pointer
//     size_t original_data_size = *size;
//
//     header.seqid = important ? ++_send_seqid : 0;
//     header.ack = 0;
//     header.type = packet_type;
//     header.auth = _auth_key;
//     header.data_size = original_data_size; // Set the data size in header
//
//     packet->header = header;
//     packet->data = *data;
//
//     // Serialize the packet using the data_size from header
//     std::vector<uint8_t> serialized_packet = serializePacket(*packet);
//
//     // Prepare the output data
//     *size = serialized_packet.size();
//     *data = new uint8_t[*size];
//     std::memcpy(*data, serialized_packet.data(), *size);
//
//     // Store the packet in the send buffer
//     _buffer_send.push_back(std::move(packet));
// }

// Add safer version of sendPacketBytes that returns smart pointer
std::unique_ptr<uint8_t[]> PacketManager::sendPacketBytesSafe(const void *data, size_t data_size, uint8_t packet_type,
                                                              size_t *output_size, bool important) {
    packet_header_t header;
    std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();

    header.seqid = important ? ++_send_seqid : 0;
    header.ack = 0;
    header.type = packet_type;
    header.auth = _auth_key;
    header.data_size = data_size;

    packet->header = header;
    // Make a copy of the input data for the packet
    if (data_size > 0) {
        packet->data = new uint8_t[data_size];
        std::memcpy(packet->data, data, data_size);
    } else {
        packet->data = nullptr;
    }

    // Serialize the packet
    std::vector<uint8_t> serialized_packet = serializePacket(*packet);

    // Create smart pointer for output data
    auto output_data = std::make_unique<uint8_t[]>(serialized_packet.size());
    std::memcpy(output_data.get(), serialized_packet.data(), serialized_packet.size());
    *output_size = serialized_packet.size();

    // Store the packet in the send buffer
    _buffer_send.push_back(std::move(packet));

    return output_data;
}

// Add safer deserialize function using smart pointers
std::unique_ptr<packet_t> PacketManager::deserializePacketSafe(const uint8_t *data, size_t size) {
    auto packet = std::make_unique<packet_t>();

    if (size < sizeof(packet_header_t)) {
        throw std::runtime_error("Data size is smaller than packet header size");
    }
    std::memcpy(&packet->header, data, sizeof(packet_header_t));

    // Validate that the data_size field matches the actual data received
    size_t expected_total_size = sizeof(packet_header_t) + packet->header.data_size;
    if (size != expected_total_size) {
        throw std::runtime_error("Packet size mismatch: expected " + std::to_string(expected_total_size) +
                                 ", got " + std::to_string(size));
    }

    if (packet->header.data_size > 0) {
        packet->data = new uint8_t[packet->header.data_size];
        std::memcpy(packet->data, data + sizeof(packet_header_t), packet->header.data_size);
    } else {
        packet->data = nullptr;
    }
    return packet;
}

void PacketManager::clean() {
    // Clean up history data before clearing
    for (auto &packet: _history_sent) {
        if (packet.data) {
            delete[] static_cast<uint8_t *>(packet.data);
            packet.data = nullptr;
        }
    }

    // Clean up received buffer data
    for (auto &packet: _buffer_received) {
        if (packet && packet->data) {
            delete[] static_cast<uint8_t *>(packet->data);
            packet->data = nullptr;
        }
    }

    // Clean up send buffer data
    for (auto &packet: _buffer_send) {
        if (packet && packet->data) {
            delete[] static_cast<uint8_t *>(packet->data);
            packet->data = nullptr;
        }
    }

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
    header.ack = 0;
    header.data_size = 0; // ACK packets have no data payload

    for (auto seqid: _missed_packets) {
        header.ack = seqid;
        packet.header = header;
        packet.data = nullptr;
        _buffer_send.push_back(std::make_unique<packet_t>(packet));
    }
    _missed_packets.clear();
}


bool PacketManager::_resendPacket(uint32_t seqid) {
    for (const packet_t &packet: _history_sent) {
        if (packet.header.seqid == seqid) {
            // Create a proper deep copy of the packet for retransmission
            std::unique_ptr<packet_t> retrans_packet = std::make_unique<packet_t>();
            retrans_packet->header = packet.header;

            // Deep copy the data if it exists
            if (packet.header.data_size > 0 && packet.data) {
                retrans_packet->data = new uint8_t[packet.header.data_size];
                std::memcpy(retrans_packet->data, packet.data, packet.header.data_size);
            } else {
                retrans_packet->data = nullptr;
            }

            _buffer_send.push_back(std::move(retrans_packet));
            return true;
        }
    }
    return false;
}


void PacketManager::_handlePacket(std::unique_ptr<packet_t> packet) {
    // Check if this is an ACK packet, handle it separately
    if (packet->header.ack != 0) {
        _resendPacket(packet->header.ack);
        return;
    }

    // If this is a missed packet, remove it from the missed list
    _missed_packets.erase(std::remove(_missed_packets.begin(), _missed_packets.end(), packet->header.seqid),
                          _missed_packets.end());

    // Update highest received sequence ID and detect missing packets
    if (packet->header.seqid != 0 && packet->header.seqid > _recv_seqid) {
        // Declare as missed all packets between last received and this one
        for (uint32_t i = _recv_seqid + 1; i < packet->header.seqid; i++) {
            _missed_packets.push_back(i);
        }
        _recv_seqid = packet->header.seqid;

        // Acknowledge all missed packets
        ackMissing();
    }

    // Always store the packet in the received buffer (allows duplicates and out-of-order)
    _buffer_received.push_back(std::move(packet));

    // Sort the received buffer by seqid
    std::sort(_buffer_received.begin(), _buffer_received.end(),
              [](const std::unique_ptr<packet_t> &a, const std::unique_ptr<packet_t> &b) {
                  return a->header.seqid < b->header.seqid;
              });
}

std::vector<std::unique_ptr<packet_t> > PacketManager::fetchReceivedPackets() {
    std::vector<std::unique_ptr<packet_t> > tmp = std::move(_buffer_received);
    _buffer_received.clear();
    return tmp;
}

std::vector<std::unique_ptr<packet_t> > PacketManager::fetchPacketsToSend() {
    std::vector<std::unique_ptr<packet_t> > tmp = std::move(_buffer_send);
    _buffer_send.clear();

    // Fill the packets history
    for (auto &packet: tmp) {
        if (_history_sent.size() >= PACKET_HISTORY_SIZE) {
            // Properly clean up the oldest packet before removing it
            if (_history_sent.front().data) {
                delete[] static_cast<uint8_t *>(_history_sent.front().data);
            }
            _history_sent.erase(_history_sent.begin());
        }
        // Skip if not important
        if (packet->header.seqid == 0)
            continue;
        // Create a copy of the packet to store in history
        packet_t packet_copy;
        packet_copy.header = packet->header;

        // Now we can properly copy the data using the data_size from header
        if (packet->header.data_size > 0 && packet->data) {
            packet_copy.data = new uint8_t[packet->header.data_size];
            std::memcpy(packet_copy.data, packet->data, packet->header.data_size);
        } else {
            packet_copy.data = nullptr;
        }
        _history_sent.push_back(packet_copy);
    }
    return tmp;
}
