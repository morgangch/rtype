#ifndef CLIENT_MAIN_MENU_HPP
#define CLIENT_MAIN_MENU_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace rtype::client::gui {
    class MainMenu {
    public:
        MainMenu(sf::RenderWindow& window);
        
        void handleEvent(const sf::Event& event);
        void update(float deltaTime);
        void render();
        
        // Callbacks for button clicks
        void setPublicServersCallback(std::function<void()> callback);
        void setPrivateServersCallback(std::function<void()> callback);
        
        const std::string& getUsername() const { return username; }
        
    private:
        sf::RenderWindow& window;
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
        
        // Button rectangles for click detections
        sf::RectangleShape publicButtonRect;
        sf::RectangleShape privateButtonRect;
        
        // Callbacks
        std::function<void()> onPublicServersClick;
        std::function<void()> onPrivateServersClick;
        
        // Helper methods
        void setupUI();
        void updateLayout();
        bool isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect);
        void centerText(sf::Text& text, float x, float y);
    };
}

#endif // CLIENT_MAIN_MENU_HPP
