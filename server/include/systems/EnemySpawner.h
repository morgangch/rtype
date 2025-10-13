#ifndef SERVER_ENEMY_SPAWNER_HPP
#define SERVER_ENEMY_SPAWNER_HPP

#include "common/ecs/World.h"

class EnemySpawner {
public:
    EnemySpawner(World& world);

    void spawnWave(int count);

private:
    World& world;
};

#endif // SERVER_ENEMY_SPAWNER_HPP
