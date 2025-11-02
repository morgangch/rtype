/**
 * @file packetmanager.h
 * @brief Network packet management system with reliability and sequencing (Thread-Safe)
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the PacketManager class which handles reliable network
 * communication with automatic retransmission, sequence tracking, and packet
 * buffering for the R-Type game's networking layer.
 * Thread-safe version with mutex protection.
 */

#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <vector>
#include <memory>
#include <mutex>
#include "packet.h"
#include <zlib.h>

/**
 * @brief Maximum number of packets to keep in transmission history
 *
 * Used for retransmission tracking and acknowledgment handling.
 * Limits memory usage while providing reliable delivery.
 */
#define PACKET_HISTORY_SIZE 512

struct sockaddr_in;

/**
 * @brief Network packet management system with reliability features (Thread-Safe)
 *
 * PacketManager provides reliable packet transmission over UDP by implementing:
 * - Automatic sequence numbering for packet ordering
 * - Retransmission of lost packets
 * - Acknowledgment tracking and handling
 * - Packet buffering for send and receive operations
 * - Serialization and deserialization of packet data
 * - Thread-safe operations with mutex protection
 *
 * The manager maintains separate buffers for incoming and outgoing packets,
 * tracks missed packets for retransmission, and provides both safe and
 * unsafe methods for packet operations.
 */
class PacketManager {
public:
    /**
     * @brief Constructor - initializes packet manager with default values
     *
     * Sets up empty buffers, initializes sequence counters to 0,
     * and prepares the manager for packet operations.
     */
    PacketManager();

    /**
     * @brief Destructor - cleans up resources and buffers
     */
    ~PacketManager();

    /**
     * @brief Deleted copy constructor to prevent copying
     *
     * PacketManager contains unique_ptr members and should not be copied.
     */
    PacketManager(const PacketManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying
     */
    PacketManager& operator=(const PacketManager&) = delete;

    /**
     * @brief Deleted move constructor (mutex is not moveable)
     */
    PacketManager(PacketManager&&) = delete;

    /**
     * @brief Deleted move assignment operator (mutex is not moveable)
     */
    PacketManager& operator=(PacketManager&&) = delete;

    /**
     * @brief Deserializes raw packet bytes into a packet structure
     *
     * Converts raw network bytes into a structured packet_t object.
     * This is the unsafe version that requires pre-allocated packet storage.
     * Thread-safe: This is a static method working on local data.
     *
     * @param data Pointer to raw packet bytes including header
     * @param size Total size of the raw packet data
     * @param packet Reference to packet_t structure to fill
     * @return packet_t The deserialized packet structure
     */
    static packet_t deserializePacket(const uint8_t *data, size_t size, packet_t &packet);

    /**
     * @brief Safe version of packet deserialization with automatic memory management
     *
     * Creates a new packet_t instance and deserializes raw bytes into it.
     * Returns a smart pointer for automatic memory management.
     * Thread-safe: This is a static method working on local data.
     *
     * @param data Pointer to raw packet bytes including header
     * @param size Total size of the raw packet data
     * @return std::unique_ptr<packet_t> Smart pointer to deserialized packet, or nullptr if invalid
     */
    static std::unique_ptr<packet_t> deserializePacketSafe(const uint8_t *data, size_t size);

    /**
     * @brief Serializes a packet structure into raw bytes for transmission
     *
     * Converts a structured packet_t object into raw bytes suitable
     * for network transmission.
     * Thread-safe: This is a static method working on local data.
     *
     * @param packet The packet structure to serialize
     * @return std::vector<uint8_t> Vector containing serialized packet bytes
     */
    static std::vector<uint8_t> serializePacket(const packet_t &packet);

    /**
     * @brief Cleans up internal buffers and resets state (Thread-Safe)
     *
     * Clears all pending packets, resets sequence numbers,
     * and prepares the manager for fresh operations.
     */
    void clean();

    /**
     * @brief Handles incoming raw packet bytes from network socket (Thread-Safe)
     *
     * Processes raw packet data received from the network, including
     * header parsing, validation, and buffering for later retrieval.
     * Invalid packets are silently discarded.
     *
     * @param data Pointer to raw packet bytes including header
     * @param size Size of the raw packet data
     * @param client_addr Socket address of the packet sender
     */
    void handlePacketBytes(const uint8_t *data, size_t size, sockaddr_in client_addr);

    /**
     * @brief Safe packet transmission with automatic memory management (Thread-Safe)
     *
     * Creates a packet from the provided data, assigns sequence numbers
     * for important packets, and prepares it for transmission. Returns
     * a smart pointer to avoid memory leaks.
     *
     * @param data Pointer to payload data to be sent
     * @param data_size Size of the payload data
     * @param packet_type Type identifier for the packet (0-255)
     * @param output_size Pointer to store the size of serialized data
     * @param important If true, packet gets sequence ID and reliability tracking
     * @return std::unique_ptr<uint8_t[]> Smart pointer to serialized packet data
     */
    std::unique_ptr<uint8_t[]> sendPacketBytesSafe(const void *data, size_t data_size, uint8_t packet_type, size_t *output_size, bool important = true);

    /**
     * @brief Handles acknowledgment of missing packets (Thread-Safe)
     *
     * Processes missed packet notifications and initiates retransmission
     * of important packets that were not acknowledged by the receiver.
     */
    void ackMissing();

    /**
     * @brief Retrieves all received packets from the buffer (Thread-Safe)
     *
     * Returns and clears the buffer of packets that have been received
     * and processed, ready for application-level handling.
     *
     * @return std::vector<std::unique_ptr<packet_t>> Vector of received packets
     */
    std::vector<std::unique_ptr<packet_t> > fetchReceivedPackets();

    /**
     * @brief Retrieves all packets queued for transmission (Thread-Safe)
     *
     * Returns and clears the buffer of packets that are ready to be
     * sent over the network.
     *
     * @return std::vector<std::unique_ptr<packet_t>> Vector of packets to send
     */
    std::vector<std::unique_ptr<packet_t> > fetchPacketsToSend();

    /**
     * @brief Gets the current send sequence ID (Thread-Safe)
     * @return uint32_t Current sequence ID for outgoing packets
     */
    [[nodiscard]] uint32_t _get_send_seqid() const;

    /**
     * @brief Gets the current receive sequence ID (Thread-Safe)
     * @return uint32_t Current sequence ID for incoming packets
     */
    [[nodiscard]] uint32_t _get_recv_seqid() const;

    /**
     * @brief Gets the current authentication key (Thread-Safe)
     * @return uint32_t Current authentication key for packet validation
     */
    [[nodiscard]] uint32_t _get_auth_key() const;

    /**
     * @brief Gets a copy of the transmission history buffer (Thread-Safe)
     * @return std::vector<packet_t> Copy of sent packet history
     */
    [[nodiscard]] std::vector<packet_t> _get_history_sent() const;

    /**
     * @brief Gets a copy of the list of missed packet sequence IDs (Thread-Safe)
     * @return std::vector<uint32_t> Copy of missed packet sequence IDs
     */
    [[nodiscard]] std::vector<uint32_t> _get_missed_packets() const;

    /**
     * @brief Gets the count of packets in the outgoing buffer (Thread-Safe)
     * @return size_t Number of packets waiting to be sent
     */
    [[nodiscard]] size_t _get_buffer_send_size() const;

    /**
     * @brief Gets the count of packets in the incoming buffer (Thread-Safe)
     * @return size_t Number of packets received
     */
    [[nodiscard]] size_t _get_buffer_received_size() const;

    /**
     * @brief Enable or disable compression for packet data (Thread-Safe)
     *
     * When enabled, all packet payloads will be compressed using zlib
     * before transmission and decompressed upon reception.
     *
     * @param enable True to enable compression, false to disable
     */
    void setCompressionEnabled(bool enable);

    /**
     * @brief Check if compression is currently enabled (Thread-Safe)
     * @return true if compression is enabled, false otherwise
     */
    [[nodiscard]] bool isCompressionEnabled() const;

private:
    /**
     * @brief Current sequence ID for outgoing packets
     */
    uint32_t _send_seqid = 0;

    /**
     * @brief Current sequence ID for incoming packets
     */
    uint32_t _recv_seqid = 0;

    /**
     * @brief Authentication key for packet validation
     */
    uint32_t _auth_key = 0;

    /**
     * @brief History of sent packets for retransmission tracking
     */
    std::vector<packet_t> _history_sent;

    /**
     * @brief List of missed packet sequence IDs awaiting retransmission
     */
    std::vector<uint32_t> _missed_packets;

    /**
     * @brief Buffer for received packets awaiting processing
     */
    std::vector<std::unique_ptr<packet_t> > _buffer_received;

    /**
     * @brief Buffer for packets queued for transmission
     */
    std::vector<std::unique_ptr<packet_t> > _buffer_send;

    /**
     * @brief Mutex for protecting all mutable state
     */
    mutable std::mutex _mutex;

    /**
     * @brief Flag to enable/disable compression
     */
    bool _compression_enabled = true;

    /**
     * @brief Resends a packet with the specified sequence ID (Internal, assumes lock held)
     *
     * Looks up a packet in the transmission history and queues it
     * for retransmission if found.
     *
     * @param seqid Sequence ID of the packet to resend
     * @return true if packet was found and queued for resend, false otherwise
     */
    bool _resendPacket(uint32_t seqid);

    /**
     * @brief Internal packet processing handler (Internal, assumes lock held)
     *
     * Processes a deserialized packet, handles sequence validation,
     * acknowledgments, and queues the packet for application processing.
     *
     * @param packet Smart pointer to the packet to process
     */
    void _handlePacket(std::unique_ptr<packet_t> packet);

    /**
     * @brief Compress data using zlib
     * @param data Pointer to data to compress
     * @param size Size of data to compress
     * @return Compressed data as vector
     */
    static std::vector<unsigned char> compress_data(const void* data, size_t size);

    /**
     * @brief Decompress data using zlib
     * @param data Pointer to compressed data
     * @param compressed_size Size of compressed data
     * @param original_size Expected size after decompression
     * @return Decompressed data as vector
     */
    static std::vector<unsigned char> decompress_data(const void* data, size_t compressed_size, size_t original_size);


};

#endif //PACKETMANAGER_H