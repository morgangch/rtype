/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Packet Handler System
*/

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <functional>
#include <unordered_map>
#include <memory>
#include "packet.h"

// Callback function type - receives the full packet_t structure
// The callback is responsible for casting packet.data to the appropriate type
using PacketCallback = std::function<void(const packet_t&)>;

class PacketHandler {
public:
    PacketHandler();
    ~PacketHandler();

    // Register a callback for a specific packet type (identified by uint8_t)
    void registerCallback(uint8_t packetType, const PacketCallback& callback);

    // Unregister a callback for a specific packet type
    void unregisterCallback(uint8_t packetType);

    // Main function to handle a packet from PacketManager
    void handlePacket(const packet_t& packet);

    // Process all packets from PacketManager
    void processPackets(const std::vector<std::unique_ptr<packet_t>>& packets);

    // Clear all registered callbacks
    void clearCallbacks();

    // Check if a callback is registered for a specific packet type
    bool hasCallback(uint8_t packetType) const;

private:
    // Storage for callbacks, indexed by packet type
    std::unordered_map<uint8_t, PacketCallback> _callbacks;
};

#endif //PACKETHANDLER_H
