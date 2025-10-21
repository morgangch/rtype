/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** LobbyState component - tracks player ready state in lobby
*/

#ifndef LOBBYSTATECOMPONENT_H
#define LOBBYSTATECOMPONENT_H

#include "ECS/Component.h"

namespace rtype::server::components {
    class LobbyState : public ECS::Component<LobbyState> {
    public:
        bool isReady = false;
        bool isInGame = false; // true when player transitions from lobby to game
        
        LobbyState(bool isReady = false, bool isInGame = false)
            : isReady(isReady), isInGame(isInGame) {}
    };
}

#endif //LOBBYSTATECOMPONENT_H
