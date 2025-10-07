/**
 * @file packet.h
 * @brief Core packet structures for network communication
 * @author R-Type Team
 * @date 2025
 *
 * This file defines the fundamental packet structures used throughout
 * the R-Type networking system. It provides the basic building blocks
 * for reliable network communication including headers and data payloads.
 */

#ifndef PACKET_H
#define PACKET_H
#include <cstdint>

/**
 * @brief Network packet header structure
 *
 * Contains all metadata required for reliable packet transmission including
 * sequence numbers, acknowledgments, authentication, and routing information.
 * The header is transmitted with every packet to enable proper networking
 * features like reliability, ordering, and security.
 */
typedef struct packet_header_s {
    /**
     * @brief Sequence ID for packet ordering and acknowledgment
     *
     * Unique identifier assigned to each important packet for tracking
     * delivery and ensuring proper ordering. Used for reliable transmission.
     */
    uint32_t seqid;

    /**
     * @brief Acknowledgment number for received packets
     *
     * Confirms receipt of packets up to this sequence number.
     * Used by the receiver to acknowledge successful packet delivery.
     */
    uint32_t ack;

    /**
     * @brief Packet type identifier (0-255)
     *
     * Identifies the type of data contained in the packet payload.
     * Used for routing packets to appropriate handlers and processing logic.
     */
    uint8_t type;

    /**
     * @brief Authentication key for packet validation
     *
     * Security token used to verify packet authenticity and prevent
     * unauthorized or malicious packets from being processed.
     */
    uint32_t auth;

    /**
     * @brief Client IP address as 4-byte array
     *
     * IPv4 address of the packet sender stored as individual bytes.
     * Used for routing responses and client identification.
     */
    uint8_t client_addr[4];

    /**
     * @brief Client port number
     *
     * UDP port number of the packet sender. Combined with client_addr
     * to form a complete network endpoint address.
     */
    uint16_t client_port;

    /**
     * @brief Size of the packet data payload in bytes
     *
     * Indicates how many bytes of data follow the header.
     * Used for proper packet parsing and memory allocation.
     */
    uint32_t data_size;
} packet_header_t;

/**
 * @brief Complete network packet structure
 *
 * Represents a complete network packet consisting of a header with
 * metadata and a data payload. This is the primary structure used
 * throughout the networking system for all packet operations.
 */
typedef struct packet_s {
    /**
     * @brief Packet header containing metadata
     *
     * Contains all routing, sequencing, and authentication information
     * required for reliable packet transmission and processing.
     */
    packet_header_t header;

    /**
     * @brief Packet data payload
     *
     * Pointer to the actual packet data. The size is specified in
     * header.data_size. Memory management of this data is handled
     * by the packet management systems.
     */
    void *data;
} packet_t;

#endif //PACKET_H
