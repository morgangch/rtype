#ifndef CLIENT_STATE_MANAGER_HPP
#define CLIENT_STATE_MANAGER_HPP

#include "states/State.hpp"
#include <memory>
#include <stack>
#include <functional>
#include <SFML/Graphics.hpp>

namespace rtype::client::gui {
    class StateManager {
    public:
        StateManager(sf::RenderWindow& window);
        
        void pushState(std::unique_ptr<State> state);
        void popState();
        void changeState(std::unique_ptr<State> state);
        
        void handleEvent(const sf::Event& event);
        void update(float deltaTime);
        void render();
        
        bool isEmpty() const;
        
        // Game launch callback
        void setOnGameStartCallback(std::function<void()> callback) { onGameStart = callback; }
        void requestGameStart() { if (onGameStart) onGameStart(); }
        
    private:
        sf::RenderWindow& window;
        std::stack<std::unique_ptr<State>> states;
        std::function<void()> onGameStart;
    };
}

#endif // CLIENT_STATE_MANAGER_HPP
