/*
** ServerEntityCleanupSystem implementation
*/
#include "rtype.h"
#include "packets.h"
#include <common/components/Health.h>
#include <common/components/Player.h>

#include "senders.h"
#include "components/LinkedRoom.h"
#include "components/PlayerConn.h"

namespace rtype::server::systems {

class ServerEntityCleanupSystem {
public:
    static void Update(ECS::World &world, float deltaTime) {
    // Find entities with Health and check for dead
    auto healths = root.world.GetAllComponents<rtype::common::components::Health>();
    if (!healths) return;

    std::vector<ECS::EntityID> toDestroy;
    for (auto &pair: *healths) {
        ECS::EntityID eid = pair.first;
        auto *h = pair.second.get();
        if (!h) continue;
        if (!h->isAlive || h->currentHp <= 0) {
            auto *room = world.GetComponent<rtype::server::components::LinkedRoom>(eid);
            if (room) {
                // Broadcast EntityDestroyPacket to players in the same room as the entity owner (if applicable)
                rtype::server::network::senders::broadcast_entity_destroy(room->room_id, eid, 1); // reason 1 = death
            }
            toDestroy.push_back(eid);
        }
    }

    for (auto e: toDestroy) {
        root.world.DestroyEntity(e);
    }
    }
};

} // namespace rtype::server::systems
