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
        // Collect rooms with zero players first (avoid destroying while iterating containers)
        std::vector<ECS::EntityID> rooms_to_destroy;

        if (auto rooms = world.GetAllComponents<rtype::server::components::RoomProperties>()) {
            for (auto &pair : *rooms) {
                auto *room = pair.second.get();
                if (!room)
                    continue;
                auto players = rtype::server::services::player_service::findPlayersByRoom(pair.first);
                if (players.empty()) {
                    rooms_to_destroy.push_back(pair.first);
                }
            }
        }

        if (rooms_to_destroy.empty())
            return;

        // For each empty room, destroy its linked entities first, then the room entity itself
        for (auto room_id : rooms_to_destroy) {
            std::vector<ECS::EntityID> linked_to_destroy;

            if (auto linkedEntities = world.GetAllComponents<rtype::server::components::LinkedRoom>()) {
                for (auto &linkedPair : *linkedEntities) {
                    auto *lr = linkedPair.second.get();
                    if (lr && lr->room_id == room_id) {
                        linked_to_destroy.push_back(linkedPair.first);
                    }
                }
            }

            // Destroy linked entities
            for (auto eid : linked_to_destroy) {
                world.DestroyEntity(eid);
            }

            // Finally destroy the room entity
            world.DestroyEntity(room_id);
        }
    }
};


#endif //ROOMCLEANSYSTEM_H
