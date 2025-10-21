/*
** Minimal server-side enemy spawner system
*/
#ifndef SERVER_ENEMY_SYSTEM_H
#define SERVER_ENEMY_SYSTEM_H

#include "ECS/System.h"
#include "rtype.h"

class ServerEnemySystem : public ECS::System {
public:
    ServerEnemySystem() : ECS::System("ServerEnemySystem", 5), _spawnTimer(0.0f), _stateTick(0.0f) {}

    void Update(ECS::World &world, float deltaTime) override;

private:
    float _spawnTimer;
    static constexpr float SPAWN_INTERVAL = 2.0f; // seconds

    // player state broadcast tick
    float _stateTick;
    static constexpr float STATE_TICK_INTERVAL = 0.05f; // 20Hz
};

#endif // SERVER_ENEMY_SYSTEM_H
