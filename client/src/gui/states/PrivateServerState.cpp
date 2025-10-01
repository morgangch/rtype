#include "gui/states/PrivateServerState.hpp"
#include "gui/states/PrivateServerLobbyState.hpp"
#include "gui/states/MainMenuState.hpp"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    PrivateServerState::PrivateServerState(StateManager& stateManager, const std::string& username)
        : stateManager(stateManager), username(username), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        
        // Try to load fonts in order of preference
        bool fontLoaded = false;
        
        if (font.loadFromFile("assets/fonts/arial.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("/System/Library/Fonts/Arial.ttf")) {
            fontLoaded = true;
        }
        else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }
        
        if (!fontLoaded) {
            std::cerr << "Warning: Could not load any font file, using SFML default" << std::endl;
        }
        
        setupUI();
    }
    
    void PrivateServerState::setupUI() {
        // Title setup
        titleText.setFont(font);
        titleText.setString("Private Servers");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        
        // Server code input setup
        serverCodeBox.setFillColor(sf::Color(50, 50, 50, 200));
        serverCodeBox.setOutlineColor(sf::Color::White);
        serverCodeBox.setOutlineThickness(2.0f);
        
        serverCodeText.setFont(font);
        serverCodeText.setCharacterSize(24);
        serverCodeText.setFillColor(sf::Color::White);
        
        serverCodeHintText.setFont(font);
        serverCodeHintText.setString("Enter server code (1000-9999)");
        serverCodeHintText.setCharacterSize(20);
        serverCodeHintText.setFillColor(sf::Color(150, 150, 150));
        
        // Join button setup
        joinButton.setFont(font);
        joinButton.setString("Join Server");
        joinButton.setCharacterSize(28);
        joinButton.setFillColor(sf::Color::White);
        
        joinButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
        joinButtonRect.setOutlineColor(sf::Color::White);
        joinButtonRect.setOutlineThickness(2.0f);
        
        // Create button setup
        createButton.setFont(font);
        createButton.setString("Create New Server");
        createButton.setCharacterSize(28);
        createButton.setFillColor(sf::Color::White);
        
        createButtonRect.setFillColor(sf::Color(50, 50, 100, 200));
        createButtonRect.setOutlineColor(sf::Color::White);
        createButtonRect.setOutlineThickness(2.0f);
        
        // Return button setup
        returnButton.setFont(font);
        returnButton.setString("Return");
        returnButton.setCharacterSize(24);
        returnButton.setFillColor(sf::Color::White);
        
        returnButtonRect.setFillColor(sf::Color(100, 50, 50, 200));
        returnButtonRect.setOutlineColor(sf::Color::White);
        returnButtonRect.setOutlineThickness(2.0f);
    }
    
    void PrivateServerState::onEnter() {
        std::cout << "Entered Private Server state with username: " << username << std::endl;
    }
    
    void PrivateServerState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning
        centerText(titleText, centerX, windowSize.y * 0.2f);
        
        // Server code input positioning
        float boxWidth = std::min(400.0f, windowSize.x * 0.6f);
        float boxHeight = 50.0f;
        serverCodeBox.setSize(sf::Vector2f(boxWidth, boxHeight));
        serverCodeBox.setPosition(centerX - boxWidth / 2, centerY - boxHeight / 2);
        
        // Server code text positioning
        sf::FloatRect boxBounds = serverCodeBox.getGlobalBounds();
        serverCodeHintText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        serverCodeText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        
        // Button positioning
        float buttonWidth = 200.0f;
        float buttonHeight = 60.0f;
        float buttonSpacing = 20.0f;
        float buttonY = centerY + 80.0f;
        
        // Join button
        joinButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        joinButtonRect.setPosition(centerX - buttonWidth - buttonSpacing / 2, buttonY);
        centerText(joinButton,
                  joinButtonRect.getPosition().x + buttonWidth / 2,
                  joinButtonRect.getPosition().y + buttonHeight / 2);
        
        // Create button
        createButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        createButtonRect.setPosition(centerX + buttonSpacing / 2, buttonY);
        centerText(createButton,
                  createButtonRect.getPosition().x + buttonWidth / 2,
                  createButtonRect.getPosition().y + buttonHeight / 2);
        
        // Return button positioning (top left)
        float returnButtonWidth = 120.0f;
        float returnButtonHeight = 40.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
    }
    
    void PrivateServerState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::Resized) {
            updateLayout(sf::Vector2u(event.size.width, event.size.height));
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                // Check server code box click
                if (isPointInRect(mousePos, serverCodeBox)) {
                    isTyping = true;
                    serverCodeBox.setOutlineColor(sf::Color::Cyan);
                }
                // Check join button click
                else if (isPointInRect(mousePos, joinButtonRect)) {
                    joinServer();
                }
                // Check create button click
                else if (isPointInRect(mousePos, createButtonRect)) {
                    createServer();
                }
                // Check return button click
                else if (isPointInRect(mousePos, returnButtonRect)) {
                    stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
                }
                // Click outside - stop typing
                else {
                    isTyping = false;
                    serverCodeBox.setOutlineColor(sf::Color::White);
                }
            }
        }
        
        if (event.type == sf::Event::TextEntered && isTyping) {
            if (event.text.unicode == 8) { // Backspace
                if (!serverCode.empty()) {
                    serverCode.pop_back();
                }
            }
            else if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                if (serverCode.length() < 4) { // Max 4 digits
                    serverCode += static_cast<char>(event.text.unicode);
                }
            }
        }
        
        // Mouse hover effects
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            
            // Join button hover
            if (isPointInRect(mousePos, joinButtonRect)) {
                joinButtonRect.setFillColor(sf::Color(70, 150, 70, 200));
                joinButton.setFillColor(sf::Color::Cyan);
            } else {
                joinButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
                joinButton.setFillColor(sf::Color::White);
            }
            
            // Create button hover
            if (isPointInRect(mousePos, createButtonRect)) {
                createButtonRect.setFillColor(sf::Color(70, 70, 150, 200));
                createButton.setFillColor(sf::Color::Cyan);
            } else {
                createButtonRect.setFillColor(sf::Color(50, 50, 100, 200));
                createButton.setFillColor(sf::Color::White);
            }
            
            // Return button hover
            if (isPointInRect(mousePos, returnButtonRect)) {
                returnButtonRect.setFillColor(sf::Color(150, 70, 70, 200));
                returnButton.setFillColor(sf::Color::Yellow);
            } else {
                returnButtonRect.setFillColor(sf::Color(100, 50, 50, 200));
                returnButton.setFillColor(sf::Color::White);
            }
        }
    }
    
    void PrivateServerState::update(float deltaTime) {
        // Cursor blinking animation
        cursorTimer += deltaTime;
        if (cursorTimer >= 0.5f) {
            showCursor = !showCursor;
            cursorTimer = 0.0f;
        }
        
        // Update server code text with cursor
        serverCodeText.setString(serverCode + (showCursor && isTyping ? "|" : ""));
        
        // Ensure text stays positioned correctly
        sf::FloatRect boxBounds = serverCodeBox.getGlobalBounds();
        serverCodeText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
    }
    
    void PrivateServerState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        
        // Render title
        window.draw(titleText);
        
        // Render server code input
        window.draw(serverCodeBox);
        if (serverCode.empty() && !isTyping) {
            window.draw(serverCodeHintText);
        } else {
            window.draw(serverCodeText);
        }
        
        // Render buttons
        window.draw(joinButtonRect);
        window.draw(joinButton);
        window.draw(createButtonRect);
        window.draw(createButton);
        window.draw(returnButtonRect);
        window.draw(returnButton);
    }
    
    void PrivateServerState::joinServer() {
        if (isValidServerCode(serverCode)) {
            std::cout << "Joining server with code: " << serverCode << std::endl;
            std::cout << "Sending network data: ServerType=1 (private), Username=" << username << ", ServerCode=" << serverCode << std::endl;
            
            // Switch to private lobby state as a regular player
            stateManager.changeState(std::make_unique<PrivateServerLobbyState>(stateManager, username, serverCode, false));
        } else {
            std::cout << "Invalid server code. Please enter a 4-digit number between 1000-9999." << std::endl;
        }
    }
    
    void PrivateServerState::createServer() {
        // Generate random server code between 1000-9999
        int randomCode = 1000 + (rand() % 9000);
        std::string generatedCode = std::to_string(randomCode);
        
        std::cout << "Creating new server with code: " << generatedCode << std::endl;
        std::cout << "Sending network data: ServerType=1 (private), Username=" << username << ", ServerCode=" << generatedCode << ", IsAdmin=true" << std::endl;
        
        // Switch to private lobby state as the admin
        stateManager.changeState(std::make_unique<PrivateServerLobbyState>(stateManager, username, generatedCode, true));
    }
    
    bool PrivateServerState::isValidServerCode(const std::string& code) {
        if (code.length() != 4) return false;
        
        try {
            int num = std::stoi(code);
            return num >= 1000 && num <= 9999;
        } catch (...) {
            return false;
        }
    }
    
    bool PrivateServerState::isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect) {
        sf::FloatRect bounds = rect.getGlobalBounds();
        return bounds.contains(point);
    }
    
    void PrivateServerState::centerText(sf::Text& text, float x, float y) {
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x - textBounds.width / 2, y - textBounds.height / 2);
    }
}
