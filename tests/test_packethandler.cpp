/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Test for PacketHandler integration with PacketManager
*/

#include <cassert>
#include <iostream>
#include <cstring>
#include "packethandler.h"
#include "packetmanager.h"
#include "packets.h"

// Test callback counters
int pingCallbackCount = 0;
int playerJoinCallbackCount = 0;
int missileSpawnCallbackCount = 0;
int type3CallbackCount = 0;

// Test callback functions that receive packet_t and handle casting themselves
void testPingCallback(const packet_t& packet) {
    pingCallbackCount++;
    std::cout << "Ping callback triggered (Type: " << (int)packet.header.type << ")" << std::endl;
}

void testPlayerJoinCallback(const packet_t& packet) {
    playerJoinCallbackCount++;

    // Callback handles casting the packet data
    if (packet.header.data_size >= sizeof(PlayerJoinPacket) && packet.data) {
        const PlayerJoinPacket* joinData = static_cast<const PlayerJoinPacket*>(packet.data);
        std::cout << "PlayerJoin callback triggered - ID: " << joinData->newPlayerId
                  << ", Name: " << joinData->name << std::endl;
    } else {
        std::cout << "PlayerJoin callback triggered but invalid data" << std::endl;
    }
}

void testMissileSpawnCallback(const packet_t& packet) {
    missileSpawnCallbackCount++;

    // Callback handles casting the packet data
    if (packet.header.data_size >= sizeof(MissileSpawnPacket) && packet.data) {
        const MissileSpawnPacket* missileData = static_cast<const MissileSpawnPacket*>(packet.data);
        std::cout << "MissileSpawn callback triggered - ID: " << missileData->missileId
                  << ", Owner: " << missileData->ownerId << std::endl;
    } else {
        std::cout << "MissileSpawn callback triggered but invalid data" << std::endl;
    }
}

void testType3Callback(const packet_t& packet) {
    type3CallbackCount++;
    std::cout << "Type 3 callback triggered - Size: " << packet.header.data_size << std::endl;
}

int main() {
    std::cout << "=== PacketHandler Integration Test ===" << std::endl;

    PacketManager packetManager;
    PacketHandler packetHandler;

    // Register test callbacks using packet type numbers
    packetHandler.registerCallback(0, testPingCallback);        // Type 0
    packetHandler.registerCallback(2, testPlayerJoinCallback);  // Type 2
    packetHandler.registerCallback(9, testMissileSpawnCallback); // Type 9
    packetHandler.registerCallback(3, testType3Callback);       // Type 3

    // Test 1: Create and send a PlayerJoin packet (type 2)
    std::cout << "\nTest 1: PlayerJoin packet (type 2)" << std::endl;
    PlayerJoinPacket playerJoinData;
    playerJoinData.newPlayerId = 12345;
    strncpy(playerJoinData.name, "TestPlayer", sizeof(playerJoinData.name) - 1);
    playerJoinData.name[sizeof(playerJoinData.name) - 1] = '\0';

    size_t serializedSize;
    auto serializedData = packetManager.sendPacketBytesSafe(
        &playerJoinData,
        sizeof(PlayerJoinPacket),
        2, // packet type 2
        &serializedSize
    );

    // Simulate receiving the packet back
    packetManager.handlePacketBytes(serializedData.get(), serializedSize);

    // Process received packets
    auto receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets);

    // Test 2: Create and send a MissileSpawn packet (type 9)
    std::cout << "\nTest 2: MissileSpawn packet (type 9)" << std::endl;
    MissileSpawnPacket missileData;
    missileData.missileId = 9999;
    missileData.ownerId = 12345;
    missileData.x = 100.5f;
    missileData.y = 200.5f;
    missileData.dir = 1.5f;
    missileData.damage = 50;

    auto missileSerializedData = packetManager.sendPacketBytesSafe(
        &missileData,
        sizeof(MissileSpawnPacket),
        9, // packet type 9
        &serializedSize
    );

    // Simulate receiving the missile packet
    packetManager.handlePacketBytes(missileSerializedData.get(), serializedSize);

    // Process received packets
    receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets);

    // Test 3: Send a Ping packet (type 0, empty packet)
    std::cout << "\nTest 3: Ping packet (type 0, empty)" << std::endl;
    auto pingSerializedData = packetManager.sendPacketBytesSafe(
        nullptr,
        0,
        0, // packet type 0
        &serializedSize
    );

    // Simulate receiving the ping packet
    packetManager.handlePacketBytes(pingSerializedData.get(), serializedSize);

    // Process received packets
    receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets);

    // Test 4: Send a packet type 3 (generic test)
    std::cout << "\nTest 4: Generic packet (type 3)" << std::endl;
    uint32_t testData = 42;
    auto type3SerializedData = packetManager.sendPacketBytesSafe(
        &testData,
        sizeof(uint32_t),
        3, // packet type 3
        &serializedSize
    );

    // Simulate receiving the type 3 packet
    packetManager.handlePacketBytes(type3SerializedData.get(), serializedSize);

    // Process received packets
    receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets);

    // Test 5: Send a packet with no registered callback (should be ignored)
    std::cout << "\nTest 5: Unregistered packet type (should be ignored)" << std::endl;
    auto unregisteredData = packetManager.sendPacketBytesSafe(
        nullptr,
        0,
        99, // packet type 99 - no callback registered
        &serializedSize
    );

    packetManager.handlePacketBytes(unregisteredData.get(), serializedSize);
    receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets); // Should be silently ignored

    // Verify results
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Ping callbacks (type 0): " << pingCallbackCount << " (expected: 1)" << std::endl;
    std::cout << "PlayerJoin callbacks (type 2): " << playerJoinCallbackCount << " (expected: 1)" << std::endl;
    std::cout << "MissileSpawn callbacks (type 9): " << missileSpawnCallbackCount << " (expected: 1)" << std::endl;
    std::cout << "Type 3 callbacks: " << type3CallbackCount << " (expected: 1)" << std::endl;

    // Assert tests passed
    assert(pingCallbackCount == 1);
    assert(playerJoinCallbackCount == 1);
    assert(missileSpawnCallbackCount == 1);
    assert(type3CallbackCount == 1);

    // Test callback management
    std::cout << "\n=== Testing Callback Management ===" << std::endl;
    assert(packetHandler.hasCallback(0) == true);
    assert(packetHandler.hasCallback(2) == true);
    assert(packetHandler.hasCallback(99) == false);

    packetHandler.unregisterCallback(0);
    assert(packetHandler.hasCallback(0) == false);

    std::cout << "✅ All tests passed!" << std::endl;
    std::cout << "PacketHandler successfully integrated with PacketManager!" << std::endl;
    std::cout << "- Callbacks are registered by packet type (uint8_t)" << std::endl;
    std::cout << "- Callbacks receive packet_t and handle data casting themselves" << std::endl;
    std::cout << "- Unregistered packet types are silently ignored" << std::endl;

    return 0;
}
