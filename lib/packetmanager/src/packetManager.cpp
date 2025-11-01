/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Thread-Safe PacketManager implementation
*/

#include "packetmanager.h"
#include <cstring>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <zlib.h>

// Platform-specific network headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

PacketManager::PacketManager() : _send_seqid(0), _recv_seqid(0) {
}

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
        const uint8_t* payload_data = data + sizeof(packet_header_t);

        // Check if data is compressed (original_size != 0)
        if (packet.header.original_size > 0) {
            // Data is compressed - decompress it
            try {
                auto decompressed = decompress_data(payload_data, packet.header.data_size, packet.header.original_size);

                // Allocate buffer and copy decompressed data
                packet.data = new uint8_t[decompressed.size()];
                std::memcpy(packet.data, decompressed.data(), decompressed.size());

                // Update data_size to reflect decompressed size for application use
                packet.header.data_size = decompressed.size();
                packet.header.original_size = 0; // Clear to indicate data is now uncompressed
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to decompress packet data: " + std::string(e.what()));
            }
        } else {
            // Data is not compressed - use as-is
            packet.data = new uint8_t[packet.header.data_size];
            std::memcpy(packet.data, payload_data, packet.header.data_size);
        }
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

void PacketManager::handlePacketBytes(const uint8_t *data, size_t size, sockaddr_in client_addr) {
    try {
        // Deserialize the packet and store it in unique_ptr<packet_t>
        std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();
        deserializePacket(data, size, *packet);
        packet->header.client_addr[0] = (client_addr.sin_addr.s_addr >> 0) & 0xFF;
        packet->header.client_addr[1] = (client_addr.sin_addr.s_addr >> 8) & 0xFF;
        packet->header.client_addr[2] = (client_addr.sin_addr.s_addr >> 16) & 0xFF;
        packet->header.client_addr[3] = (client_addr.sin_addr.s_addr >> 24) & 0xFF;
        packet->header.client_port = ntohs(client_addr.sin_port);

        // Lock before calling _handlePacket
        std::lock_guard<std::mutex> lock(_mutex);
        _handlePacket(std::move(packet));
    } catch (const std::exception &e) {
        // Invalid packet, ignore it
        return;
    }
}

std::unique_ptr<uint8_t[]> PacketManager::sendPacketBytesSafe(const void *data, size_t data_size, uint8_t packet_type,
                                                              size_t *output_size, bool important) {
    std::lock_guard<std::mutex> lock(_mutex);

    packet_header_t header;
    std::unique_ptr<packet_t> packet = std::make_unique<packet_t>();

    header.seqid = important ? ++_send_seqid : 0;
    header.ack = 0;
    header.type = packet_type;
    header.auth = _auth_key;
    header.original_size = 0; // Will be set if compression is used
    std::memset(&header.client_addr, 0, sizeof(header.client_addr));
    header.client_port = 0;

    void* packet_data = nullptr;
    size_t packet_data_size = data_size;

    // Compress data if compression is enabled and data size is significant (> 32 bytes)
    if (_compression_enabled && data_size > 32) {
        try {
            auto compressed = compress_data(data, data_size);

            // Only use compression if it actually reduces size
            if (compressed.size() < data_size) {
                header.original_size = data_size;
                packet_data_size = compressed.size();
                packet_data = new uint8_t[compressed.size()];
                std::memcpy(packet_data, compressed.data(), compressed.size());
            } else {
                // Compression didn't help, use original data
                header.original_size = 0;
                packet_data_size = data_size;
                if (data_size > 0) {
                    packet_data = new uint8_t[data_size];
                    std::memcpy(packet_data, data, data_size);
                }
            }
        } catch (const std::exception& e) {
            // Compression failed, use original data
            header.original_size = 0;
            packet_data_size = data_size;
            if (data_size > 0) {
                packet_data = new uint8_t[data_size];
                std::memcpy(packet_data, data, data_size);
            }
        }
    } else {
        // Compression disabled or data too small - use original data
        if (data_size > 0) {
            packet_data = new uint8_t[data_size];
            std::memcpy(packet_data, data, data_size);
        }
    }

    header.data_size = packet_data_size;
    packet->header = header;
    packet->data = static_cast<uint8_t*>(packet_data);

    // Serialize the packet
    std::vector<uint8_t> serialized_packet = serializePacket(*packet);

    // Create smart pointer for output data
    auto output_data = std::make_unique<uint8_t[]>(serialized_packet.size());
    std::memcpy(output_data.get(), serialized_packet.data(), serialized_packet.size());

    if (output_size != nullptr) {
        *output_size = serialized_packet.size();
    }
    // Store the packet in the send buffer
    _buffer_send.push_back(std::move(packet));

    return output_data;
}

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
        const uint8_t* payload_data = data + sizeof(packet_header_t);

        // Check if data is compressed (original_size != 0)
        if (packet->header.original_size > 0) {
            // Data is compressed - decompress it
            try {
                auto decompressed = decompress_data(payload_data, packet->header.data_size, packet->header.original_size);

                // Allocate buffer and copy decompressed data
                packet->data = new uint8_t[decompressed.size()];
                std::memcpy(packet->data, decompressed.data(), decompressed.size());

                // Update data_size to reflect decompressed size for application use
                packet->header.data_size = decompressed.size();
                packet->header.original_size = 0; // Clear to indicate data is now uncompressed
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to decompress packet data: " + std::string(e.what()));
            }
        } else {
            // Data is not compressed - use as-is
            packet->data = new uint8_t[packet->header.data_size];
            std::memcpy(packet->data, payload_data, packet->header.data_size);
        }
    } else {
        packet->data = nullptr;
    }
    return packet;
}

void PacketManager::clean() {
    std::lock_guard<std::mutex> lock(_mutex);

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
    std::lock_guard<std::mutex> lock(_mutex);

    packet_header_t header;
    packet_t packet;
    header.seqid = 0;
    header.type = 0;
    header.auth = _auth_key;
    header.ack = 0;
    header.data_size = 0;

    for (auto seqid: _missed_packets) {
        header.ack = seqid;
        packet.header = header;
        packet.data = nullptr;
        _buffer_send.push_back(std::make_unique<packet_t>(packet));
    }
    _missed_packets.clear();
}

bool PacketManager::_resendPacket(uint32_t seqid) {
    // Note: This method assumes the mutex is already locked by the caller
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
    // Note: This method assumes the mutex is already locked by the caller

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
        packet_header_t header;
        packet_t ack_packet;
        header.seqid = 0;
        header.type = 0;
        header.auth = _auth_key;
        header.ack = 0;
        header.data_size = 0;

        for (auto seqid: _missed_packets) {
            header.ack = seqid;
            ack_packet.header = header;
            ack_packet.data = nullptr;
            _buffer_send.push_back(std::make_unique<packet_t>(ack_packet));
        }
        _missed_packets.clear();
    }

    // Always store the packet in the received buffer
    _buffer_received.push_back(std::move(packet));

    // Sort the received buffer by seqid
    std::sort(_buffer_received.begin(), _buffer_received.end(),
              [](const std::unique_ptr<packet_t> &a, const std::unique_ptr<packet_t> &b) {
                  return a->header.seqid < b->header.seqid;
              });
}

std::vector<std::unique_ptr<packet_t> > PacketManager::fetchReceivedPackets() {
    std::lock_guard<std::mutex> lock(_mutex);

    std::vector<std::unique_ptr<packet_t> > tmp = std::move(_buffer_received);
    _buffer_received.clear();
    return tmp;
}

std::vector<std::unique_ptr<packet_t> > PacketManager::fetchPacketsToSend() {
    std::lock_guard<std::mutex> lock(_mutex);

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

        // Copy the data using the data_size from header
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

// Thread-safe getter implementations
uint32_t PacketManager::_get_send_seqid() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _send_seqid;
}

uint32_t PacketManager::_get_recv_seqid() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _recv_seqid;
}

uint32_t PacketManager::_get_auth_key() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _auth_key;
}

std::vector<packet_t> PacketManager::_get_history_sent() const {
    std::lock_guard<std::mutex> lock(_mutex);

    // Create a deep copy of the history
    std::vector<packet_t> copy;
    for (const auto &packet : _history_sent) {
        packet_t packet_copy;
        packet_copy.header = packet.header;

        if (packet.header.data_size > 0 && packet.data) {
            packet_copy.data = new uint8_t[packet.header.data_size];
            std::memcpy(packet_copy.data, packet.data, packet.header.data_size);
        } else {
            packet_copy.data = nullptr;
        }
        copy.push_back(packet_copy);
    }
    return copy;
}

std::vector<uint32_t> PacketManager::_get_missed_packets() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _missed_packets;
}

size_t PacketManager::_get_buffer_send_size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _buffer_send.size();
}

size_t PacketManager::_get_buffer_received_size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _buffer_received.size();
}

void PacketManager::setCompressionEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(_mutex);
    _compression_enabled = enable;
}

bool PacketManager::isCompressionEnabled() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _compression_enabled;
}

std::vector<unsigned char> PacketManager::compress_data(const void* data, size_t size) {
    if (!data || size == 0) {
        throw std::invalid_argument("Données invalides");
    }

    // Calcul de la taille maximale après compression
    uLongf compressed_size = compressBound(size);
    std::vector<unsigned char> compressed(compressed_size);

    // Compression
    int result = compress(compressed.data(), &compressed_size,
                         static_cast<const Bytef*>(data), size);

    if (result != Z_OK) {
        throw std::runtime_error("Erreur de compression");
    }

    // Redimensionne au taille réelle
    compressed.resize(compressed_size);
    return compressed;
}

// Décompresse les données
// Retourne un vecteur contenant les données décompressées
std::vector<unsigned char> PacketManager::decompress_data(const void* data, size_t compressed_size,
                                           size_t original_size) {
    if (!data || compressed_size == 0) {
        throw std::invalid_argument("Données invalides");
    }

    std::vector<unsigned char> decompressed(original_size);
    uLongf decompressed_size = original_size;

    // Décompression
    int result = uncompress(decompressed.data(), &decompressed_size,
                           static_cast<const Bytef*>(data), compressed_size);

    if (result != Z_OK) {
        throw std::runtime_error("Erreur de décompression");
    }

    decompressed.resize(decompressed_size);
    return decompressed;
}