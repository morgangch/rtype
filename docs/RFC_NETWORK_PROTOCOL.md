# RFC: R-Type Network Protocol Specification

**Status:** Draft  
**Version:** 1.0  
**Date:** November 1, 2025  
**Authors:** R-Type Development Team

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Architecture Overview](#2-architecture-overview)
3. [Transport Layer](#3-transport-layer)
4. [Packet Structure](#4-packet-structure)
5. [Compression](#5-compression)
6. [Packet Types](#6-packet-types)
7. [Connection Flow](#7-connection-flow)
8. [Client Implementation Guide](#8-client-implementation-guide)
9. [Security Considerations](#9-security-considerations)
10. [Examples](#10-examples)

---

## 1. Introduction

### 1.1 Purpose

This document specifies the R-Type multiplayer game network protocol. It defines the packet structures, communication patterns, and implementation requirements for creating compatible R-Type clients and servers.

### 1.2 Scope

This protocol covers:
- UDP-based reliable packet transmission
- Game state synchronization
- Player input handling
- Entity lifecycle management
- Room/lobby management

### 1.3 Conventions

- **Client → Server**: Packet sent from client to server
- **Server → Client**: Packet sent from server to specific client
- **Server → All**: Broadcast packet sent to all clients in a room
- All multi-byte integers use **network byte order** (big-endian)
- All floating-point values are IEEE 754 single-precision (32-bit)

---

## 2. Architecture Overview

### 2.1 Network Model

The R-Type protocol uses a **client-server architecture** with:
- **UDP** for low-latency communication
- **Reliable transmission** layer for critical packets
- **Sequence numbering** for packet ordering
- **Acknowledgment system** for packet delivery confirmation

### 2.2 Game Flow

```
1. Client connects → Join Room
2. Lobby phase → Player ready states
3. Game start → Entity spawning and synchronization
4. Active gameplay → Input transmission and state updates
5. Game end or disconnect → Cleanup
```

---

## 3. Transport Layer

### 3.1 Reliability Mechanism

The protocol implements reliability over UDP through:

- **Sequence IDs**: Each important packet receives a unique sequence number
- **Acknowledgments**: Receivers send ACK packets for missed sequences
- **Retransmission**: Senders retransmit unacknowledged packets
- **Packet History**: Last 512 packets kept for potential retransmission

### 3.2 Packet Delivery

- **Important packets** (seqid != 0): Tracked and retransmitted if lost
- **Unimportant packets** (seqid == 0): Fire-and-forget, no retransmission

---

## 4. Packet Structure

### 4.1 Packet Header

All packets begin with a 31-byte header:

```c
struct packet_header_t {
    uint32_t seqid;           // Sequence ID (0 = unimportant)
    uint32_t ack;             // Acknowledgment ID
    uint8_t  type;            // Packet type identifier
    uint32_t auth;            // Authentication key
    uint8_t  client_addr[4];  // Client IPv4 address
    uint16_t client_port;     // Client UDP port
    uint32_t data_size;       // Size of payload (compressed if original_size != 0)
    uint32_t original_size;   // Original size before compression (0 = not compressed)
};
```

**Field Descriptions:**

- `seqid`: Sequence number for reliable delivery. 0 indicates unreliable packet.
- `ack`: If non-zero, this is an acknowledgment for the specified sequence ID.
- `type`: Identifies the packet payload type (see Section 6).
- `auth`: Authentication token (default: 0, reserved for future use).
- `client_addr`: Sender's IPv4 address as 4 bytes.
- `client_port`: Sender's UDP port number.
- `data_size`: Number of bytes in the payload. If compressed, this is the compressed size.
- `original_size`: If non-zero, payload is compressed. This field contains the uncompressed size.

### 4.2 Complete Packet

```
+-------------------+
| Packet Header     | 31 bytes
+-------------------+
| Payload Data      | data_size bytes (may be compressed)
+-------------------+
```

---

## 5. Compression

### 5.1 Compression Algorithm

The protocol uses **zlib** compression with `Z_BEST_SPEED` level for real-time performance.

### 5.2 Compression Behavior

- **Enabled by default** for payloads larger than 32 bytes
- **Skipped** if compressed size ≥ original size
- **Transparent** to application layer

### 5.3 Compression Detection

A packet is compressed if `header.original_size != 0`:
- `data_size`: Compressed payload size
- `original_size`: Uncompressed payload size

### 5.4 Decompression

Receivers must:
1. Check if `original_size != 0`
2. If compressed, allocate buffer of size `original_size`
3. Decompress `data_size` bytes into `original_size` bytes using zlib
4. Use decompressed data for application logic

---

## 6. Packet Types

### 6.1 Packet Type Enumeration

```c
enum Packets {
    PLAYER_DISCONNECT      = 1,
    JOIN_ROOM              = 2,
    JOIN_ROOM_ACCEPTED     = 3,
    GAME_START_REQUEST     = 4,
    ROOM_ADMIN_UPDATE      = 5,
    PLAYER_JOIN            = 6,
    PLAYER_STATE           = 7,
    ENTITY_DESTROY         = 8,
    PLAYER_INPUT           = 9,
    PLAYER_READY           = 10,
    LOBBY_STATE            = 11,
    GAME_START             = 12,
    PLAYER_SHOOT           = 13,
    SPAWN_PROJECTILE       = 14,
    SPAWN_ENEMY            = 15,
    SPAWN_BOSS_REQUEST     = 16,
    PLAYER_SCORE_UPDATE    = 17,
};
```

---

### 6.2 JOIN_ROOM (Type 2)

**Direction:** Client → Server  
**Purpose:** Request to join or create a game room  
**Reliability:** Important (seqid != 0)

**Payload Structure:**

```c
struct JoinRoomPacket {
    char     name[32];    // Player name (null-terminated)
    uint32_t joinCode;    // Room code (0 = create new, 1 = join public)
};
```

**Behavior:**
- `joinCode = 0`: Create a new private room
- `joinCode = 1`: Join a public room (matchmaking)
- `joinCode = <code>`: Join specific room by code

**Response:** Server sends `JOIN_ROOM_ACCEPTED` or connection timeout

---

### 6.3 JOIN_ROOM_ACCEPTED (Type 3)

**Direction:** Server → Client  
**Purpose:** Confirm successful room join  
**Reliability:** Important

**Payload Structure:**

```c
struct JoinRoomAcceptedPacket {
    uint32_t roomCode;         // Assigned room code
    bool     admin;            // true if player is room admin
    uint32_t playerServerId;   // Server entity ID for this player
};
```

**Behavior:**
- Client stores `roomCode` for future reference
- Client stores `playerServerId` to identify own entity
- Admin can trigger game start

---

### 6.4 PLAYER_JOIN (Type 6)

**Direction:** Server → Client  
**Purpose:** Notify about new player joining the room  
**Reliability:** Important

**Payload Structure:**

```c
struct PlayerJoinPacket {
    uint32_t newPlayerId;   // Server entity ID of joining player
    char     name[32];      // Player name
};
```

**Behavior:**
- Client creates UI element for new player in lobby
- Client tracks all player IDs in current room

---

### 6.5 PLAYER_READY (Type 10)

**Direction:** Client → Server  
**Purpose:** Toggle player's ready state in lobby  
**Reliability:** Important

**Payload Structure:**

```c
struct PlayerReadyPacket {
    bool isReady;   // true = ready, false = not ready
};
```

**Behavior:**
- Server updates player's ready state
- Server broadcasts `LOBBY_STATE` to all clients

---

### 6.6 LOBBY_STATE (Type 11)

**Direction:** Server → All  
**Purpose:** Update lobby information  
**Reliability:** Unimportant (frequent updates)

**Payload Structure:**

```c
struct LobbyStatePacket {
    uint32_t totalPlayers;   // Total players in room
    uint32_t readyPlayers;   // Number of ready players
};
```

**Behavior:**
- Client updates lobby UI
- When all players ready, admin can start game

---

### 6.7 GAME_START_REQUEST (Type 4)

**Direction:** Client → Server  
**Purpose:** Admin requests game start  
**Reliability:** Important

**Payload Structure:** Empty (header only)

**Behavior:**
- Only accepted from room admin
- All players must be ready
- Server transitions room to game state

**Response:** Server sends `GAME_START` to all clients

---

### 6.8 GAME_START (Type 12)

**Direction:** Server → All  
**Purpose:** Signal game start  
**Reliability:** Important

**Payload Structure:** Empty (header only)

**Behavior:**
- Client transitions from lobby to game state
- Client prepares to receive entity spawn packets
- Gameplay begins

---

### 6.9 PLAYER_INPUT (Type 9)

**Direction:** Client → Server  
**Purpose:** Send player input state  
**Reliability:** Unimportant (sent frequently, ~60Hz)

**Payload Structure:**

```c
struct PlayerInputPacket {
    bool  moveUp;      // W or Up arrow
    bool  moveDown;    // S or Down arrow
    bool  moveLeft;    // A or Left arrow
    bool  moveRight;   // D or Right arrow
    float clientX;     // Client's predicted X position
    float clientY;     // Client's predicted Y position
};
```

**Behavior:**
- Sent every frame while input state changes
- Server processes input server-side (authoritative)
- `clientX/Y` used for client-side prediction validation

---

### 6.10 PLAYER_SHOOT (Type 13)

**Direction:** Client → Server  
**Purpose:** Player fires weapon  
**Reliability:** Important

**Payload Structure:**

```c
struct PlayerShootPacket {
    bool  isCharged;   // true = charged shot, false = normal
    float playerX;     // Player X position when shooting
    float playerY;     // Player Y position when shooting
};
```

**Behavior:**
- Server validates shot (cooldown, alive state, etc.)
- Server creates projectile entity
- Server broadcasts `SPAWN_PROJECTILE`

---

### 6.11 SPAWN_PROJECTILE (Type 14)

**Direction:** Server → All  
**Purpose:** Spawn a projectile entity  
**Reliability:** Important

**Payload Structure:**

```c
struct SpawnProjectilePacket {
    uint32_t projectileId;   // Server entity ID
    uint32_t ownerId;        // Entity ID of shooter (0 = enemy)
    float    x, y;           // Spawn position
    float    vx, vy;         // Velocity vector
    uint16_t damage;         // Damage value
    bool     piercing;       // true = pierces enemies
    bool     isCharged;      // true = charged shot visual
};
```

**Behavior:**
- Client creates projectile entity with given parameters
- Client stores mapping: `projectileId` → local entity
- Client handles visual rendering based on `isCharged`

---

### 6.12 SPAWN_ENEMY (Type 15)

**Direction:** Server → All  
**Purpose:** Spawn an enemy entity  
**Reliability:** Important

**Payload Structure:**

```c
struct SpawnEnemyPacket {
    uint32_t enemyId;      // Server entity ID
    uint16_t enemyType;    // Enemy type (0=Basic, 1=Snake, 2=Suicide, 3=Boss)
    float    x, y;         // Spawn position
    uint16_t hp;           // Initial health
};
```

**Behavior:**
- Client creates enemy entity with appropriate sprite/behavior
- Client stores mapping: `enemyId` → local entity
- Enemy movement handled server-side or client-predicted

---

### 6.13 PLAYER_STATE (Type 7)

**Direction:** Server → All  
**Purpose:** Update player entity state  
**Reliability:** Unimportant (frequent updates)

**Payload Structure:**

```c
struct PlayerStatePacket {
    uint32_t playerId;        // Server entity ID
    float    x, y;            // Position
    float    dir;             // Rotation/direction
    uint16_t hp;              // Current health
    bool     isAlive;         // Alive state
    bool     invulnerable;    // Invulnerability state
};
```

**Behavior:**
- Sent periodically (~20Hz) for all players
- Client updates entity position and state
- Client may interpolate between updates

---

### 6.14 ENTITY_DESTROY (Type 8)

**Direction:** Server → All  
**Purpose:** Destroy an entity  
**Reliability:** Important

**Payload Structure:**

```c
struct EntityDestroyPacket {
    uint32_t entityId;   // Server entity ID to destroy
    uint16_t reason;     // 0=out of bounds, 1=killed, 2=disconnected
};
```

**Behavior:**
- Client removes entity from world
- Client may play death animation based on `reason`
- Client removes entity from ID mapping

---

### 6.15 PLAYER_DISCONNECT (Type 1)

**Direction:** Server → Client  
**Purpose:** Notify client of disconnection  
**Reliability:** Important

**Payload Structure:**

```c
struct PlayerDisconnectPacket {
    uint32_t playerId;   // Entity ID of disconnecting player
};
```

**Behavior:**
- Server sends to disconnecting client and room members
- Client cleans up player entity
- Client may return to main menu if own player

---

### 6.16 ROOM_ADMIN_UPDATE (Type 5)

**Direction:** Server → All  
**Purpose:** Notify admin change  
**Reliability:** Important

**Payload Structure:**

```c
struct RoomAdminUpdatePacket {
    uint32_t newAdminPlayerId;   // New admin's entity ID
};
```

**Behavior:**
- Sent when admin leaves and another player becomes admin
- Client updates UI to show admin status

---

### 6.17 SPAWN_BOSS_REQUEST (Type 16)

**Direction:** Client → Server  
**Purpose:** Admin requests boss spawn  
**Reliability:** Important

**Payload Structure:** Empty (header only)

**Behavior:**
- Only accepted from admin during game
- Server spawns boss enemy
- Server broadcasts `SPAWN_ENEMY` with boss type

---

### 6.18 PLAYER_SCORE_UPDATE (Type 17)

**Direction:** Server → Client  
**Purpose:** Update player score  
**Reliability:** Important

**Payload Structure:**

```c
struct PlayerScoreUpdatePacket {
    uint32_t playerId;   // Player entity ID
    int32_t  score;      // New absolute score value
};
```

**Behavior:**
- Sent when player scores points (enemy kill, etc.)
- Client updates score display

---

## 7. Connection Flow

### 7.1 Initial Connection

```
Client                                    Server
  |                                         |
  |--- JOIN_ROOM (name, joinCode) -------->|
  |                                         |
  |<-- JOIN_ROOM_ACCEPTED (roomCode) ------|
  |<-- PLAYER_JOIN (existing players) ------|
  |                                         |
```

### 7.2 Lobby Phase

```
Client                                    Server
  |                                         |
  |--- PLAYER_READY (true) ---------------->|
  |                                         |
  |<-- LOBBY_STATE (2/4 ready) ------------|
  |                                         |
  | [All players ready]                     |
  |                                         |
  |--- GAME_START_REQUEST ----------------->| (Admin only)
  |                                         |
  |<-- GAME_START --------------------------|
  |                                         |
```

### 7.3 Gameplay Loop

```
Client                                    Server
  |                                         |
  |--- PLAYER_INPUT (60Hz) ---------------->|
  |                                         |
  |<-- PLAYER_STATE (20Hz) ----------------|
  |<-- SPAWN_ENEMY -------------------------|
  |                                         |
  |--- PLAYER_SHOOT ----------------------->|
  |                                         |
  |<-- SPAWN_PROJECTILE --------------------|
  |                                         |
  |<-- ENTITY_DESTROY ----------------------|
  |                                         |
```

---

## 8. Client Implementation Guide

### 8.1 Prerequisites

**Required Libraries:**
- **zlib**: For packet compression/decompression
- **Network library**: Socket programming (BSD sockets, Winsock, etc.)
- **Game engine**: SFML, SDL, or custom renderer

### 8.2 Core Components

A basic R-Type client requires:

1. **PacketManager**: Handles packet serialization, compression, reliability
2. **NetworkManager**: Manages UDP socket, send/receive loops
3. **StateManager**: Manages game states (menu, lobby, game)
4. **EntityManager**: Manages game entities (players, enemies, projectiles)
5. **InputHandler**: Captures and sends player input

---

### 8.3 PacketManager Implementation

```cpp
class PacketManager {
private:
    uint32_t _send_seqid = 0;
    uint32_t _recv_seqid = 0;
    bool _compression_enabled = true;
    std::vector<packet_t> _history;
    std::vector<uint32_t> _missed_packets;
    
public:
    // Serialize packet to bytes
    std::vector<uint8_t> serializePacket(const packet_t& packet);
    
    // Deserialize bytes to packet
    packet_t deserializePacket(const uint8_t* data, size_t size);
    
    // Create packet with compression
    std::unique_ptr<uint8_t[]> createPacket(
        const void* data, 
        size_t data_size,
        uint8_t type,
        bool important
    );
    
    // Handle received packet
    void handleReceivedPacket(const uint8_t* data, size_t size);
    
    // Compress data using zlib
    std::vector<uint8_t> compressData(const void* data, size_t size);
    
    // Decompress data using zlib
    std::vector<uint8_t> decompressData(
        const void* data, 
        size_t compressed_size,
        size_t original_size
    );
};
```

**Key Implementation Details:**

```cpp
std::vector<uint8_t> PacketManager::compressData(const void* data, size_t size) {
    uLongf max_compressed = compressBound(size);
    std::vector<uint8_t> compressed(max_compressed);
    
    uLongf compressed_size = max_compressed;
    int result = compress2(
        compressed.data(),
        &compressed_size,
        static_cast<const Bytef*>(data),
        size,
        Z_BEST_SPEED
    );
    
    if (result == Z_OK && compressed_size < size) {
        compressed.resize(compressed_size);
        return compressed;
    }
    
    // Compression didn't help, return empty
    return std::vector<uint8_t>();
}

std::vector<uint8_t> PacketManager::decompressData(
    const void* data,
    size_t compressed_size,
    size_t original_size
) {
    std::vector<uint8_t> decompressed(original_size);
    
    uLongf decompressed_size = original_size;
    int result = uncompress(
        decompressed.data(),
        &decompressed_size,
        static_cast<const Bytef*>(data),
        compressed_size
    );
    
    if (result != Z_OK) {
        throw std::runtime_error("Decompression failed");
    }
    
    return decompressed;
}
```

---

### 8.4 NetworkManager Implementation

```cpp
class NetworkManager {
private:
    int _socket_fd;
    sockaddr_in _server_addr;
    PacketManager _packet_manager;
    std::thread _recv_thread;
    std::atomic<bool> _running;
    
public:
    // Connect to server
    bool connect(const std::string& ip, uint16_t port);
    
    // Send packet
    void sendPacket(const packet_t& packet);
    
    // Receive loop (runs in thread)
    void receiveLoop();
    
    // Get received packets
    std::vector<packet_t> getReceivedPackets();
    
    // Disconnect
    void disconnect();
};
```

**Example Implementation:**

```cpp
bool NetworkManager::connect(const std::string& ip, uint16_t port) {
    // Create UDP socket
    _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket_fd < 0) {
        return false;
    }
    
    // Setup server address
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &_server_addr.sin_addr);
    
    // Start receive thread
    _running = true;
    _recv_thread = std::thread(&NetworkManager::receiveLoop, this);
    
    return true;
}

void NetworkManager::sendPacket(const packet_t& packet) {
    auto serialized = _packet_manager.serializePacket(packet);
    
    sendto(
        _socket_fd,
        serialized.data(),
        serialized.size(),
        0,
        (sockaddr*)&_server_addr,
        sizeof(_server_addr)
    );
}

void NetworkManager::receiveLoop() {
    uint8_t buffer[2048];
    
    while (_running) {
        sockaddr_in sender_addr;
        socklen_t addr_len = sizeof(sender_addr);
        
        int n = recvfrom(
            _socket_fd,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&sender_addr,
            &addr_len
        );
        
        if (n > 0) {
            _packet_manager.handleReceivedPacket(buffer, n);
        }
    }
}
```

---

### 8.5 Game State Machine

```cpp
enum class GameState {
    MENU,
    LOBBY,
    GAME,
    GAME_OVER
};

class StateManager {
private:
    GameState _current_state;
    NetworkManager& _network;
    std::map<uint32_t, Entity> _entities;
    uint32_t _my_player_id;
    
public:
    void handlePacket(const packet_t& packet);
    void update(float deltaTime);
    void render();
};
```

---

### 8.6 Packet Handlers

```cpp
void StateManager::handlePacket(const packet_t& packet) {
    switch (packet.header.type) {
        case JOIN_ROOM_ACCEPTED: {
            auto* pkt = (JoinRoomAcceptedPacket*)packet.data;
            _my_player_id = pkt->playerServerId;
            _current_state = GameState::LOBBY;
            break;
        }
        
        case GAME_START: {
            _current_state = GameState::GAME;
            break;
        }
        
        case SPAWN_PROJECTILE: {
            auto* pkt = (SpawnProjectilePacket*)packet.data;
            createProjectile(
                pkt->projectileId,
                pkt->x, pkt->y,
                pkt->vx, pkt->vy,
                pkt->isCharged
            );
            break;
        }
        
        case SPAWN_ENEMY: {
            auto* pkt = (SpawnEnemyPacket*)packet.data;
            createEnemy(
                pkt->enemyId,
                pkt->enemyType,
                pkt->x, pkt->y,
                pkt->hp
            );
            break;
        }
        
        case PLAYER_STATE: {
            auto* pkt = (PlayerStatePacket*)packet.data;
            updatePlayerState(
                pkt->playerId,
                pkt->x, pkt->y,
                pkt->hp,
                pkt->isAlive
            );
            break;
        }
        
        case ENTITY_DESTROY: {
            auto* pkt = (EntityDestroyPacket*)packet.data;
            destroyEntity(pkt->entityId);
            break;
        }
    }
}
```

---

### 8.7 Input Handling

```cpp
void StateManager::update(float deltaTime) {
    if (_current_state != GameState::GAME) return;
    
    // Capture input
    bool moveUp = keyboard.isKeyPressed(Key::W);
    bool moveDown = keyboard.isKeyPressed(Key::S);
    bool moveLeft = keyboard.isKeyPressed(Key::A);
    bool moveRight = keyboard.isKeyPressed(Key::D);
    
    // Send input to server
    PlayerInputPacket input;
    input.moveUp = moveUp;
    input.moveDown = moveDown;
    input.moveLeft = moveLeft;
    input.moveRight = moveRight;
    input.clientX = _my_position.x;
    input.clientY = _my_position.y;
    
    packet_t packet;
    packet.header.type = PLAYER_INPUT;
    packet.data = &input;
    packet.header.data_size = sizeof(input);
    
    _network.sendPacket(packet);
    
    // Handle shooting
    if (keyboard.isKeyPressed(Key::Space)) {
        PlayerShootPacket shoot;
        shoot.isCharged = false;
        shoot.playerX = _my_position.x;
        shoot.playerY = _my_position.y;
        
        packet_t shootPacket;
        shootPacket.header.type = PLAYER_SHOOT;
        shootPacket.data = &shoot;
        shootPacket.header.data_size = sizeof(shoot);
        
        _network.sendPacket(shootPacket);
    }
}
```

---

### 8.8 Complete Minimal Client Example

```cpp
#include <iostream>
#include "NetworkManager.h"
#include "PacketManager.h"
#include "packets.h"

int main() {
    NetworkManager network;
    
    // Connect to server
    if (!network.connect("127.0.0.1", 8080)) {
        std::cerr << "Failed to connect" << std::endl;
        return 1;
    }
    
    // Join room
    JoinRoomPacket joinPkt;
    strncpy(joinPkt.name, "Player1", 32);
    joinPkt.joinCode = 1; // Join public room
    
    packet_t packet;
    packet.header.type = JOIN_ROOM;
    packet.header.seqid = 1;
    packet.data = &joinPkt;
    packet.header.data_size = sizeof(joinPkt);
    
    network.sendPacket(packet);
    
    // Main game loop
    bool running = true;
    while (running) {
        // Process received packets
        auto packets = network.getReceivedPackets();
        for (auto& pkt : packets) {
            handlePacket(pkt);
        }
        
        // Update game state
        update(0.016f); // 60 FPS
        
        // Render
        render();
    }
    
    network.disconnect();
    return 0;
}
```

---

## 9. Security Considerations

### 9.1 Authentication

- Current implementation uses placeholder `auth` field
- **Future**: Implement token-based authentication
- **Recommendation**: Use SSL/TLS for control channel

### 9.2 Validation

Servers MUST validate:
- Packet size limits (MAX_PACKET_SIZE = 2048 bytes)
- Player identity for privileged actions (shoot, admin commands)
- Input ranges (position, velocity bounds)
- Rate limiting (prevent spam)

### 9.3 Cheating Prevention

- **Server authoritative**: All game logic on server
- **Input validation**: Sanitize player inputs
- **Client prediction**: For smooth gameplay, not authority
- **Reconciliation**: Server corrects client predictions

---

## 10. Examples

### 10.1 Endianness Conversion

**Always convert multi-byte integers to network byte order before sending:**

```cpp
// Sending
pkt.playerId = htonl(localPlayerId);    // uint32_t
pkt.damage = htons(localDamage);        // uint16_t

// Receiving
uint32_t playerId = ntohl(pkt.playerId);
uint16_t damage = ntohs(pkt.damage);
```

### 10.2 Compression Example

```cpp
// Sending with compression
void sendPacket(const void* data, size_t size, uint8_t type) {
    packet_t packet;
    packet.header.type = type;
    packet.header.seqid = getNextSeqId();
    packet.header.original_size = 0;
    
    // Compress if beneficial
    if (size > 32) {
        auto compressed = compressData(data, size);
        if (compressed.size() < size) {
            packet.header.original_size = size;
            packet.header.data_size = compressed.size();
            packet.data = compressed.data();
        } else {
            packet.header.data_size = size;
            packet.data = const_cast<void*>(data);
        }
    } else {
        packet.header.data_size = size;
        packet.data = const_cast<void*>(data);
    }
    
    sendRawPacket(packet);
}

// Receiving with decompression
void handleReceivedPacket(const uint8_t* data, size_t size) {
    packet_t packet = deserializePacket(data, size);
    
    if (packet.header.original_size != 0) {
        // Decompress
        auto decompressed = decompressData(
            packet.data,
            packet.header.data_size,
            packet.header.original_size
        );
        
        // Use decompressed data
        processPacket(packet.header.type, decompressed.data(), decompressed.size());
    } else {
        // Use as-is
        processPacket(packet.header.type, packet.data, packet.header.data_size);
    }
}
```

### 10.3 Reliable Transmission Example

```cpp
// Track sent packets
std::map<uint32_t, packet_t> sentPackets;

// Send important packet
void sendReliablePacket(const packet_t& packet) {
    sentPackets[packet.header.seqid] = packet;
    sendRawPacket(packet);
}

// Handle ACK
void handleAck(uint32_t ackSeqId) {
    // Remove from history - successfully delivered
    sentPackets.erase(ackSeqId);
}

// Retransmit on timeout
void checkRetransmissions() {
    for (auto& [seqid, packet] : sentPackets) {
        if (shouldRetransmit(packet)) {
            sendRawPacket(packet);
        }
    }
}
```

---

## Appendix A: Quick Reference

### Packet Type Summary

| Type | Name | Direction | Important | Size |
|------|------|-----------|-----------|------|
| 1 | PLAYER_DISCONNECT | S→C | Yes | 4 bytes |
| 2 | JOIN_ROOM | C→S | Yes | 36 bytes |
| 3 | JOIN_ROOM_ACCEPTED | S→C | Yes | 9 bytes |
| 4 | GAME_START_REQUEST | C→S | Yes | 0 bytes |
| 5 | ROOM_ADMIN_UPDATE | S→All | Yes | 4 bytes |
| 6 | PLAYER_JOIN | S→C | Yes | 36 bytes |
| 7 | PLAYER_STATE | S→All | No | 23 bytes |
| 8 | ENTITY_DESTROY | S→All | Yes | 6 bytes |
| 9 | PLAYER_INPUT | C→S | No | 14 bytes |
| 10 | PLAYER_READY | C→S | Yes | 1 byte |
| 11 | LOBBY_STATE | S→All | No | 8 bytes |
| 12 | GAME_START | S→All | Yes | 0 bytes |
| 13 | PLAYER_SHOOT | C→S | Yes | 12 bytes |
| 14 | SPAWN_PROJECTILE | S→All | Yes | 27 bytes |
| 15 | SPAWN_ENEMY | S→All | Yes | 16 bytes |
| 16 | SPAWN_BOSS_REQUEST | C→S | Yes | 0 bytes |
| 17 | PLAYER_SCORE_UPDATE | S→C | Yes | 8 bytes |

---

## Appendix B: Glossary

- **Authoritative**: Server has final say on game state
- **Client Prediction**: Client simulates locally for responsiveness
- **Reconciliation**: Server corrects client predictions
- **Sequence ID**: Unique packet identifier for reliable delivery
- **ACK**: Acknowledgment packet confirming receipt
- **Entity**: Game object (player, enemy, projectile)
- **Room**: Game lobby/match instance
- **Admin**: Player who can control room settings

---

## Appendix C: References

- [RFC 768 - User Datagram Protocol](https://tools.ietf.org/html/rfc768)
- [zlib Compression Library](https://www.zlib.net/)
- [Game Networking Patterns](https://gafferongames.com/)
- [Fast-Paced Multiplayer](https://www.gabrielgambetta.com/client-server-game-architecture.html)

---

## Document History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2025-11-01 | Initial RFC draft |

---

**END OF DOCUMENT**

