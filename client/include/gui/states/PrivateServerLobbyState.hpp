#ifndef CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
#define CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP

#include "State.hpp"
#include "../StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    class PrivateServerLobbyState : public State {
    public:
        PrivateServerLobbyState(StateManager& stateManager, const std::string& username, const std::string& serverCode, bool isAdmin);
        
        void handleEvent(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;
        
        void onEnter() override;
        
    private:
        StateManager& stateManager;
        std::string username;
        std::string serverCode;
        bool isAdmin;
        sf::Font font;
        
        // UI Elements
        sf::Text playersReadyText;
        sf::Text actionButton; // "Ready" for players, "Start" for admin
        sf::RectangleShape actionButtonRect;
        sf::Text returnButton;
        sf::RectangleShape returnButtonRect;
        sf::Text serverCodeDisplay;
        
        // State
        bool isReady;
        int playersReady;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        void centerText(sf::Text& text, float x, float y);
        void toggleReady();
        void startGame();
        void updatePlayersReadyText();
        void updateActionButton();
    };
}

#endif // CLIENT_PRIVATE_SERVER_LOBBY_STATE_HPP
