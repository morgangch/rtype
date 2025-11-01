/**
 * @file RoomController.h
 * @brief Room-related packet handlers and helper functions for the server
 *
 * This header declares the public controller functions used to handle room
 * management packets (join, ready, start) and player actions related to the
 * room lifecycle. Implementations live under server/src/controllers.
 *
 * The controller functions accept a packet_t reference so they may extract
 * the player/entity context and payload from the incoming network packet.
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

#ifndef ROOMCONTROLLER_H
#define ROOMCONTROLLER_H
#include "packet.h"
#include "packethandler.h"
#include "ECS/Types.h"

namespace ECS {
    class World;
}

namespace rtype::server::controllers::room_controller {
    /**
     * @brief Handle a client's request to join a room
     * @param packet Incoming network packet containing join request data
     *
     * Expected behavior: validate join code, create player entity if needed,
     * and send JOIN_ROOM_ACCEPTED or JOIN_ROOM_REJECTED as appropriate.
     */
    void handleJoinRoomPacket(const packet_t& packet);

    /**
     * @brief Handle a client's request to start the game in a room
     * @param packet Incoming network packet containing start request
     *
     * Expected behavior: verify requester is room owner/admin and that all
     * players are ready, then transition the room to game state and notify players.
     */
    void handleGameStartRequest(const packet_t& packet);

    /**
     * @brief Handle player input packets forwarded from clients
     * @param packet Incoming PLAYER_INPUT packet
     *
     * Responsibilities: update server-side player position/velocity components
     * and integrate input into server-side simulation.
     */
    void handlePlayerInput(const packet_t& packet);

    /**
     * @brief Handle player ready/unready toggle in the lobby
     * @param packet Incoming PLAYER_READY packet
     *
     * Expected to update the player's LobbyState component and broadcast
     * the updated lobby state to all players in the room.
     */
    void handlePlayerReady(const packet_t& packet);

    /**
     * @brief Handle player shoot requests (local or network-initiated)
     * @param packet Incoming PLAYER_SHOOT packet
     *
     * May spawn a server-owned projectile entity and broadcast it to players.
     */
    void handlePlayerShoot(const packet_t& packet);

    /**
     * @brief Handle admin request to spawn a boss in the room
     * @param packet Incoming SPAWN_BOSS_REQUEST packet
     *
     * Validates that the requester is the room admin and that no boss
     * currently exists in the room before spawning.
     */
    void handleSpawnBossRequest(const packet_t& packet);

    /**
     * @brief Handle admin lobby settings updates (difficulty, toggles)
     * @param packet Incoming LOBBY_SETTINGS_UPDATE packet
     */
    void handleLobbySettingsUpdate(const packet_t& packet);

    /**
     * @brief Handle lobby settings updates from the admin client
     * @param packet Incoming LOBBY_SETTINGS_UPDATE packet
     */
    void handleLobbySettingsUpdate(const packet_t& packet);

    /**
     * @brief Broadcast the current lobby state to all players in a room
     * @param room Room entity ID representing the room
     *
     * Used to keep client-side lobby UIs synchronized (player ready flags, etc.).
     */
    void broadcastLobbyState(ECS::EntityID room);

    /**
     * @brief Register packet callbacks on a PacketHandler for player-related packets
     * @param handler PacketHandler to register callbacks on
     *
     * This sets up the mapping between incoming packet IDs and controller functions.
     */
    void registerPlayerCallbacks(PacketHandler& handler);
    
    // === Helper Functions ===
    
    /**
     * @brief Mark all players in a room as in-game
     * @param room The room entity ID
     */
    void markPlayersAsInGame(ECS::EntityID room);
    
    /**
     * @brief Send GAME_START packets to all players in a room
     * @param room The room entity ID
     */
    void broadcastGameStart(ECS::EntityID room);
    
    /**
     * @brief Send PLAYER_JOIN packets for all players to each other
     * @param room The room entity ID
     */
    void broadcastPlayerRoster(ECS::EntityID room);
    
    /**
     * @brief Create a projectile entity on the server
     * @param room The room entity ID
     * @param x Starting X position
     * @param y Starting Y position
     * @param isCharged Whether this is a charged shot
     * @return The created projectile entity ID
     */
    ECS::EntityID createServerProjectile(ECS::EntityID room, ECS::EntityID owner, float x, float y, bool isCharged);
    
    /**
     * @brief Create an enemy projectile with custom velocity
     * @param room The room entity ID
     * @param x Starting X position
     * @param y Starting Y position
     * @param vx Velocity X
     * @param vy Velocity Y
     * @param world The ECS world
     * @return The created projectile entity ID
     */
    ECS::EntityID createEnemyProjectile(ECS::EntityID room, float x, float y, float vx, float vy, ECS::World& world);

    /**
     * @brief Broadcast projectile spawn to all players in a room
     * @param projectile The projectile entity ID
     * @param owner The owner player entity ID
     * @param room The room entity ID
     * @param isCharged Whether this is a charged shot
     */
    void broadcastProjectileSpawn(ECS::EntityID projectile, ECS::EntityID owner, ECS::EntityID room, bool isCharged);
    
    // === Join Room Helper Functions ===
    
    /**
     * @brief Find or create a player entity for a join request
     * @param packet The join room packet
     * @param playerName The player's name
     * @param joinCode The join code from the packet
     * @param ipStr The player's IP address as string
     * @param port The player's port
     * @return The player entity ID, or 0 if creation failed
     */
    ECS::EntityID findOrCreatePlayer(const packet_t& packet, const std::string& playerName, uint32_t joinCode, const std::string& ipStr, int port);
    
    /**
     * @brief Find or create a room based on join code
     * @param joinCode The join code (0 = new private, 1 = public matchmaking, other = specific room)
     * @param player The player entity ID requesting to join
     * @return The room entity ID, or 0 if not found/created
     */
    ECS::EntityID findOrCreateRoom(uint32_t joinCode, ECS::EntityID player);
    
    /**
     * @brief Send join acceptance packet to player
     * @param player The player entity ID
     * @param room The room entity ID
     */
    void sendJoinAccepted(ECS::EntityID player, ECS::EntityID room);
    
    /**
     * @brief Notify joining player about existing players in room
     * @param player The joining player entity ID
     * @param room The room entity ID
     */
    void notifyJoiningPlayerOfExisting(ECS::EntityID player, ECS::EntityID room);
    
    /**
     * @brief Notify existing players about the new player joining
     * @param player The new player entity ID
     * @param playerName The new player's name
     * @param room The room entity ID
     */
    void notifyExistingPlayersOfNewJoin(ECS::EntityID player, const std::string& playerName, ECS::EntityID room);
    
    /**
     * @brief Initialize lobby state for a player if they don't have one
     * @param player The player entity ID
     */
    void initializeLobbyState(ECS::EntityID player);
}

#endif //ROOMCONTROLLER_H
