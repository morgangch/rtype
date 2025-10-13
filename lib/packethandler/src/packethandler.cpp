/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Packet Handler System Implementation
*/

#include "packethandler.h"

PacketHandler::PacketHandler() {
    // Initialize with empty callback map
}

PacketHandler::~PacketHandler() {
    clearCallbacks();
}

// Register a callback for a specific packet type
void PacketHandler::registerCallback(uint8_t packetType, const PacketCallback& callback) {
    _callbacks[packetType] = callback;
}

// Unregister a callback for a specific packet type
void PacketHandler::unregisterCallback(uint8_t packetType) {
    _callbacks.erase(packetType);
}

// Main packet handling function
void PacketHandler::handlePacket(const packet_t& packet) {
    // Find callback for this packet type
    auto it = _callbacks.find(packet.header.type);
    if (it != _callbacks.end() && it->second) {
        // Call the registered callback with the full packet_t
        it->second(packet);
    }
    // If no callback is registered, silently ignore the packet
}

// Process multiple packets at once
void PacketHandler::processPackets(const std::vector<std::unique_ptr<packet_t>>& packets) {
    for (const auto& packet : packets) {
        if (packet) {
            handlePacket(*packet);
        }
    }
}

// Clear all callbacks
void PacketHandler::clearCallbacks() {
    _callbacks.clear();
}

// Check if a callback is registered for a specific packet type
bool PacketHandler::hasCallback(uint8_t packetType) const {
    return _callbacks.find(packetType) != _callbacks.end();
}
