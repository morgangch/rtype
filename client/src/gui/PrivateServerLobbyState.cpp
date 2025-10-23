/**
 * @file PrivateServerLobbyState.cpp
 * @brief Implementation of the private server lobby state for the R-Type client GUI
 * 
 * This file contains the implementation of the PrivateServerLobbyState class, which handles
 * the private server lobby interface where players wait for game start. The lobby supports
 * both admin and regular player roles, with different UI interactions for each role.
 * 
 * Key features:
 * - Ready state management for players
 * - Admin controls for starting games
 * - Real-time player count display
 * - Server code display for sharing
 * - Dynamic button behavior based on user role
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/PrivateServerLobbyState.h"
#include "gui/GameState.h"
#include "gui/MainMenuState.h"
#include "network/senders.h"
#include <iostream>
#include <cstdlib>
#include <string>

// External declaration of global player server ID set by JOIN_ROOM_ACCEPTED handler
extern uint32_t g_playerServerId;

namespace rtype::client::gui {
    // Global pointer to current lobby state (for network callbacks)
    PrivateServerLobbyState* g_lobbyState = nullptr;
    PrivateServerLobbyState::PrivateServerLobbyState(StateManager& stateManager, const std::string& username, 
                                                     const std::string& serverCode, bool isAdmin)
        : stateManager(stateManager), username(username), serverCode(serverCode), isAdmin(isAdmin) {
        setupUI();
        g_lobbyState = this; // Register this instance globally for network callbacks
    }
    
    void PrivateServerLobbyState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Waiting text setup (for non-admin players)
        playersWaitingText.setFont(font);
        playersWaitingText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 28);
        playersWaitingText.setFillColor(GUIHelper::Colors::TEXT);
        
        // Action button setup (admin only)
        if (isAdmin) {
            GUIHelper::setupButton(actionButton, actionButtonRect, "", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        }
        
        // Return button
        GUIHelper::setupReturnButton(returnButton, returnButtonRect);
        
        // Server code display setup
        serverCodeDisplay.setFont(font);
        serverCodeDisplay.setString("Server Code " + serverCode);
        serverCodeDisplay.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        serverCodeDisplay.setFillColor(sf::Color::Yellow);
        serverCodeDisplay.setStyle(sf::Text::Bold);
        
        updateWaitingText();
        if (isAdmin) {
            updateActionButton();
        }
    }
    
    void PrivateServerLobbyState::onEnter() {
        std::cout << "Entered Private Server Lobby:" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Server Code " << serverCode << std::endl;
        std::cout << "Is Admin: " << (isAdmin ? "Yes" : "No") << std::endl;
    }
    
    void PrivateServerLobbyState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Waiting text positioning (center)
        GUIHelper::centerText(playersWaitingText, centerX, centerY - 50.0f);
        
        // Action button positioning (admin only, below the text)
        if (isAdmin) {
            float buttonWidth = 300.0f;
            float buttonHeight = 80.0f;
            float buttonY = centerY + 50.0f;
            
            actionButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            actionButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
            GUIHelper::centerText(actionButton,
                      actionButtonRect.getPosition().x + buttonWidth / 2,
                      actionButtonRect.getPosition().y + buttonHeight / 2);
        }
        
        // Return button positioning (top left)
        float returnButtonWidth = 150.0f;
        float returnButtonHeight = 50.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        GUIHelper::centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
        
        // Server code display positioning (top right, moved more to the left)
        sf::FloatRect codeTextBounds = serverCodeDisplay.getLocalBounds();
        serverCodeDisplay.setPosition(windowSize.x - codeTextBounds.width - 30.0f, 30.0f);
    }
    
    void PrivateServerLobbyState::handleEvent(const sf::Event& event) {
        switch (event.type) {
            case sf::Event::Resized:
                handleResizeEvent(event);
                break;
            case sf::Event::KeyPressed:
                handleKeyboardEvent(event);
                break;
            case sf::Event::MouseButtonPressed:
                handleMouseButtonEvent(event);
                break;
            case sf::Event::MouseMoved:
                handleMouseMoveEvent(event);
                break;
            default:
                break;
        }
    }
    
    void PrivateServerLobbyState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void PrivateServerLobbyState::handleKeyboardEvent(const sf::Event& event) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
        }
    }
    
    void PrivateServerLobbyState::handleMouseButtonEvent(const sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check action button click (admin only)
            if (isAdmin && GUIHelper::isPointInRect(mousePos, actionButtonRect)) {
                startGame();
            }
            // Check return button click
            else if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
        }
    }
    
    void PrivateServerLobbyState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        // Button hover effects for admin start button
        if (isAdmin) {
            bool isActionHovered = GUIHelper::isPointInRect(mousePos, actionButtonRect);
            if (isActionHovered) {
                actionButtonRect.setFillColor(sf::Color(50, 150, 50, 200));
                actionButton.setFillColor(sf::Color::Cyan);
            } else {
                actionButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
                actionButton.setFillColor(GUIHelper::Colors::TEXT);
            }
        }
        
        // Return button hover
        GUIHelper::applyButtonHover(returnButtonRect, returnButton, 
                                  GUIHelper::isPointInRect(mousePos, returnButtonRect),
                                  GUIHelper::Colors::RETURN_BUTTON, sf::Color(150, 70, 70, 200));
    }
    
    void PrivateServerLobbyState::update(float deltaTime) {
        // Update logic here if needed
        // For example, network updates to get real player count
    }
    
    void PrivateServerLobbyState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        
        // Render waiting text
        window.draw(playersWaitingText);
        
        // Render action button (admin only)
        if (isAdmin) {
            window.draw(actionButtonRect);
            window.draw(actionButton);
        }
        
        // Render return button
        window.draw(returnButtonRect);
        window.draw(returnButton);
        
        // Render server code display
        window.draw(serverCodeDisplay);
    }
    
    void PrivateServerLobbyState::startGame() {
        if (isAdmin) {
            std::cout << "Admin " << username << " is starting the game!" << std::endl;
            std::cout << "Sending GAME_START_REQUEST to server in room " << serverCode << std::endl;
            
            rtype::client::network::senders::send_game_start_request();
        }
    }
    
    void PrivateServerLobbyState::updateWaitingText() {
        if (isAdmin) {
            playersWaitingText.setString("Waiting for players...");
        } else {
            playersWaitingText.setString("Waiting for room host");
        }
    }
    
    void PrivateServerLobbyState::updateActionButton() {
        if (isAdmin) {
            actionButton.setString("Start Game");
            actionButtonRect.setFillColor(sf::Color(50, 100, 50, 200)); // Green for start
        }
    }
    
    void PrivateServerLobbyState::updateFromServer(uint32_t totalPlayers) {
        std::cout << "Lobby updated from server: " << totalPlayers << " total players" << std::endl;
        
        // Update the display text
        if (isAdmin) {
            playersWaitingText.setString("Waiting for players... " + std::to_string(totalPlayers) + " in lobby");
        }
        // Non-admin players keep the static "Waiting for room host" text
    }
}
