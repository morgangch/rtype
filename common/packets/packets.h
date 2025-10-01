/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#ifndef PACKETS_H
#define PACKETS_H
#define MAX_PACKET_SIZE 2048

/**
 * Packet types
 * Linking a packet type with the packet type id.
 */

enum Packets {
    JOIN_ROOM = 2,
    PING = 3,
    PONG = 4,
    PLAYER_JOIN = 5,
    PLAYER_LEAVE = 6,
    PLAYER_INPUT = 7,
    PLAYER_SHOOT = 8,
    PLAYER_STATE = 9,
    SPAWN_ENEMY = 10,
    ENEMY_STATE = 11,
    MISSILE_SPAWN = 12,
};


/**
 * When a player want to join a room
 * Client → Server
 * Packet type 2
 * @param name Player name (max 32 bytes)
 * @param joinCode Room ID to join. Use 0 to create a new room, 1 to join a public room.
 */
struct JoinRoomPacket {
    char name[32];
    uint32_t joinCode;
};

/**
 * When the server accepts a player to join a room
 * Server → Client
 * Packet type 3
 * @param roomCode The ID of the room the player joined
 * @param admin If the user is an admin, the value is true.
 */
struct JoinRoomAcceptedPacket {
    uint32_t roomCode;
    bool admin;
};

/**
 * When the owner of a room is starting a game
 * Client → Server
 * Packet type 4
 */
struct GameStartRequestPacket {
};

/**
 * When the game is starting
 * Server → All clients in the room
 */
struct GameStartPacket {
};

/**
 * To disconnect a player.
 * If the player is on the game, it will leave after the reception of this packet.
 * Server → Client
 */
struct PlayerDisconnectPacket {
    ECS::EntityID playerId;
};


struct PingPacket {
};

struct PongPacket {
};

struct PlayerJoinPacket {
    uint32_t newPlayerId;
    char name[32];
};

// Serveur → All
struct PlayerLeavePacket {
    uint32_t leftPlayerId;
};

// Client → Server: Inform the server about player input
struct PlayerInputPacket {
    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;
};

// Player → Server: Player shoot
struct PlayerShootPacket {
    uint32_t playerId;
    uint16_t weaponType; // type d'arme (1 = tir simple, 2 = tir chargé)
};

// Server → All: Update player state
struct PlayerStatePacket {
    uint32_t playerId;
    float x, y;
    float dir;
    uint16_t hp;
    bool isAlive;
};

// Server → All: Spawn a new enemy
struct SpawnEnemyPacket {
    uint32_t enemyId;
    uint16_t enemyType; // type de monstre
    float x, y;
    uint16_t hp;
};

// Server → All: Update enemy state
struct EnemyStatePacket {
    uint32_t enemyId;
    float x, y;
    uint16_t hp;
};


// Server → All: Spawn a new missile
struct MissileSpawnPacket {
    uint32_t missileId;
    uint32_t ownerId; // joueur ou ennemi
    float x, y;
    float dir;
    uint16_t damage;
};

// Server → All: Update missile state
struct MissileStatePacket {
    uint32_t missileId;
    float x, y;
    float dir;
};

// Server → All: Destroy an entity (player, enemy, missile)
struct EntityDestroyPacket {
    uint32_t entityId;
    uint16_t reason; // 0 = out of bounds, 1 = killed, 2 = disconnected
};


#endif //PACKETS_H
