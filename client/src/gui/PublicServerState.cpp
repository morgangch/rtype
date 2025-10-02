#include "gui/PublicServerState.hpp"
#include "gui/MainMenuState.hpp"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    PublicServerState::PublicServerState(StateManager& stateManager, const std::string& username)
        : stateManager(stateManager), username(username), isReady(false), playersReady(0) {
        setupUI();
    }
    
    void PublicServerState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Players ready text setup
        playersReadyText.setFont(font);
        playersReadyText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 28);
        playersReadyText.setFillColor(GUIHelper::Colors::TEXT);
        
        // Button setup using GUIHelper
        GUIHelper::setupButton(readyButton, readyButtonRect, "", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        GUIHelper::setupReturnButton(returnButton, returnButtonRect);
        
        updatePlayersReadyText();
    }
    
    void PublicServerState::onEnter() {
        std::cout << "Entered Public Server state with username: " << username << std::endl;
        std::cout << "Sending network data: ServerType=0 (public), Username=" << username << std::endl;
    }
    
    void PublicServerState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Players ready text positioning (center)
        GUIHelper::centerText(playersReadyText, centerX, centerY - 50.0f);
        
        // Ready button positioning (below the text)
        float buttonWidth = 200.0f;
        float buttonHeight = 60.0f;
        float buttonY = centerY + 50.0f;
        
        readyButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        readyButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
        GUIHelper::centerText(readyButton,
                  readyButtonRect.getPosition().x + buttonWidth / 2,
                  readyButtonRect.getPosition().y + buttonHeight / 2);
        
        // Return button positioning (top left)
        float returnButtonWidth = 120.0f;
        float returnButtonHeight = 40.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        GUIHelper::centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
    }
    
    void PublicServerState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::Resized) {
            updateLayout(sf::Vector2u(event.size.width, event.size.height));
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                // Go back to main menu
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                // Check ready button click
                if (GUIHelper::isPointInRect(mousePos, readyButtonRect)) {
                    toggleReady();
                }
                // Check return button click
                else if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
                    stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
                }
            }
        }
        
        // Mouse hover effects
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            
            // Button hover effects using GUIHelper
            GUIHelper::applyButtonHover(readyButtonRect, readyButton, 
                                      GUIHelper::isPointInRect(mousePos, readyButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
            
            GUIHelper::applyButtonHover(returnButtonRect, returnButton, 
                                      GUIHelper::isPointInRect(mousePos, returnButtonRect),
                                      GUIHelper::Colors::RETURN_BUTTON, sf::Color(150, 70, 70, 200));
        }
    }
    
    void PublicServerState::update(float deltaTime) {
        // Update logic here if needed
        // For example, network updates to get real player count
    }
    
    void PublicServerState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        
        // Render players ready text
        window.draw(playersReadyText);
        
        // Render ready button
        window.draw(readyButtonRect);
        window.draw(readyButton);
        
        // Render return button
        window.draw(returnButtonRect);
        window.draw(returnButton);
    }
    
    void PublicServerState::toggleReady() {
        isReady = !isReady;
        
        if (isReady) {
            playersReady++;
            readyButton.setString("Ready");
            readyButtonRect.setFillColor(sf::Color(50, 150, 50, 200)); // Green when ready
            std::cout << username << " is now ready!" << std::endl;
        } else {
            playersReady--;
            readyButton.setString("Not ready");
            readyButtonRect.setFillColor(sf::Color(70, 70, 70, 200)); // Gray when not ready
            std::cout << username << " is no longer ready!" << std::endl;
        }
        
        updatePlayersReadyText();
    }
    
    void PublicServerState::updatePlayersReadyText() {
        playersReadyText.setString("Amount of players ready: " + std::to_string(playersReady));
        
        // Update button text based on ready state
        if (isReady) {
            readyButton.setString("Ready");
        } else {
            readyButton.setString("Not ready");
        }
    }
}
