/**
 * @file MainMenuState.cpp
 * @brief Implementation of the MainMenuState class
 * 
 * This file implements the main menu interface for the R-TYPE client.
 * It handles user input for username entry, button interactions, and
 * navigation to server selection screens. The implementation uses the
 * centralized GUIHelper utilities for consistent styling and behavior.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/MainMenuState.hpp"
#include "gui/PublicServerState.hpp"
#include "gui/PrivateServerState.hpp"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    
    MainMenuState::MainMenuState(StateManager& stateManager)
        : stateManager(stateManager), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        setupUI();
    }
    
    void MainMenuState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Title setup
        titleText.setFont(font);
        titleText.setString("THE TOP R-TYPE");
        titleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE);
        titleText.setFillColor(GUIHelper::Colors::TEXT);
        titleText.setStyle(sf::Text::Bold);
        
        // Username input setup
        usernameBox.setFillColor(GUIHelper::Colors::INPUT_BOX);
        usernameBox.setOutlineColor(GUIHelper::Colors::TEXT);
        usernameBox.setOutlineThickness(2.0f);
        
        usernameText.setFont(font);
        usernameText.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        usernameText.setFillColor(GUIHelper::Colors::TEXT);
        
        usernameHintText.setFont(font);
        usernameHintText.setString("Add here your username");
        usernameHintText.setCharacterSize(GUIHelper::Sizes::HINT_FONT_SIZE);
        usernameHintText.setFillColor(GUIHelper::Colors::HINT_TEXT);
        
        // Button setup using GUIHelper
        GUIHelper::setupButton(publicServersButton, publicButtonRect, "Public servers", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        GUIHelper::setupButton(privateServersButton, privateButtonRect, "Private servers", GUIHelper::Sizes::BUTTON_FONT_SIZE);
    }
    
    void MainMenuState::onEnter() {
        // Called when this state becomes active
    }
    
    void MainMenuState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning (center top)
        GUIHelper::centerText(titleText, centerX, windowSize.y * 0.2f);
        
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
        GUIHelper::centerText(publicServersButton, 
                  publicButtonRect.getPosition().x + buttonWidth / 2,
                  publicButtonRect.getPosition().y + buttonHeight / 2);
        
        // Private servers button
        privateButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        privateButtonRect.setPosition(centerX + buttonSpacing / 2, buttonY);
        GUIHelper::centerText(privateServersButton,
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
                if (GUIHelper::isPointInRect(mousePos, usernameBox)) {
                    isTyping = true;
                    usernameBox.setOutlineColor(sf::Color::Cyan);
                }
                // Check public servers button
                else if (GUIHelper::isPointInRect(mousePos, publicButtonRect)) {
                    onPublicServersClick();
                }
                // Check private servers button
                else if (GUIHelper::isPointInRect(mousePos, privateButtonRect)) {
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
            GUIHelper::applyButtonHover(publicButtonRect, publicServersButton, 
                                      GUIHelper::isPointInRect(mousePos, publicButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
            
            // Private button hover
            GUIHelper::applyButtonHover(privateButtonRect, privateServersButton, 
                                      GUIHelper::isPointInRect(mousePos, privateButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
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
        std::string finalUsername = username.empty() ? GUIHelper::generateRandomUsername() : username;
        stateManager.changeState(std::make_unique<PublicServerState>(stateManager, finalUsername));
    }
    
    void MainMenuState::onPrivateServersClick() {
        std::cout << "Switching to Private Server state" << std::endl;
        std::string finalUsername = username.empty() ? GUIHelper::generateRandomUsername() : username;
        stateManager.changeState(std::make_unique<PrivateServerState>(stateManager, finalUsername));
    }
}
