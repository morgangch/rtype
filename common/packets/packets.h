/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#ifndef PACKETS_H
#define PACKETS_H
#define MAX_PACKET_SIZE 2048

#include <cstdint>
#include <cstring>

/**
 * Packet types
 * Linking a packet type with the packet type id.
 */

enum Packets {
    PLAYER_DISCONNECT = 1,
    JOIN_ROOM = 2,
    JOIN_ROOM_ACCEPTED = 3,
    GAME_START_REQUEST = 4,
    ROOM_ADMIN_UPDATE = 5,
    PLAYER_JOIN = 6,
    PLAYER_STATE = 7,
    ENTITY_DESTROY = 8,
    PLAYER_INPUT = 9,
    PLAYER_READY = 10,
    LOBBY_STATE = 11,
    GAME_START = 12,
    PLAYER_SHOOT = 13,
    SPAWN_PROJECTILE = 14,
    SPAWN_ENEMY = 15,
    SPAWN_BOSS_REQUEST = 16,
    PLAYER_SCORE_UPDATE = 17,
    LOBBY_SETTINGS_UPDATE = 18,
    ALL_PLAYERS_STATE = 19, // Optimized: single packet for all players
};


/**
 * When a player want to join a room
 * Client → Server
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
 * @param roomCode The ID of the room the player joined
 * @param admin If the user is an admin, the value is true.
 * @param playerServerId The server-side entity ID for this player
 */
struct JoinRoomAcceptedPacket {
    uint32_t roomCode;
    bool admin;
    uint32_t playerServerId; // NEW: server entity ID for this player
};

/**
 * When the owner of a room is starting a game
 * Client → Server
 */
struct GameStartRequestPacket {
};

/**
 * When the game is starting for all players in a room
 * Server → All clients in room
 */
struct GameStartPacket {
    uint8_t startLevel;   // starting level index: 0=Lvl1, 1=Lvl2
};

/**
 * To disconnect a player.
 * If the player is on the game, it will leave after the reception of this packet.
 * Server → Client
 */
struct PlayerDisconnectPacket {
    unsigned int playerId;
};


/**
 * To inform clients about room admin changes
 */
struct RoomAdminUpdatePacket {
    uint32_t newAdminPlayerId;
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
    float clientX;  // Client's predicted position X (for server validation)
    float clientY;  // Client's predicted position Y (for server validation)
};

// Client → Server: Player shoot
struct PlayerShootPacket {
    bool isCharged; // true if it's a charged shot, false for regular
    float playerX;  // Player's X position at time of shooting
    float playerY;  // Player's Y position at time of shooting
};

// Server → All: Spawn a projectile
struct SpawnProjectilePacket {
    uint32_t projectileId;  // Server entity ID
    uint32_t ownerId;       // Player who shot it
    float x, y;             // Spawn position
    float vx, vy;           // Velocity
    uint16_t damage;
    bool piercing;
    bool isCharged;         // true if charged shot, false for normal
};

// Player → Server: Player shoot (OLD - keeping for compatibility)
struct OldPlayerShootPacket {
    uint32_t playerId;
    uint16_t weaponType; // weapon type (1 = simple shot, 2 = charged shot)
};

// Server → All: Update player state
struct PlayerStatePacket {
    uint32_t playerId;
    float x, y;
    float dir;
    uint16_t hp;
    bool isAlive;
    bool invulnerable; // Server-authoritative invulnerability state
};

// Server → All: Update ALL players state in one packet (OPTIMIZED)
// This replaces sending individual PlayerStatePacket for each player
// Reduces network overhead from O(N²) to O(N) broadcasts
struct AllPlayersStatePacket {
    uint8_t playerCount; // Number of players in this packet (max 4)
    struct PlayerData {
        uint32_t playerId;
        float x, y;
        float dir;
        uint16_t hp;
        bool isAlive;
        bool invulnerable;
    } players[4]; // Support up to 4 players
};

// Server → All: Spawn a new enemy
struct SpawnEnemyPacket {
    uint32_t enemyId;
    uint16_t enemyType; // enemy type
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

// Client → Server: Player toggles ready state in lobby
struct PlayerReadyPacket {
    bool isReady;
};

// Server → All: Update lobby state (player count, ready count)
struct LobbyStatePacket {
    uint32_t totalPlayers;
    uint32_t readyPlayers;
};

// Client → Server: Admin requests boss spawn (B key pressed)
struct SpawnBossRequestPacket {
    // Empty packet - admin identity verified by server through connection
};

/**
 * Server → Client: Update player's in-game score (server authoritative)
 */
struct PlayerScoreUpdatePacket {
    uint32_t playerId; // server entity ID for the player
    int32_t score;     // new absolute score value
};

/**
 * Client (Admin) → Server: Update lobby settings for the room
 * These settings are applied server-side on game start and during gameplay as appropriate.
 * Currently used fields:
 *  - aiAssist: whether to spawn an AI assistant when only one human player
 *  - godMode: whether the admin should have 1000 HP
 *  - megaDamage: whether the admin's projectiles should deal 1000 damage
 * Cosmetic fields are included for future use:
 *  - difficulty, friendlyFire
 */
struct LobbySettingsUpdatePacket {
    uint8_t difficulty;   // 0 = Easy, 1 = Normal, 2 = Hard
    bool friendlyFire;    // cosmetic for now
    bool aiAssist;        // functional
    bool megaDamage;      // functional
    uint8_t startLevel;   // debug start level: 0=Lvl1, 1=Lvl2
};

#endif //PACKETS_H
