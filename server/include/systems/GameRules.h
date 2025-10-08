#ifndef SERVER_GAME_RULES_HPP
#define SERVER_GAME_RULES_HPP

#include <vector>
#include "common/ecs/World.h"
#include "common/core/Event.h"

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

#endif // SERVER_GAME_RULES_HPP
