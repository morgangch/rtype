#pragma once
#include "common/ecs/World.hpp"

class EnemySpawner {
public:
    EnemySpawner(World& world);

    void spawnWave(int count);

private:
    World& world;
};
