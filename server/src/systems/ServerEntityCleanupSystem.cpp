/*
** ServerEntityCleanupSystem implementation
*/
#include "systems/ServerEntityCleanupSystem.h"
#include "rtype.h"
#include "packets.h"
#include <common/components/Health.h>
#include <common/components/Player.h>
#include "components/PlayerConn.h"

void ServerEntityCleanupSystem::Update(ECS::World &world, float deltaTime) {
    // Find entities with Health and check for dead
    auto healths = root.world.GetAllComponents<rtype::common::components::Health>();
    if (!healths) return;

    std::vector<ECS::EntityID> toDestroy;
    for (auto &pair : *healths) {
        ECS::EntityID eid = pair.first;
        auto* h = pair.second.get();
        if (!h) continue;
        if (!h->isAlive || h->currentHp <= 0) {
            // Broadcast EntityDestroyPacket to players in the same room as the entity owner (if applicable)
            EntityDestroyPacket pkt{};
            pkt.entityId = static_cast<uint32_t>(eid);
            pkt.reason = 1; // killed

            // Send to all players (simple approach) — alternatively could restrict by room
            auto allPlayers = root.world.GetAllComponents<rtype::common::components::Player>();
            if (allPlayers) {
                for (auto &pp : *allPlayers) {
                    ECS::EntityID pid = pp.first;
                    auto* pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
                    if (!pconn) continue;
                    pconn->packet_manager.sendPacketBytesSafe(&pkt, sizeof(pkt), ENTITY_DESTROY, nullptr, true);
                }
            }

            toDestroy.push_back(eid);
        }
    }

    for (auto e : toDestroy) {
        root.world.DestroyEntity(e);
    }
}
