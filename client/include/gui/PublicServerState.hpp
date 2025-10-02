#ifndef CLIENT_PUBLIC_SERVER_STATE_HPP
#define CLIENT_PUBLIC_SERVER_STATE_HPP

#include "State.hpp"
#include "StateManager.hpp"
#include "GUIHelper.hpp"
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
        void toggleReady();
        void updatePlayersReadyText();
    };
}

#endif // CLIENT_PUBLIC_SERVER_STATE_HPP
