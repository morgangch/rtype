/**
 * @file LobbyState.h
 * @brief Component tracking a player's lobby state
 *
 * This component is attached to player entities while they are in a room lobby.
 * It records whether the player is ready and whether they are currently in-game
 * (i.e., transitioned from the lobby into an active match).
 *
 * The server uses this information when evaluating whether a room can start
 * the game and when broadcasting lobby updates to clients.
 *
 * @author R-TYPE Dev Team
 * @date 2025
 */

#ifndef LOBBYSTATECOMPONENT_H
#define LOBBYSTATECOMPONENT_H

#include "ECS/Component.h"

namespace rtype::server::components {
    /**
     * @brief Simple POD component to store lobby-related flags for a player
     *
     * Members:
     * - isReady: whether the player clicked "ready" in the lobby
     * - isInGame: whether the player has transitioned into the game scene
     */
    class LobbyState : public ECS::Component<LobbyState> {
    public:
        /**
         * @brief True when the player has signaled ready in the lobby
         */
        bool isReady = false;

        /**
         * @brief True once the player has moved into the in-game state
         *
         * @note This flag helps the server decide which packets to send and
         * which players should receive in-game state updates.
         */
        bool isInGame = false;
        
        /**
         * @brief Construct a new LobbyState component
         * @param isReady Initial ready state (default: false)
         * @param isInGame Initial in-game state (default: false)
         */
        LobbyState(bool isReady = false, bool isInGame = false)
            : isReady(isReady), isInGame(isInGame) {}
    };
}

#endif //LOBBYSTATECOMPONENT_H
