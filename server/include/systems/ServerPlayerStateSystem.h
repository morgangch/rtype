/**
 * @file ServerPlayerStateSystem.h
 * @brief System for broadcasting player state updates to clients
 *
 * This system runs at a fixed tick rate (20Hz) and broadcasts PLAYER_STATE
 * packets to all players in the same room, keeping clients synchronized
 * with the authoritative server state.
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

#ifndef SERVER_PLAYER_STATE_SYSTEM_H
#define SERVER_PLAYER_STATE_SYSTEM_H

#include "ECS/System.h"
#include "ECS/World.h"

/**
 * @brief Server-side player state broadcasting system
 *
 * Broadcasts player position, rotation, health, and alive status
 * to all players in the same room at 20Hz (50ms intervals).
 *
 * @note Priority: 6 (runs after gameplay systems)
 * @note Broadcast rate: 20Hz (50ms intervals)
 */
class ServerPlayerStateSystem : public ECS::System<ServerPlayerStateSystem> {
public:
    /**
     * @brief Update cycle - broadcasts player states at 20Hz
     *
     * For each active game room:
     * 1. Gathers position and health data for all players
     * 2. Builds PLAYER_STATE packets
     * 3. Broadcasts to all players in the same room
     *
     * @param world The ECS world containing all entities
     * @param deltaTime Time elapsed since last update (in seconds)
     */
    void Update(ECS::World &world, float deltaTime);

private:
    float _tick = 0.0f;                              ///< Accumulator for broadcast timing
    static constexpr float TICK_INTERVAL = 0.05f;   ///< Broadcast every 50ms (20Hz)
};

#endif // SERVER_PLAYER_STATE_SYSTEM_H
