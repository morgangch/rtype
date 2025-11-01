/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Main R-Type server class and global instance
*/
#ifndef RTYPE_H
#define RTYPE_H
#include "packethandler.h"
#include "packetmanager.h"
#include "ECS/World.h"

namespace rtype::server {
    /**
     * @brief Main R-Type server instance
     * 
     * Central server class that manages:
     * - Global packet management (broadcast packets)
     * - Global packet handling (callbacks)
     * - ECS world (all entities: players, enemies, projectiles, rooms)
     * - UDP socket file descriptor
     * 
     * The server uses an Entity Component System (ECS) architecture
     * where all game objects are entities with attached components.
     */
    class Rtype {
    private:

    public:
        PacketManager packetManager;    ///< Global packet manager for broadcast packets
        PacketHandler packetHandler;    ///< Global packet handler with registered callbacks
        ECS::World world;               ///< ECS world containing all game entities

        int udp_server_fd;              ///< File descriptor for UDP socket
        
        /**
         * @brief Main server update loop
         * 
         * Called each frame to:
         * 1. Process incoming packets
         * 2. Update all ECS systems
         * 3. Send outgoing packets
         * 
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void loop(float deltaTime);
    };
};

/**
 * @brief Global instance of the R-Type server
 * 
 * This global is accessible throughout the server codebase for
 * quick access to the ECS world, packet managers, and handlers.
 * 
 * @warning Use with care - global state can make testing difficult
 */
inline rtype::server::Rtype root;

#endif //RTYPE_H
