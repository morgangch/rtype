#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "controllers/RoomController.h"
#include "systems/PacketHandlingSystem.h"
#include "systems/ServerEnemySystem.h"
#include "systems/ServerPlayerStateSystem.h"
#include "systems/ServerEntityCleanupSystem.h"
#include "systems/ServerCollisionSystem.h"
#include "../../common/components/Position.h"
#include "../../common/components/Velocity.h"
#include "../../common/components/Projectile.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include "systems/AdminDetectorSystem.h"

void rtype::server::Rtype::loop(float deltaTime) {
    network::loop_recv(udp_server_fd);
    network::loop_send(udp_server_fd);
    packetHandler.processPackets(packetManager.fetchReceivedPackets());
    
    // Simple server-side movement system (update positions based on velocity)
    auto* positions = world.GetAllComponents<rtype::common::components::Position>();
    if (positions) {
        for (auto& [entity, posPtr] : *positions) {
            auto* vel = world.GetComponent<rtype::common::components::Velocity>(entity);
            if (!vel) continue;
            
            // Calculate distance moved this frame
            float dx = vel->vx * deltaTime;
            float dy = vel->vy * deltaTime;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // Update position
            posPtr->x += dx;
            posPtr->y += dy;
            
            // Update projectile distance traveled (for collision skip logic)
            auto* proj = world.GetComponent<rtype::common::components::Projectile>(entity);
            if (proj) {
                proj->distanceTraveled += distance;
            }
        }
    }
    
    world.UpdateSystems(deltaTime);
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(4242);

    root.packetHandler.registerCallback(Packets::JOIN_ROOM, rtype::server::controllers::room_controller::handleJoinRoomPacket);
    root.packetHandler.registerCallback(Packets::GAME_START_REQUEST, rtype::server::controllers::room_controller::handleGameStartRequest);
    root.packetHandler.registerCallback(Packets::PLAYER_INPUT, rtype::server::controllers::room_controller::handlePlayerInput);
    root.packetHandler.registerCallback(Packets::PLAYER_READY, rtype::server::controllers::room_controller::handlePlayerReady);
    std::cout << "✓ Registered PLAYER_READY callback (type " << static_cast<int>(Packets::PLAYER_READY) << ")" << std::endl;
    root.world.RegisterSystem<PacketHandlingSystem>();
    root.world.RegisterSystem<ServerEnemySystem>();
    root.world.RegisterSystem<rtype::server::systems::ServerCollisionSystem>();
    std::cout << "✓ Registered ServerCollisionSystem" << std::endl;
    root.world.RegisterSystem<AdminDetectorSystem>();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        r.loop(deltaTime);
    }
    return 0;
}
