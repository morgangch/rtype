#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP

#include <SFML/Graphics.hpp>

namespace rtype::client::gui {
    class State {
    public:
        virtual ~State() = default;
        
        virtual void handleEvent(const sf::Event& event) = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;
        
        virtual void onEnter() {}
        virtual void onExit() {}
    };
}

#endif // CLIENT_STATE_HPP
