/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "services/PlayerService.h"
#include "rtype.h"
#include <string>
#include <cstring>
//TODO: optimize the following import.
#include <common/components/Player.h>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/Score.h>
#include <common/components/VesselClass.h>
#include "components/PlayerConn.h"
#include "controllers/RoomController.h"
#include "packets.h"
#include "components/LinkedRoom.h"
#include "components/RoomProperties.h"
#include "services/RoomService.h"

using namespace rtype::server::services;

ECS::EntityID player_service::createNewPlayer(std::string name, int room_code, std::string ip, int port, 
                                               rtype::common::components::VesselType vesselType) {
    auto player = root.world.CreateEntity();

    // Create VesselClass component with selected type
    auto vesselClass = rtype::common::components::VesselClass(vesselType);
    root.world.AddComponent<rtype::common::components::VesselClass>(player, vesselClass);

    // Player component with vessel type
    root.world.AddComponent<rtype::common::components::Player>(player, name, player, vesselType);
    
    // Basic player components so server can track and broadcast state
    root.world.AddComponent<rtype::common::components::Position>(player, 100.0f, 360.0f, 0.0f);
    
    // Velocity with modified max speed based on vessel stats (base 400 px/s)
    float baseSpeed = 400.0f;
    float effectiveSpeed = vesselClass.getEffectiveSpeed(baseSpeed);
    root.world.AddComponent<rtype::common::components::Velocity>(player, 0.0f, 0.0f, effectiveSpeed);
    
    // Health with modified max HP based on vessel stats (base 100 HP for server tracking)
    int baseHealth = 100;
    int effectiveHealth = vesselClass.getEffectiveMaxHealth(baseHealth);
    root.world.AddComponent<rtype::common::components::Health>(player, effectiveHealth);
    
    root.world.AddComponent<rtype::common::components::Team>(player, rtype::common::components::TeamType::Player);
    root.world.AddComponent<rtype::common::components::Score>(player, 0, 0, 0); // Initialize score to 0
    root.world.AddComponent<rtype::server::components::PlayerConn>(player, ip, port, room_code);

    // CRITICAL: Register packet callbacks on the player's packet_handler
    // Without this, the player's PacketHandlingSystem won't route packets to handlers!
    auto *playerConn = root.world.GetComponent<rtype::server::components::PlayerConn>(player);
    if (playerConn) {
        // Register all packet callbacks on this player's packet_handler
        rtype::server::controllers::room_controller::registerPlayerCallbacks(playerConn->packet_handler);

        // Note: JOIN_ROOM_ACCEPTED is sent by handleJoinRoomPacket(), not here
        // This ensures correct room code and admin status are sent
    }

    return player;
}

ECS::EntityID player_service::findPlayerByNetwork(const std::string &ip, int port) {
    auto players = root.world.GetAllComponents<rtype::server::components::PlayerConn>();

    for (const auto &pair: *players) {
        auto *addr = root.world.GetComponent<rtype::server::components::PlayerConn>(pair.first);
        if (addr && addr->address == ip && addr->port == port) {
            return pair.first;
        }
    }
    return 0;
}

ECS::EntityID player_service::findPlayerByNetwork(const uint8_t *uint8_t, uint16_t port) {
    std::string ip = std::to_string(uint8_t[0]) + "." + std::to_string(uint8_t[1]) + "." +
                     std::to_string(uint8_t[2]) + "." + std::to_string(uint8_t[3]);
    return findPlayerByNetwork(ip, port);
}

ECS::EntityID player_service::findPlayerByNetwork(const sockaddr_in &addr) {
    return findPlayerByNetwork((const uint8_t *) &addr.sin_addr, ntohs(addr.sin_port));
}

std::vector<ECS::EntityID> player_service::findPlayersByRoomCode(int room_code) {
    std::vector<ECS::EntityID> players_in_room;

    auto room = room_service::getRoomByJoinCode(room_code);
    if (!room) {
        std::cout << "WARNING: No room found with join code " << room_code << std::endl;
        return players_in_room;
    }
    auto players = player_service::findPlayersByRoom(room);

    return players;
}

std::vector<ECS::EntityID> player_service::findPlayersByRoom(ECS::EntityID room) {
    std::vector<ECS::EntityID> players_in_room;
    auto players = root.world.GetAllComponents<rtype::server::components::PlayerConn>();

    for (const auto &pair: *players) {
        auto *linkedRoom = root.world.GetComponent<rtype::server::components::LinkedRoom>(pair.first);
        if (linkedRoom && linkedRoom->room_id == room) {
            players_in_room.push_back(pair.first);
        }
    }
    return players_in_room;
}
