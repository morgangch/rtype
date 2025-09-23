#pragma once
#include <vector>
#include "common/ecs/World.hpp"
#include "common/core/Event.hpp"

class GameRules {
public:
    explicit GameRules(bool friendlyFire = false);

    void update(World& world);

    bool isFriendlyFireEnabled() const;

    const std::vector<Event>& getEvents() const;
    void clearEvents();

private:
    bool friendlyFire;
    std::vector<Event> events;
};
