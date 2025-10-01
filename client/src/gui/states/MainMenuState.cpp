#include "gui/states/MainMenuState.hpp"
#include "gui/states/PublicServerState.hpp"
#include "gui/states/PrivateServerState.hpp"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    MainMenuState::MainMenuState(StateManager& stateManager)
        : stateManager(stateManager), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        
        // Try to load fonts in order of preference
        bool fontLoaded = false;
        
        // Try custom font first
        if (font.loadFromFile("assets/fonts/arial.ttf")) {
            fontLoaded = true;
        }
        // Try common system fonts
        else if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("/System/Library/Fonts/Arial.ttf")) { // macOS
            fontLoaded = true;
        }
        else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) { // Windows
            fontLoaded = true;
        }
        
        if (!fontLoaded) {
            std::cerr << "Warning: Could not load any font file, using SFML default" << std::endl;
        }
        
        setupUI();
    }
    
    void MainMenuState::setupUI() {
        // Title setup
        titleText.setFont(font);
        titleText.setString("THE TOP R-TYPE");
        titleText.setCharacterSize(64);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        
        // Username input setup
        usernameBox.setFillColor(sf::Color(50, 50, 50, 200));
        usernameBox.setOutlineColor(sf::Color::White);
        usernameBox.setOutlineThickness(2.0f);
        
        usernameText.setFont(font);
        usernameText.setCharacterSize(24);
        usernameText.setFillColor(sf::Color::White);
        
        usernameHintText.setFont(font);
        usernameHintText.setString("Add here your username");
        usernameHintText.setCharacterSize(20);
        usernameHintText.setFillColor(sf::Color(150, 150, 150));
        
        // Button setup
        publicServersButton.setFont(font);
        publicServersButton.setString("Public servers");
        publicServersButton.setCharacterSize(28);
        publicServersButton.setFillColor(sf::Color::White);
        
        privateServersButton.setFont(font);
        privateServersButton.setString("Private servers");
        privateServersButton.setCharacterSize(28);
        privateServersButton.setFillColor(sf::Color::White);
        
        // Button rectangles
        publicButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
        publicButtonRect.setOutlineColor(sf::Color::White);
        publicButtonRect.setOutlineThickness(2.0f);
        
        privateButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
        privateButtonRect.setOutlineColor(sf::Color::White);
        privateButtonRect.setOutlineThickness(2.0f);
    }
    
    void MainMenuState::onEnter() {
        // Called when this state becomes active
    }
    
    void MainMenuState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning (center top)
        centerText(titleText, centerX, windowSize.y * 0.2f);
        
        // Username box positioning (middle)
        float boxWidth = std::min(400.0f, windowSize.x * 0.6f);
        float boxHeight = 50.0f;
        usernameBox.setSize(sf::Vector2f(boxWidth, boxHeight));
        usernameBox.setPosition(centerX - boxWidth / 2, centerY - boxHeight / 2);
        
        // Username text positioning
        sf::FloatRect boxBounds = usernameBox.getGlobalBounds();
        usernameHintText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        usernameText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        
        // Button positioning (below username box)
        float buttonWidth = std::min(200.0f, windowSize.x * 0.25f);
        float buttonHeight = 60.0f;
        float buttonSpacing = 20.0f;
        float buttonY = centerY + 80.0f;
        
        // Public servers button
        publicButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        publicButtonRect.setPosition(centerX - buttonWidth - buttonSpacing / 2, buttonY);
        centerText(publicServersButton, 
                  publicButtonRect.getPosition().x + buttonWidth / 2,
                  publicButtonRect.getPosition().y + buttonHeight / 2);
        
        // Private servers button
        privateButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        privateButtonRect.setPosition(centerX + buttonSpacing / 2, buttonY);
        centerText(privateServersButton,
                  privateButtonRect.getPosition().x + buttonWidth / 2,
                  privateButtonRect.getPosition().y + buttonHeight / 2);
    }
    
    void MainMenuState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::Resized) {
            updateLayout(sf::Vector2u(event.size.width, event.size.height));
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                // Check username box click
                if (isPointInRect(mousePos, usernameBox)) {
                    isTyping = true;
                    usernameBox.setOutlineColor(sf::Color::Cyan);
                }
                // Check public servers button
                else if (isPointInRect(mousePos, publicButtonRect)) {
                    onPublicServersClick();
                }
                // Check private servers button
                else if (isPointInRect(mousePos, privateButtonRect)) {
                    onPrivateServersClick();
                }
                // Click outside - stop typing
                else {
                    isTyping = false;
                    usernameBox.setOutlineColor(sf::Color::White);
                }
            }
        }
        
        if (event.type == sf::Event::TextEntered && isTyping) {
            if (event.text.unicode == 8) { // Backspace
                if (!username.empty()) {
                    username.pop_back();
                }
            }
            else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                if (username.length() < 20) { // Max username length
                    username += static_cast<char>(event.text.unicode);
                }
            }
        }
        
        // Mouse hover effects
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            
            // Public button hover
            if (isPointInRect(mousePos, publicButtonRect)) {
                publicButtonRect.setFillColor(sf::Color(100, 100, 100, 200));
                publicServersButton.setFillColor(sf::Color::Cyan);
            } else {
                publicButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
                publicServersButton.setFillColor(sf::Color::White);
            }
            
            // Private button hover
            if (isPointInRect(mousePos, privateButtonRect)) {
                privateButtonRect.setFillColor(sf::Color(100, 100, 100, 200));
                privateServersButton.setFillColor(sf::Color::Cyan);
            } else {
                privateButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
                privateServersButton.setFillColor(sf::Color::White);
            }
        }
    }
    
    void MainMenuState::update(float deltaTime) {
        // Cursor blinking animation
        cursorTimer += deltaTime;
        if (cursorTimer >= 0.5f) {
            showCursor = !showCursor;
            cursorTimer = 0.0f;
        }
        
        // Update username text with cursor
        usernameText.setString(username + (showCursor && isTyping ? "|" : ""));
        
        // Ensure text stays positioned correctly
        sf::FloatRect boxBounds = usernameBox.getGlobalBounds();
        usernameText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
    }
    
    void MainMenuState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        
        // Render title
        window.draw(titleText);
        
        // Render username input
        window.draw(usernameBox);
        if (username.empty() && !isTyping) {
            window.draw(usernameHintText);
        } else {
            window.draw(usernameText);
        }
        
        // Render buttons
        window.draw(publicButtonRect);
        window.draw(publicServersButton);
        window.draw(privateButtonRect);
        window.draw(privateServersButton);
    }
    
    void MainMenuState::onPublicServersClick() {
        std::cout << "Switching to Public Server state" << std::endl;
        std::string finalUsername = username.empty() ? generateUsername() : username;
        stateManager.changeState(std::make_unique<PublicServerState>(stateManager, finalUsername));
    }
    
    void MainMenuState::onPrivateServersClick() {
        std::cout << "Switching to Private Server state" << std::endl;
        std::string finalUsername = username.empty() ? generateUsername() : username;
        stateManager.changeState(std::make_unique<PrivateServerState>(stateManager, finalUsername));
    }
    
    std::string MainMenuState::generateUsername() {
        // Generate random 4-digit number between 1000-9999
        int randomNum = 1000 + (rand() % 9000);
        return "USERNAME" + std::to_string(randomNum);
    }
    
    bool MainMenuState::isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect) {
        sf::FloatRect bounds = rect.getGlobalBounds();
        return bounds.contains(point);
    }
    
    void MainMenuState::centerText(sf::Text& text, float x, float y) {
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x - textBounds.width / 2, y - textBounds.height / 2);
    }
}
