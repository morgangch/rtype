#ifndef COMMON_NETWORK_MESSAGETYPE_HPP
#define COMMON_NETWORK_MESSAGETYPE_HPP
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
#endif // COMMON_NETWORK_MESSAGETYPE_HPP
