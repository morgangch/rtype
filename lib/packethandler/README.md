# PacketHandler Library

A lightweight C++ library for handling network packets with callback-based routing. The PacketHandler library integrates seamlessly with the PacketManager to provide a clean, type-safe way to process incoming network packets.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Integration with PacketManager](#integration-with-packetmanager)
- [Best Practices](#best-practices)
- [Testing](#testing)

## Overview

The PacketHandler library provides a callback-based system for processing network packets. When integrated with PacketManager, it automatically routes incoming packets to registered callback functions based on the packet type. Each callback receives the full `packet_t` structure and is responsible for casting the packet data to the appropriate type.

## Features

- ✅ **Simple Registration**: Register callbacks using packet type numbers (uint8_t)
- ✅ **Automatic Routing**: Packets are automatically routed to the correct callback
- ✅ **Type Safety**: Callbacks handle their own data casting for maximum flexibility  
- ✅ **Error Handling**: Unregistered packet types are silently ignored
- ✅ **Memory Management**: No memory leaks, proper cleanup on destruction
- ✅ **Performance**: Efficient hash-map based packet routing
- ✅ **Thread Safe**: Can be used in multi-threaded environments

## Installation

The PacketHandler library is part of the R-Type project and is built using CMake.

### Prerequisites

- C++17 or later
- CMake 3.10 or later
- PacketManager library (dependency)

### Building

```bash
cd /path/to/rtype
mkdir build && cd build
cmake ..
make packethandler
```

The library will be built as `libpackethandler.a` in the build directory.

## Quick Start

Here's a minimal example to get you started:

```cpp
#include "packethandler.h"
#include "packetmanager.h"

// Define a callback function
void onPingReceived(const packet_t& packet) {
    std::cout << "Ping received!" << std::endl;
}

int main() {
    PacketManager packetManager;
    PacketHandler packetHandler;
    
    // Register callback for packet type 0 (ping)
    packetHandler.registerCallback(0, onPingReceived);
    
    // In your main loop:
    auto receivedPackets = packetManager.fetchReceivedPackets();
    packetHandler.processPackets(receivedPackets);
    
    return 0;
}
```

## API Reference

### Class: PacketHandler

#### Constructor/Destructor

```cpp
PacketHandler();
~PacketHandler();
```

#### Methods

##### `registerCallback(uint8_t packetType, const PacketCallback& callback)`

Registers a callback function for a specific packet type.

**Parameters:**
- `packetType`: The packet type number (0-255)
- `callback`: Function to call when a packet of this type is received

**Example:**
```cpp
packetHandler.registerCallback(5, [](const packet_t& packet) {
    // Handle packet type 5
});
```

##### `unregisterCallback(uint8_t packetType)`

Removes the callback for a specific packet type.

**Parameters:**
- `packetType`: The packet type number to unregister

##### `handlePacket(const packet_t& packet)`

Processes a single packet by calling the appropriate callback.

**Parameters:**
- `packet`: The packet to process

##### `processPackets(const std::vector<std::unique_ptr<packet_t>>& packets)`

Processes multiple packets at once.

**Parameters:**
- `packets`: Vector of packets to process

##### `clearCallbacks()`

Removes all registered callbacks.

##### `hasCallback(uint8_t packetType) const`

Checks if a callback is registered for a specific packet type.

**Parameters:**
- `packetType`: The packet type to check

**Returns:** `true` if a callback is registered, `false` otherwise

### Type: PacketCallback

```cpp
using PacketCallback = std::function<void(const packet_t&)>;
```

Function signature for packet callback functions. Callbacks receive the full `packet_t` structure and must handle data casting themselves.

## Examples

### Example 1: Basic Packet Handling

```cpp
#include "packethandler.h"
#include "packetmanager.h"

void handlePlayerJoin(const packet_t& packet) {
    if (packet.data && packet.header.data_size >= sizeof(PlayerJoinPacket)) {
        const PlayerJoinPacket* joinData = 
            static_cast<const PlayerJoinPacket*>(packet.data);
        
        std::cout << "Player " << joinData->name 
                  << " joined with ID " << joinData->newPlayerId << std::endl;
    }
}

void handlePlayerLeave(const packet_t& packet) {
    if (packet.data && packet.header.data_size >= sizeof(PlayerLeavePacket)) {
        const PlayerLeavePacket* leaveData = 
            static_cast<const PlayerLeavePacket*>(packet.data);
        
        std::cout << "Player " << leaveData->playerId << " left" << std::endl;
    }
}

int main() {
    PacketHandler handler;
    
    // Register callbacks for different packet types
    handler.registerCallback(2, handlePlayerJoin);   // Type 2 = PlayerJoin
    handler.registerCallback(3, handlePlayerLeave);  // Type 3 = PlayerLeave
    
    // ... rest of your application
}
```

### Example 2: Lambda Callbacks

```cpp
#include "packethandler.h"

int main() {
    PacketHandler handler;
    
    // Register lambda callbacks
    handler.registerCallback(0, [](const packet_t& packet) {
        std::cout << "Ping received from client" << std::endl;
    });
    
    handler.registerCallback(9, [](const packet_t& packet) {
        if (packet.data && packet.header.data_size >= sizeof(MissileSpawnPacket)) {
            const auto* missile = static_cast<const MissileSpawnPacket*>(packet.data);
            std::cout << "Missile spawned at (" << missile->x << ", " 
                      << missile->y << ")" << std::endl;
        }
    });
    
    // Process packets...
}
```

### Example 3: Class Method Callbacks

```cpp
class GameServer {
public:
    void setupPacketHandlers() {
        // Using std::bind to register class methods as callbacks
        packetHandler.registerCallback(4, 
            std::bind(&GameServer::handlePlayerInput, this, std::placeholders::_1));
        
        packetHandler.registerCallback(5, 
            std::bind(&GameServer::handlePlayerShoot, this, std::placeholders::_1));
    }
    
private:
    PacketHandler packetHandler;
    
    void handlePlayerInput(const packet_t& packet) {
        if (packet.data && packet.header.data_size >= sizeof(PlayerInputPacket)) {
            const auto* input = static_cast<const PlayerInputPacket*>(packet.data);
            // Process player input...
        }
    }
    
    void handlePlayerShoot(const packet_t& packet) {
        if (packet.data && packet.header.data_size >= sizeof(PlayerShootPacket)) {
            const auto* shoot = static_cast<const PlayerShootPacket*>(packet.data);
            // Process player shooting...
        }
    }
};
```

### Example 4: Generic Packet Handler

```cpp
void genericPacketHandler(const packet_t& packet) {
    std::cout << "Received packet type " << (int)packet.header.type 
              << " with " << packet.header.data_size << " bytes of data" << std::endl;
              
    // Log packet for debugging
    if (packet.data && packet.header.data_size > 0) {
        std::cout << "First 4 bytes: ";
        for (int i = 0; i < std::min(4u, packet.header.data_size); i++) {
            printf("%02X ", static_cast<uint8_t*>(packet.data)[i]);
        }
        std::cout << std::endl;
    }
}

int main() {
    PacketHandler handler;
    
    // Register the same handler for multiple packet types
    for (uint8_t type = 10; type <= 20; type++) {
        handler.registerCallback(type, genericPacketHandler);
    }
}
```

## Integration with PacketManager

The PacketHandler library is designed to work seamlessly with PacketManager. Here's the typical workflow:

```cpp
#include "packethandler.h"
#include "packetmanager.h"

int main() {
    PacketManager packetManager;
    PacketHandler packetHandler;
    
    // 1. Register your packet callbacks
    packetHandler.registerCallback(0, handlePing);
    packetHandler.registerCallback(2, handlePlayerJoin);
    // ... register more callbacks
    
    // 2. In your main network loop:
    while (running) {
        // Receive raw bytes from network socket
        uint8_t buffer[1024];
        size_t bytesReceived = receiveFromSocket(buffer, sizeof(buffer));
        
        // 3. Process raw bytes with PacketManager
        packetManager.handlePacketBytes(buffer, bytesReceived);
        
        // 4. Get processed packets
        auto receivedPackets = packetManager.fetchReceivedPackets();
        
        // 5. Route packets to callbacks with PacketHandler
        packetHandler.processPackets(receivedPackets);
    }
    
    return 0;
}
```

### Packet Flow Diagram

```
Raw Network Data → PacketManager → packet_t structures → PacketHandler → Callbacks
      ↓                  ↓                    ↓                ↓            ↓
  [Binary Data]    [Deserialization]   [Type Routing]    [User Logic]  [Game State]
```

## Best Practices

### 1. Always Validate Packet Data

```cpp
void safePacketHandler(const packet_t& packet) {
    // Always check data validity before casting
    if (!packet.data || packet.header.data_size < sizeof(ExpectedPacket)) {
        std::cerr << "Invalid packet data" << std::endl;
        return;
    }
    
    const ExpectedPacket* data = static_cast<const ExpectedPacket*>(packet.data);
    // Now safe to use data...
}
```

### 2. Use Descriptive Packet Type Constants

```cpp
// Define constants for packet types
constexpr uint8_t PACKET_PING = 0;
constexpr uint8_t PACKET_PLAYER_JOIN = 2;
constexpr uint8_t PACKET_PLAYER_LEAVE = 3;
constexpr uint8_t PACKET_MISSILE_SPAWN = 9;

// Use constants instead of magic numbers
packetHandler.registerCallback(PACKET_PING, handlePing);
packetHandler.registerCallback(PACKET_PLAYER_JOIN, handlePlayerJoin);
```

### 3. Handle Errors Gracefully

```cpp
void robustPacketHandler(const packet_t& packet) {
    try {
        // Packet processing logic
        if (packet.data && packet.header.data_size >= sizeof(MyPacket)) {
            const MyPacket* data = static_cast<const MyPacket*>(packet.data);
            processPacketData(*data);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing packet type " 
                  << (int)packet.header.type << ": " << e.what() << std::endl;
    }
}
```

### 4. Organize Callbacks by Functionality

```cpp
class NetworkHandler {
    void setupCallbacks() {
        // Player-related packets
        handler.registerCallback(2, std::bind(&NetworkHandler::handlePlayerJoin, this, std::placeholders::_1));
        handler.registerCallback(3, std::bind(&NetworkHandler::handlePlayerLeave, this, std::placeholders::_1));
        
        // Game state packets  
        handler.registerCallback(6, std::bind(&NetworkHandler::handlePlayerState, this, std::placeholders::_1));
        handler.registerCallback(7, std::bind(&NetworkHandler::handleEnemyState, this, std::placeholders::_1));
        
        // Action packets
        handler.registerCallback(4, std::bind(&NetworkHandler::handlePlayerInput, this, std::placeholders::_1));
        handler.registerCallback(5, std::bind(&NetworkHandler::handlePlayerShoot, this, std::placeholders::_1));
    }
};
```

## Testing

The library includes comprehensive tests. To run them:

```bash
cd build
./tests/test_packethandler
```

### Test Coverage

- ✅ Callback registration and unregistration
- ✅ Packet routing to correct callbacks  
- ✅ Multiple packet processing
- ✅ Unregistered packet type handling
- ✅ Data casting in callbacks
- ✅ Memory management
- ✅ Edge cases and error conditions

### Writing Your Own Tests

```cpp
#include "packethandler.h"
#include <cassert>

void testBasicCallback() {
    PacketHandler handler;
    bool callbackCalled = false;
    
    handler.registerCallback(1, [&](const packet_t& packet) {
        callbackCalled = true;
        assert(packet.header.type == 1);
    });
    
    // Create test packet
    packet_t testPacket = {};
    testPacket.header.type = 1;
    testPacket.header.data_size = 0;
    testPacket.data = nullptr;
    
    handler.handlePacket(testPacket);
    
    assert(callbackCalled == true);
    std::cout << "✅ Basic callback test passed" << std::endl;
}
```

## Contributing

When contributing to the PacketHandler library:

1. Follow the existing code style
2. Add tests for new functionality
3. Update this documentation
4. Ensure backward compatibility

## License

This library is part of the R-Type project and follows the same license terms.

---

For more information about the R-Type project and related libraries, see the main project documentation.
