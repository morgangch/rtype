#ifndef CLIENT_MAIN_MENU_STATE_HPP
#define CLIENT_MAIN_MENU_STATE_HPP

#include "State.hpp"
#include "StateManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace rtype::client::gui {
    class MainMenuState : public State {
    public:
        MainMenuState(StateManager& stateManager);
        
        void handleEvent(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;
        
        void onEnter() override;
        
        const std::string& getUsername() const { return username; }
        
    private:
        StateManager& stateManager;
        sf::Font font;
        
        // UI Elements
        sf::Text titleText;
        sf::Text usernameText;
        sf::Text usernameHintText;
        sf::Text publicServersButton;
        sf::Text privateServersButton;
        
        // Input field
        sf::RectangleShape usernameBox;
        std::string username;
        bool isTyping;
        float cursorTimer;
        bool showCursor;
        
        // Button rectangles for click detection
        sf::RectangleShape publicButtonRect;
        sf::RectangleShape privateButtonRect;
        
        // Helper methods
        void setupUI();
        void updateLayout(const sf::Vector2u& windowSize);
        bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        void centerText(sf::Text& text, float x, float y);
        std::string generateUsername();
        
        // Button actions
        void onPublicServersClick();
        void onPrivateServersClick();
    };
}

#endif // CLIENT_MAIN_MENU_STATE_HPP
