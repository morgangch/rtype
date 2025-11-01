#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "controllers/RoomController.h"
#include "systems/PacketHandlingSystem.h"
#include "systems/ServerEnemySystem.h"
#include "systems/ServerCollisionSystem.h"
#include "systems/EnemyAISystem.h"
#include "systems/AssistantSystem.h"
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Projectile.h>
#include <common/components/Player.h>
#include <common/systems/MovementSystem.h>
#include <common/systems/FortressShieldSystem.h>
#include "components/LobbyState.h"
#include "components/PlayerConn.h"
#include "components/LinkedRoom.h"
#include "senders.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include "systems/AdminDetectorSystem.h"
#include <thread>

// Game bounds - must match client SCREEN_WIDTH and SCREEN_HEIGHT
constexpr float GAME_WIDTH = 1280.0f;
constexpr float GAME_HEIGHT = 720.0f;
constexpr float PLAYER_HALF_SIZE = 16.0f; // Half of player sprite size (32x32 / 2)

/**
 * @brief Clamp player position to game bounds
 * Ensures all players stay within the visible game area for consistent multiplayer state
 */
void clampPlayerPosition(rtype::common::components::Position* pos) {
    if (!pos) return;
    
    // Clamp X position (left-right bounds)
    if (pos->x < PLAYER_HALF_SIZE) {
        pos->x = PLAYER_HALF_SIZE;
    } else if (pos->x > GAME_WIDTH - PLAYER_HALF_SIZE) {
        pos->x = GAME_WIDTH - PLAYER_HALF_SIZE;
    }
    
    // Clamp Y position (top-bottom bounds)
    if (pos->y < PLAYER_HALF_SIZE) {
        pos->y = PLAYER_HALF_SIZE;
    } else if (pos->y > GAME_HEIGHT - PLAYER_HALF_SIZE) {
        pos->y = GAME_HEIGHT - PLAYER_HALF_SIZE;
    }
}
void net_loop() {
    while (true) {
        rtype::server::network::loop_recv(root.udp_server_fd);
        rtype::server::network::loop_send(root.udp_server_fd);
    }
}
void rtype::server::Rtype::loop(float deltaTime) {
    packetHandler.processPackets(packetManager.fetchReceivedPackets());

    rtype::common::systems::MovementSystem::update(world, deltaTime);
    
    // Update Fortress shield system (check turret status and disable boss shield if needed)
    rtype::common::systems::FortressShieldSystem::update(world, deltaTime);

    // Server-specific post-movement processing
    auto* positions = world.GetAllComponents<rtype::common::components::Position>();
    if (positions) {
        for (auto& [entity, posPtr] : *positions) {
            // Note: projectile distanceTraveled is now updated in MovementSystem (common)

            // Clamp player positions to game bounds (server-authoritative)
            auto* player = world.GetComponent<rtype::common::components::Player>(entity);
            if (player && posPtr) {
                clampPlayerPosition(posPtr.get());
            }

            // Update invulnerability timers for all entities with Health
            auto* health = world.GetComponent<rtype::common::components::Health>(entity);
            if (health && health->invulnerable) {
                health->invulnerabilityTimer -= deltaTime;
                if (health->invulnerabilityTimer <= 0.0f) {
                    health->invulnerable = false;
                    health->invulnerabilityTimer = 0.0f;
                }
            }
        }
    }

    world.UpdateSystems(deltaTime);
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(4242);
    // Run the network loop in a separate thread
    std::thread networkThread(net_loop);
    networkThread.detach();

    root.packetHandler.registerCallback(Packets::JOIN_ROOM, rtype::server::controllers::room_controller::handleJoinRoomPacket);
    root.packetHandler.registerCallback(Packets::GAME_START_REQUEST, rtype::server::controllers::room_controller::handleGameStartRequest);
    root.packetHandler.registerCallback(Packets::PLAYER_INPUT, rtype::server::controllers::room_controller::handlePlayerInput);
    root.packetHandler.registerCallback(Packets::PLAYER_READY, rtype::server::controllers::room_controller::handlePlayerReady);
    root.packetHandler.registerCallback(Packets::PLAYER_SHOOT, rtype::server::controllers::room_controller::handlePlayerShoot);
    root.packetHandler.registerCallback(Packets::LOBBY_SETTINGS_UPDATE, rtype::server::controllers::room_controller::handleLobbySettingsUpdate);
    std::cout << "✓ Registered PLAYER_READY callback (type " << static_cast<int>(Packets::PLAYER_READY) << ")" << std::endl;

    // Register systems
    root.world.RegisterSystem<PacketHandlingSystem>();
    root.world.RegisterSystem<ServerEnemySystem>();
    root.world.RegisterSystem<rtype::server::systems::ServerCollisionSystem>();
    root.world.RegisterSystem<rtype::server::systems::AssistantSystem>();
    std::cout << "✓ Registered AssistantSystem" << std::endl;
    std::cout << "✓ Registered ServerCollisionSystem" << std::endl;

    root.world.RegisterSystem<rtype::server::systems::EnemyAISystem>();
    auto* enemyAI = root.world.GetSystem<rtype::server::systems::EnemyAISystem>();
    if (enemyAI) {
        enemyAI->SetProjectileCallback([](ECS::EntityID shooter, float x, float y, float vx, float vy, ECS::World& world) {
            auto* shooterRoom = world.GetComponent<rtype::server::components::LinkedRoom>(shooter);
            if (!shooterRoom) return;

            auto projectile = rtype::server::controllers::room_controller::createEnemyProjectile(shooterRoom->room_id, x, y, vx, vy, world);

            auto* pos = world.GetComponent<rtype::common::components::Position>(projectile);
            auto* vel = world.GetComponent<rtype::common::components::Velocity>(projectile);
            auto* proj = world.GetComponent<rtype::common::components::Projectile>(projectile);

            if (pos && vel && proj) {
                rtype::server::network::senders::broadcast_projectile_spawn(
                    shooterRoom->room_id,
                    static_cast<uint32_t>(projectile),
                    0,
                    pos->x, pos->y,
                    vel->vx, vel->vy,
                    proj->damage,
                    proj->piercing,
                    false
                );
            }
        });
        std::cout << "✓ Registered EnemyAISystem with projectile broadcast" << std::endl;
    }

    root.world.RegisterSystem<AdminDetectorSystem>();

    auto lastTime = std::chrono::high_resolution_clock::now();
    constexpr float TARGET_DELTA = 1.0f / 60.0f;

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

        if (deltaTime < TARGET_DELTA) {
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((TARGET_DELTA - deltaTime) * 1000000)));
            currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        }

        lastTime = currentTime;
        r.loop(deltaTime);
    }
    return 0;
}
