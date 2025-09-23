/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>

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
