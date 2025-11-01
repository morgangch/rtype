/**
 * @file PlayerConn.h
 * @brief Network connection component for a player on the server
 *
 * This component is attached to the server-side player entity and stores
 * networking-related state needed to communicate with the client, including
 * a PacketManager for reliable packet sending and a PacketHandler for
 * per-player packet callbacks.
 *
 * It is intentionally lightweight and stores only data required for
 * networking and room bookkeeping.
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

#ifndef NETWORKADDRESS_H
#define NETWORKADDRESS_H

#include <string>
#include "ECS/Component.h"
#include "packetmanager.h"
#include "packethandler.h"

namespace rtype::server::components {
    /**
     * @brief Component storing per-player networking state
     *
     * Members:
     * - packet_manager: Responsible for reliable delivery and retransmission.
     * - packet_handler: Per-player packet callback registry (if needed).
     * - address / port: Remote address information for sending UDP packets.
     * - room_code: Current room the player is in (used for broadcasting messages).
     * - last_packet_timestamp: Last time a packet was received (used for timeouts).
     */
    class PlayerConn : public ECS::Component<PlayerConn> {
    public:
        /**
         * @brief Packet manager used to send reliable packets to this player
         */
        PacketManager packet_manager;

        /**
         * @brief Optional per-player packet handler (for special callbacks)
         */
        PacketHandler packet_handler;

        /**
         * @brief Remote IP address (text form) of the connected client
         */
        std::string address;

        /**
         * @brief Remote port of the connected client
         */
        int port;

        /**
         * @brief Room join code the player currently occupies
         */
        unsigned int room_code;

        /**
         * @brief Unix timestamp (ms) of the last received packet from the client
         *
         * Used by the server to detect timeouts and disconnect inactive players.
         */
        unsigned long last_packet_timestamp;

        /**
         * @brief Construct a new PlayerConn component
         * @param address Remote IP address (default: empty)
         * @param port Remote port (default: 0)
         * @param room_code Initial room join code (default: 0)
         */
        PlayerConn(std::string address = "", int port = 0, unsigned int room_code = 0)
            : address(address), port(port) , room_code(room_code) {
        };
    };
}

#endif //NETWORKADDRESS_H
