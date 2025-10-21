/*
** ServerPlayerStateSystem implementation
*/
#include "systems/ServerPlayerStateSystem.h"
#include "rtype.h"
#include "packets.h"
#include "../../../common/components/Player.h"
#include "../../../common/components/Position.h"
#include "../../../common/components/Health.h"
#include "components/PlayerConn.h"

void ServerPlayerStateSystem::Update(ECS::World &world, float deltaTime) {
    _tick += deltaTime;
    if (_tick < TICK_INTERVAL) return;
    _tick = 0.0f;

    auto players = root.world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;

    for (auto &pair : *players) {
        ECS::EntityID pid = pair.first;
        auto* pos = root.world.GetComponent<rtype::common::components::Position>(pid);
        auto* health = root.world.GetComponent<rtype::common::components::Health>(pid);
        auto* pconn = root.world.GetComponent<rtype::server::components::PlayerConn>(pid);
        if (!pconn) continue; // player not networked

        PlayerStatePacket s{};
        s.playerId = static_cast<uint32_t>(pid);
        s.x = pos ? pos->x : 0.0f;
        s.y = pos ? pos->y : 0.0f;
        s.dir = pos ? pos->rotation : 0.0f;
        s.hp = health ? static_cast<uint16_t>(health->currentHp) : 0;
        s.isAlive = health ? health->isAlive : false;

        // Send this player's state to everyone in the same room
        auto allPlayers = root.world.GetAllComponents<rtype::common::components::Player>();
        if (!allPlayers) continue;
        for (auto &pp : *allPlayers) {
            ECS::EntityID other = pp.first;
            auto* otherConn = root.world.GetComponent<rtype::server::components::PlayerConn>(other);
            if (!otherConn) continue;
            if (otherConn->room_code != pconn->room_code) continue;
            // don't need to skip sender; sending to self is fine (client may ignore own player updates)
            otherConn->packet_manager.sendPacketBytesSafe(&s, sizeof(s), /*packet type*/ 0 /* client code should register PlayerStatePacket handler */, nullptr, true);
        }
    }
}
