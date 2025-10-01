#ifndef CLIENT_PRIVATE_SERVER_STATE_HPP
#define CLIENT_PRIVATE_SERVER_STATE_HPP

#include "State.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace rtype::client::gui {
    class PrivateServerState : public State {
    public:
        PrivateServerState(StateManager& stateManager, const std::string& username);
        
        void handleEvent(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;
        
        void onEnter() override;
        
    private:
        StateManager& stateManager;
        std::string username;
        sf::Font font;
        
        // UI Elements
        sf::Text titleText;
        sf::Text serverCodeText;
        sf::Text serverCodeHintText;
        sf::RectangleShape serverCodeBox;
        sf::Text joinButton;
        sf::RectangleShape joinButtonRect;
        sf::Text createButton;
        sf::RectangleShape createButtonRect;
        sf::Text returnButton;
        sf::RectangleShape returnButtonRect;
        
        // State
        std::string serverCode;
        bool isTyping;
        float cursorTimer;
        bool showCursor;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        void centerText(sf::Text& text, float x, float y);
        void joinServer();
        void createServer();
        bool isValidServerCode(const std::string& code);
    };
}

#endif // CLIENT_PRIVATE_SERVER_STATE_HPP
