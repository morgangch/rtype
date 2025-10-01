#ifndef CLIENT_PUBLIC_SERVER_STATE_HPP
#define CLIENT_PUBLIC_SERVER_STATE_HPP

#include "State.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    class PublicServerState : public State {
    public:
        PublicServerState(StateManager& stateManager, const std::string& username);
        
        void handleEvent(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;
        
        void onEnter() override;
        
    private:
        StateManager& stateManager;
        std::string username;
        sf::Font font;
        
        // UI Elements
        sf::Text playersReadyText;
        sf::Text readyButton;
        sf::RectangleShape readyButtonRect;
        sf::Text returnButton;
        sf::RectangleShape returnButtonRect;
        
        // State
        bool isReady;
        int playersReady;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        void centerText(sf::Text& text, float x, float y);
        void toggleReady();
        void updatePlayersReadyText();
    };
}

#endif // CLIENT_PUBLIC_SERVER_STATE_HPP
