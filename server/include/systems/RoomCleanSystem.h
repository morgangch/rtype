/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef ROOMCLEANSYSTEM_H
#define ROOMCLEANSYSTEM_H
#include "ECS/System.h"
#include "server/include/components/LinkedRoom.h"
#include "server/include/components/RoomProperties.h"
#include "server/include/services/PlayerService.h"

class RoomCleanSystem : public ECS::System {
public:
    RoomCleanSystem() : ECS::System("RoomCleanSystem", 10) {
    }

    /**
     * @brief Update cycle - cleans up empty rooms and linked entities
     *
     * For each room:
     * 1. Checks if there are any players in the room
     * 2. If no players are found, destroys the room entity and any linked entities
     *  @param world The ECS world containing all entities
     *  @param deltaTime Time elapsed since last update (unused)
     */
    void Update(ECS::World &world, float deltaTime) override {
        for (auto &pair: *world.GetAllComponents<rtype::server::components::RoomProperties>()) {
            rtype::server::components::RoomProperties *room = pair.second.get();
            if (!room)
                continue;
            auto players = rtype::server::services::player_service::findPlayersByRoom(pair.first);
            if (players.empty()) {
                auto linkedEntities = world.GetAllComponents<rtype::server::components::LinkedRoom>();
                for (auto &linkedPair: *linkedEntities) {
                    rtype::server::components::LinkedRoom *linkedRoom = linkedPair.second.get();
                    if (linkedRoom && linkedRoom->room_id == pair.first) {
                        world.DestroyEntity(linkedPair.first);
                    }
                    world.DestroyEntity(pair.first);
                }
            }
        }
    }
};


#endif //ROOMCLEANSYSTEM_H
