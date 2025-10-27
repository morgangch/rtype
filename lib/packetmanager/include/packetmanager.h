/**
 * @file packetmanager.h
 * @brief Network packet management system with reliability and sequencing
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the PacketManager class which handles reliable network
 * communication with automatic retransmission, sequence tracking, and packet
 * buffering for the R-Type game's networking layer.
 */

#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <vector>
#include <memory>
#include <mutex>
#include "packet.h"

/**
 * @brief Maximum number of packets to keep in transmission history
 *
 * Used for retransmission tracking and acknowledgment handling.
 * Limits memory usage while providing reliable delivery.
 */
#define PACKET_HISTORY_SIZE 512

struct sockaddr_in;

/**
 * @brief Network packet management system with reliability features
 *
 * PacketManager provides reliable packet transmission over UDP by implementing:
 * - Automatic sequence numbering for packet ordering
 * - Retransmission of lost packets
 * - Acknowledgment tracking and handling
 * - Packet buffering for send and receive operations
 * - Serialization and deserialization of packet data
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
     * @brief Default move constructor for efficient transfers
     */
    PacketManager(PacketManager&&) = default;

    /**
     * @brief Default move assignment operator for efficient transfers
     */
    PacketManager& operator=(PacketManager&&) = default;

    /**
     * @brief Deserializes raw packet bytes into a packet structure
     *
     * Converts raw network bytes into a structured packet_t object.
     * This is the unsafe version that requires pre-allocated packet storage.
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
     *
     * @param packet The packet structure to serialize
     * @return std::vector<uint8_t> Vector containing serialized packet bytes
     */
    static std::vector<uint8_t> serializePacket(const packet_t &packet);

    /**
     * @brief Cleans up internal buffers and resets state
     *
     * Clears all pending packets, resets sequence numbers,
     * and prepares the manager for fresh operations.
     */
    void clean();

    /**
     * @brief Handles incoming raw packet bytes from network socket
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
     * @brief Safe packet transmission with automatic memory management
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
     * @brief Handles acknowledgment of missing packets
     *
     * Processes missed packet notifications and initiates retransmission
     * of important packets that were not acknowledged by the receiver.
     */
    void ackMissing();

    /**
     * @brief Retrieves all received packets from the buffer
     *
     * Returns and clears the buffer of packets that have been received
     * and processed, ready for application-level handling.
     *
     * @return std::vector<std::unique_ptr<packet_t>> Vector of received packets
     */
    std::vector<std::unique_ptr<packet_t> > fetchReceivedPackets();

    /**
     * @brief Retrieves all packets queued for transmission
     *
     * Returns and clears the buffer of packets that are ready to be
     * sent over the network.
     *
     * @return std::vector<std::unique_ptr<packet_t>> Vector of packets to send
     */
    std::vector<std::unique_ptr<packet_t> > fetchPacketsToSend();

    /**
     * @brief Gets the current send sequence ID
     * @return uint32_t Current sequence ID for outgoing packets
     */
    [[nodiscard]] uint32_t _get_send_seqid() const { return _send_seqid; }

    /**
     * @brief Gets the current receive sequence ID
     * @return uint32_t Current sequence ID for incoming packets
     */
    [[nodiscard]] uint32_t _get_recv_seqid() const { return _recv_seqid; }

    /**
     * @brief Gets the current authentication key
     * @return uint32_t Current authentication key for packet validation
     */
    [[nodiscard]] uint32_t _get_auth_key() const { return _auth_key; }

    /**
     * @brief Gets the transmission history buffer
     * @return const std::vector<packet_t>* Pointer to sent packet history
     */
    [[nodiscard]] const std::vector<packet_t> *_get_history_sent() const { return &_history_sent; }

    /**
     * @brief Gets the list of missed packet sequence IDs
     * @return const std::vector<uint32_t>* Pointer to missed packet sequence IDs
     */
    [[nodiscard]] const std::vector<uint32_t> *_get_missed_packets() const { return &_missed_packets; }

    /**
     * @brief Gets the outgoing packet buffer
     * @return const std::vector<std::unique_ptr<packet_t>>* Pointer to send buffer
     */
    [[nodiscard]] const std::vector<std::unique_ptr<packet_t> > *_get_buffer_send() const {
        return &_buffer_send;
    }

    /**
     * @brief Gets the incoming packet buffer
     * @return const std::vector<std::unique_ptr<packet_t>>* Pointer to receive buffer
     */
    [[nodiscard]] const std::vector<std::unique_ptr<packet_t> > *_get_buffer_received() const {
        return &_buffer_received;
    }

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
     * @brief Mutex for protecting send-related operations and data
     *
     * Protects: _send_seqid, _buffer_send, _history_sent
     */
    mutable std::mutex _send_mutex;

    /**
     * @brief Mutex for protecting receive-related operations and data
     *
     * Protects: _recv_seqid, _buffer_received, _missed_packets
     */
    mutable std::mutex _recv_mutex;

    /**
     * @brief Mutex for protecting missed packets list
     *
     * Protects: _missed_packets
     */
    mutable std::mutex _missed_mutex;

    /**
     * @brief Mutex for protecting history of sent packets
     *
     * Protects: _history_sent
     */
    mutable std::mutex _history_mutex;

    /**
     * @brief Resends a packet with the specified sequence ID
     *
     * Looks up a packet in the transmission history and queues it
     * for retransmission if found.
     *
     * @param seqid Sequence ID of the packet to resend
     * @return true if packet was found and queued for resend, false otherwise
     */
    bool _resendPacket(uint32_t seqid);

    /**
     * @brief Internal packet processing handler
     *
     * Processes a deserialized packet, handles sequence validation,
     * acknowledgments, and queues the packet for application processing.
     *
     * @param packet Smart pointer to the packet to process
     */
    void _handlePacket(std::unique_ptr<packet_t> packet);
};

#endif //PACKETMANAGER_H
