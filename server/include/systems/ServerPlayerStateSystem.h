/*
** ServerPlayerStateSystem: periodically broadcast PlayerStatePacket for each player
*/
#ifndef SERVER_PLAYER_STATE_SYSTEM_H
#define SERVER_PLAYER_STATE_SYSTEM_H

#include "ECS/System.h"

class ServerPlayerStateSystem : public ECS::System {
public:
    ServerPlayerStateSystem() : ECS::System("ServerPlayerStateSystem", 6), _tick(0.0f) {}
    void Update(ECS::World &world, float deltaTime) override;
private:
    float _tick;
    static constexpr float TICK_INTERVAL = 0.05f; // 20Hz
};

#endif // SERVER_PLAYER_STATE_SYSTEM_H
