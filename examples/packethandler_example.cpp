/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Example usage of PacketHandler with PacketManager
*/

#include <iostream>
#include "packethandler.h"
#include "packetmanager.h"
#include "packets.h"

// Example callback functions that handle packet_t and cast the data themselves
void onPingReceived(const packet_t& packet) {
    std::cout << "Ping received! (Type: " << (int)packet.header.type << ")" << std::endl;
}

void onPlayerJoinReceived(const packet_t& packet) {
    // The callback handles the casting of packet.data
    if (packet.header.data_size >= sizeof(PlayerJoinPacket) && packet.data) {
        const PlayerJoinPacket* joinData = static_cast<const PlayerJoinPacket*>(packet.data);
        std::cout << "Player joined: ID=" << joinData->newPlayerId
                  << ", Name=" << joinData->name << std::endl;
    }
}

void onPlayerStateReceived(const packet_t& packet) {
    // The callback handles the casting of packet.data
    if (packet.header.data_size >= sizeof(PlayerStatePacket) && packet.data) {
        const PlayerStatePacket* stateData = static_cast<const PlayerStatePacket*>(packet.data);
        std::cout << "Player state update: ID=" << stateData->playerId
                  << ", Position=(" << stateData->x << "," << stateData->y << ")"
                  << ", HP=" << stateData->hp << std::endl;
    }
}

void onMissileSpawnReceived(const packet_t& packet) {
    // The callback handles the casting of packet.data
    if (packet.header.data_size >= sizeof(MissileSpawnPacket) && packet.data) {
        const MissileSpawnPacket* missileData = static_cast<const MissileSpawnPacket*>(packet.data);
        std::cout << "Missile spawned: ID=" << missileData->missileId
                  << ", Owner=" << missileData->ownerId
                  << ", Position=(" << missileData->x << "," << missileData->y << ")"
                  << ", Damage=" << missileData->damage << std::endl;
    }
}

void onGenericPacketReceived(const packet_t& packet) {
    std::cout << "Generic packet received: Type=" << (int)packet.header.type
              << ", Size=" << packet.header.data_size << std::endl;
}

int main() {
    // Create instances
    PacketManager packetManager;
    PacketHandler packetHandler;

    // Register callbacks for specific packet types using uint8_t numbers
    packetHandler.registerCallback(0, onPingReceived);           // Type 0 = Ping
    packetHandler.registerCallback(2, onPlayerJoinReceived);     // Type 2 = PlayerJoin
    packetHandler.registerCallback(6, onPlayerStateReceived);    // Type 6 = PlayerState
    packetHandler.registerCallback(9, onMissileSpawnReceived);   // Type 9 = MissileSpawn

    // Register a generic handler for type 3 (whatever that might be)
    packetHandler.registerCallback(3, onGenericPacketReceived);

    std::cout << "=== Packet Handler Example ===" << std::endl;
    std::cout << "Registered callbacks for packet types: 0, 2, 3, 6, 9" << std::endl;

    // Example workflow explanation:
    std::cout << "\nExample workflow:" << std::endl;
    std::cout << "1. Register callbacks: packetHandler.registerCallback(packetType, callback)" << std::endl;
    std::cout << "2. Raw packet data received from network" << std::endl;
    std::cout << "3. PacketManager.handlePacketBytes() processes raw data" << std::endl;
    std::cout << "4. PacketManager.fetchReceivedPackets() returns parsed packets" << std::endl;
    std::cout << "5. PacketHandler.processPackets() calls registered callbacks" << std::endl;
    std::cout << "6. Each callback receives packet_t and casts packet.data as needed" << std::endl;

    // In a real application, you would:
    /*
    // Receive raw bytes from socket
    uint8_t* rawData = receiveFromSocket(&dataSize);

    // Process with PacketManager
    packetManager.handlePacketBytes(rawData, dataSize);

    // Get processed packets
    auto receivedPackets = packetManager.fetchReceivedPackets();

    // Handle with registered callbacks
    packetHandler.processPackets(receivedPackets);
    */

    return 0;
}
