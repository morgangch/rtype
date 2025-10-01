#include "gui/states/PrivateServerLobbyState.hpp"
#include "gui/states/MainMenuState.hpp"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    PrivateServerLobbyState::PrivateServerLobbyState(StateManager& stateManager, const std::string& username, 
                                                     const std::string& serverCode, bool isAdmin)
        : stateManager(stateManager), username(username), serverCode(serverCode), isAdmin(isAdmin), 
          isReady(false), playersReady(0) {
        
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
    
    void PrivateServerLobbyState::setupUI() {
        // Players ready text setup
        playersReadyText.setFont(font);
        playersReadyText.setCharacterSize(36);
        playersReadyText.setFillColor(sf::Color::White);
        
        // Action button setup (Ready/Start)
        actionButton.setFont(font);
        actionButton.setCharacterSize(28);
        actionButton.setFillColor(sf::Color::White);
        
        // Action button rectangle
        actionButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
        actionButtonRect.setOutlineColor(sf::Color::White);
        actionButtonRect.setOutlineThickness(2.0f);
        
        // Return button setup
        returnButton.setFont(font);
        returnButton.setString("Return");
        returnButton.setCharacterSize(24);
        returnButton.setFillColor(sf::Color::White);
        
        // Return button rectangle
        returnButtonRect.setFillColor(sf::Color(100, 50, 50, 200));
        returnButtonRect.setOutlineColor(sf::Color::White);
        returnButtonRect.setOutlineThickness(2.0f);
        
        // Server code display setup
        serverCodeDisplay.setFont(font);
        serverCodeDisplay.setString("Server Code: " + serverCode);
        serverCodeDisplay.setCharacterSize(24);
        serverCodeDisplay.setFillColor(sf::Color::Yellow);
        serverCodeDisplay.setStyle(sf::Text::Bold);
        
        updatePlayersReadyText();
        updateActionButton();
    }
    
    void PrivateServerLobbyState::onEnter() {
        std::cout << "Entered Private Server Lobby:" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Server Code: " << serverCode << std::endl;
        std::cout << "Is Admin: " << (isAdmin ? "Yes" : "No") << std::endl;
    }
    
    void PrivateServerLobbyState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Players ready text positioning (center)
        centerText(playersReadyText, centerX, centerY - 50.0f);
        
        // Action button positioning (below the text)
        float buttonWidth = 200.0f;
        float buttonHeight = 60.0f;
        float buttonY = centerY + 50.0f;
        
        actionButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        actionButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
        centerText(actionButton,
                  actionButtonRect.getPosition().x + buttonWidth / 2,
                  actionButtonRect.getPosition().y + buttonHeight / 2);
        
        // Return button positioning (top left)
        float returnButtonWidth = 120.0f;
        float returnButtonHeight = 40.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
        
        // Server code display positioning (top right, moved more to the left)
        sf::FloatRect codeTextBounds = serverCodeDisplay.getLocalBounds();
        serverCodeDisplay.setPosition(windowSize.x - codeTextBounds.width - 30.0f, 30.0f);
    }
    
    void PrivateServerLobbyState::handleEvent(const sf::Event& event) {
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
                
                // Check action button click
                if (isPointInRect(mousePos, actionButtonRect)) {
                    if (isAdmin) {
                        startGame();
                    } else {
                        toggleReady();
                    }
                }
                // Check return button click
                else if (isPointInRect(mousePos, returnButtonRect)) {
                    stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
                }
            }
        }
        
        // Mouse hover effects
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            
            // Action button hover
            if (isPointInRect(mousePos, actionButtonRect)) {
                if (isAdmin) {
                    actionButtonRect.setFillColor(sf::Color(50, 150, 50, 200));
                } else {
                    actionButtonRect.setFillColor(sf::Color(100, 100, 100, 200));
                }
                actionButton.setFillColor(sf::Color::Cyan);
            } else {
                if (isAdmin) {
                    actionButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
                } else {
                    actionButtonRect.setFillColor(sf::Color(70, 70, 70, 200));
                }
                actionButton.setFillColor(sf::Color::White);
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
    
    void PrivateServerLobbyState::update(float deltaTime) {
        // Update logic here if needed
        // For example, network updates to get real player count
    }
    
    void PrivateServerLobbyState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        
        // Render players ready text
        window.draw(playersReadyText);
        
        // Render action button
        window.draw(actionButtonRect);
        window.draw(actionButton);
        
        // Render return button
        window.draw(returnButtonRect);
        window.draw(returnButton);
        
        // Render server code display
        window.draw(serverCodeDisplay);
    }
    
    void PrivateServerLobbyState::toggleReady() {
        if (!isAdmin) { // Only non-admin players can toggle ready
            isReady = !isReady;
            
            if (isReady) {
                playersReady++;
                std::cout << username << " is now ready!" << std::endl;
            } else {
                playersReady--;
                std::cout << username << " is no longer ready!" << std::endl;
            }
            
            updatePlayersReadyText();
            updateActionButton();
        }
    }
    
    void PrivateServerLobbyState::startGame() {
        if (isAdmin) {
            std::cout << "Admin " << username << " is starting the game!" << std::endl;
            std::cout << "Game starting with " << playersReady << " ready players in server " << serverCode << std::endl;
            // TODO: Start the actual game
        }
    }
    
    void PrivateServerLobbyState::updatePlayersReadyText() {
        playersReadyText.setString("Amount of players ready: " + std::to_string(playersReady));
    }
    
    void PrivateServerLobbyState::updateActionButton() {
        if (isAdmin) {
            actionButton.setString("Start Game");
            actionButtonRect.setFillColor(sf::Color(50, 100, 50, 200)); // Green for start
        } else {
            if (isReady) {
                actionButton.setString("Ready");
                actionButtonRect.setFillColor(sf::Color(50, 150, 50, 200)); // Green when ready
            } else {
                actionButton.setString("Not ready");
                actionButtonRect.setFillColor(sf::Color(70, 70, 70, 200)); // Gray when not ready
            }
        }
    }
    
    bool PrivateServerLobbyState::isPointInRect(const sf::Vector2f& point, const sf::RectangleShape& rect) {
        sf::FloatRect bounds = rect.getGlobalBounds();
        return bounds.contains(point);
    }
    
    void PrivateServerLobbyState::centerText(sf::Text& text, float x, float y) {
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x - textBounds.width / 2, y - textBounds.height / 2);
    }
}
