#pragma once
#include <cstdint>

enum class MessageType : uint8_t {
    NONE = 0,
    MOVE,
    SHOOT,
    HIT,
    SPAWN_ENEMY,
    PLAYER_JOIN,
    PLAYER_LEAVE,
    POWERUP,
    GAME_OVER
};
