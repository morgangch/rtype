/**
 * @file packethandler.h
 * @brief Packet Handler System for routing network packets to registered callbacks
 * @author EPITECH PROJECT, 2025
 * @date 2025
 */

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <functional>
#include <unordered_map>
#include <memory>
#include "packet.h"

/**
 * @brief Callback function type for packet handling
 *
 * This function type receives the full packet_t structure and is responsible
 * for casting packet.data to the appropriate type based on packet.header.type
 */
using PacketCallback = std::function<void(const packet_t &)>;

/**
 * @brief Packet Handler System for managing packet callbacks
 *
 * The PacketHandler class provides a callback-based system for processing
 * network packets. It automatically routes incoming packets to registered
 * callback functions based on the packet type. Each callback receives the
 * full packet_t structure and handles its own data casting.
 */
class PacketHandler {
public:
    /**
     * @brief Default constructor
     *
     * Initializes the PacketHandler with an empty callback map
     */
    PacketHandler();

    /**
     * @brief Destructor
     *
     * Cleans up all registered callbacks and resources
     */
    ~PacketHandler();

    /**
     * @brief Register a callback for a specific packet type
     *
     * Associates a callback function with a packet type identifier.
     * When a packet of this type is processed, the registered callback
     * will be invoked with the packet data.
     *
     * @param packetType The packet type identifier (0-255)
     * @param callback The callback function to register for this packet type
     */
    void registerCallback(uint8_t packetType, const PacketCallback &callback);

    /**
     * @brief Unregister a callback for a specific packet type
     *
     * Removes the callback associated with the specified packet type.
     * After unregistering, packets of this type will be silently ignored.
     *
     * @param packetType The packet type identifier to unregister
     */
    void unregisterCallback(uint8_t packetType);

    /**
     * @brief Process a single packet by calling the appropriate callback
     *
     * Routes the packet to its registered callback based on packet.header.type.
     * If no callback is registered for the packet type, the packet is ignored.
     *
     * @param packet The packet to process
     */
    void handlePacket(const packet_t &packet);

    /**
     * @brief Process multiple packets from PacketManager
     *
     * Convenience function to process a vector of packets in sequence.
     * Each packet is routed to its appropriate callback if one is registered.
     *
     * @param packets Vector of unique_ptr packets to process
     */
    void processPackets(const std::vector<std::unique_ptr<packet_t> > &packets);

    /**
     * @brief Clear all registered callbacks
     *
     * Removes all registered callbacks from the handler. After calling this,
     * all incoming packets will be silently ignored until new callbacks are registered.
     */
    void clearCallbacks();

    /**
     * @brief Check if a callback is registered for a specific packet type
     *
     * @param packetType The packet type to check
     * @return true if a callback is registered for this packet type, false otherwise
     */
    bool hasCallback(uint8_t packetType) const;

private:
    /**
     * @brief Storage for callbacks, indexed by packet type
     *
     * Maps packet type identifiers (uint8_t) to their corresponding callback functions.
     * Uses unordered_map for O(1) average lookup performance.
     */
    std::unordered_map<uint8_t, PacketCallback> _callbacks;
};

#endif //PACKETHANDLER_H
