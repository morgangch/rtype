/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#ifndef PACKETS_H
#define PACKETS_H
#define MAX_PACKET_SIZE 2048
#include "ECS/ECS.h"

/**
 * Packet types
 * Linking a packet type with the packet type id.
 */

enum Packets {
    PLAYER_DISCONNECT = 1,
    JOIN_ROOM = 2,
    JOIN_ROOM_ACCEPTED = 3,
    GAME_START_REQUEST = 4,
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
 */
struct JoinRoomAcceptedPacket {
    uint32_t roomCode;
    bool admin;
};

/**
 * When the owner of a room is starting a game
 * Client → Server
 */
struct GameStartRequestPacket {
};


#endif //PACKETS_H
