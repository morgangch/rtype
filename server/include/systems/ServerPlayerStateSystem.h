/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerPlayerStateSystem - Periodically broadcasts player states to all clients
*/
#ifndef SERVER_PLAYER_STATE_SYSTEM_H
#define SERVER_PLAYER_STATE_SYSTEM_H

#include "ECS/System.h"

/**
 * @brief System that broadcasts player position and health at 20Hz
 * 
 * Ensures all clients stay synchronized with server-side player states
 * by periodically broadcasting PLAYER_STATE packets containing:
 * - Player position (x, y)
 * - Player rotation/direction
 * - Player health (HP)
 * - Player alive status
 * 
 * Only broadcasts for players in rooms where the game has started.
 * 
 * @note Priority: 6 (runs after gameplay systems)
 * @note Broadcast rate: 20Hz (50ms intervals)
 */
class ServerPlayerStateSystem : public ECS::System {
public:
    ServerPlayerStateSystem() : ECS::System("ServerPlayerStateSystem", 6), _tick(0.0f) {}
    
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
    void Update(ECS::World &world, float deltaTime) override;
    
private:
    float _tick;                                       ///< Accumulator for broadcast timing
    static constexpr float TICK_INTERVAL = 0.05f;     ///< Broadcast every 50ms (20Hz)
};

#endif // SERVER_PLAYER_STATE_SYSTEM_H
