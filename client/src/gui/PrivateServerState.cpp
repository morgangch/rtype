/**
 * @file PrivateServerState.cpp
 * @brief Implementation of the private server connection state for the R-Type client GUI
 * 
 * This file contains the implementation of the PrivateServerState class, which handles
 * the private server connection interface. Users can either join an existing private
 * server using a 4-digit code or create a new private server as an admin.
 * 
 * Key features:
 * - Server code input validation (4-digit numbers 1000-9999)
 * - Join existing private server functionality
 * - Create new private server with random code generation
 * - Text input handling with cursor animation
 * - Real-time input validation and feedback
 * 
 * The state transitions to PrivateServerLobbyState upon successful server
 * connection or creation.
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/PrivateServerState.h"
#include "gui/PrivateServerLobbyState.h"
#include "gui/MainMenuState.h"
#include "gui/NetworkManager.h"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    PrivateServerState::PrivateServerState(StateManager& stateManager, const std::string& username)
        : stateManager(stateManager), username(username), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        setupUI();
    }
    
    void PrivateServerState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Title setup
        titleText.setFont(font);
        titleText.setString("Private Servers");
        titleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 16);
        titleText.setFillColor(GUIHelper::Colors::TEXT);
        titleText.setStyle(sf::Text::Bold);
        
        // Server code input setup
        serverCodeBox.setFillColor(GUIHelper::Colors::INPUT_BOX);
        serverCodeBox.setOutlineColor(GUIHelper::Colors::TEXT);
        serverCodeBox.setOutlineThickness(2.0f);
        
        serverCodeText.setFont(font);
        serverCodeText.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        serverCodeText.setFillColor(GUIHelper::Colors::TEXT);
        
        serverCodeHintText.setFont(font);
        serverCodeHintText.setString("Enter server code");
        serverCodeHintText.setCharacterSize(GUIHelper::Sizes::HINT_FONT_SIZE);
        serverCodeHintText.setFillColor(GUIHelper::Colors::HINT_TEXT);
        
        // Button setup using GUIHelper
        GUIHelper::setupButton(joinButton, joinButtonRect, "Join", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        joinButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
        
        GUIHelper::setupButton(createButton, createButtonRect, "Create", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        createButtonRect.setFillColor(sf::Color(50, 50, 100, 200));
        
        GUIHelper::setupReturnButton(returnButton, returnButtonRect);
    }
    
    void PrivateServerState::onEnter() {
        std::cout << "Entered Private Server state with username: " << username << std::endl;
    }
    
    void PrivateServerState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning
        GUIHelper::centerText(titleText, centerX, windowSize.y * 0.2f);
        
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
        float buttonWidth = 300.0f;
        float buttonHeight = 80.0f;
        float buttonSpacing = 20.0f;
        float buttonY = centerY + 80.0f;
        
        // Join button
        joinButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        joinButtonRect.setPosition(centerX - buttonWidth - buttonSpacing / 2, buttonY);
        GUIHelper::centerText(joinButton,
                  joinButtonRect.getPosition().x + buttonWidth / 2,
                  joinButtonRect.getPosition().y + buttonHeight / 2);
        
        // Create button
        createButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        createButtonRect.setPosition(centerX + buttonSpacing / 2, buttonY);
        GUIHelper::centerText(createButton,
                  createButtonRect.getPosition().x + buttonWidth / 2,
                  createButtonRect.getPosition().y + buttonHeight / 2);
        
        // Return button positioning (top left)
        float returnButtonWidth = 150.0f;
        float returnButtonHeight = 50.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        GUIHelper::centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
    }
    
    void PrivateServerState::handleEvent(const sf::Event& event) {
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
            case sf::Event::TextEntered:
                handleTextInputEvent(event);
                break;
            case sf::Event::MouseMoved:
                handleMouseMoveEvent(event);
                break;
            default:
                break;
        }
    }
    
    void PrivateServerState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void PrivateServerState::handleKeyboardEvent(const sf::Event& event) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
        }
    }
    
    void PrivateServerState::handleMouseButtonEvent(const sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check server code box click
            if (GUIHelper::isPointInRect(mousePos, serverCodeBox)) {
                isTyping = true;
                serverCodeBox.setOutlineColor(sf::Color::Cyan);
            }
            // Check join button click
            else if (GUIHelper::isPointInRect(mousePos, joinButtonRect)) {
                joinServer();
            }
            // Check create button click
            else if (GUIHelper::isPointInRect(mousePos, createButtonRect)) {
                createServer();
            }
            // Check return button click
            else if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
            // Click outside - stop typing
            else {
                isTyping = false;
                serverCodeBox.setOutlineColor(sf::Color::White);
            }
        }
    }
    
    void PrivateServerState::handleTextInputEvent(const sf::Event& event) {
        if (isTyping) {
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
    }
    
    void PrivateServerState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        // Button hover effects using GUIHelper
        GUIHelper::applyButtonHover(joinButtonRect, joinButton, 
                                  GUIHelper::isPointInRect(mousePos, joinButtonRect),
                                  sf::Color(50, 100, 50, 200), sf::Color(70, 150, 70, 200));
        
        GUIHelper::applyButtonHover(createButtonRect, createButton, 
                                  GUIHelper::isPointInRect(mousePos, createButtonRect),
                                  sf::Color(50, 50, 100, 200), sf::Color(70, 70, 150, 200));
        
        GUIHelper::applyButtonHover(returnButtonRect, returnButton, 
                                  GUIHelper::isPointInRect(mousePos, returnButtonRect),
                                  GUIHelper::Colors::RETURN_BUTTON, sf::Color(150, 70, 70, 200));
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
        if (GUIHelper::isValidServerCode(serverCode)) {
            std::cout << "Joining private server with code: " << serverCode << std::endl;
            
            // Convert server code to room ID (assuming server code maps to room ID)
            uint32_t roomId = static_cast<uint32_t>(std::stoi(serverCode));
            
            // Connect to the private server using the NetworkManager
            bool connected = stateManager.getNetworkManager().connectToServer("127.0.0.1", 8080, username, roomId);
            
            if (connected) {
                std::cout << "Successfully connected to private server!" << std::endl;
                // Switch to private lobby state as a regular player
                stateManager.changeState(std::make_unique<PrivateServerLobbyState>(stateManager, username, serverCode, false));
            } else {
                std::cout << "Failed to connect to private server!" << std::endl;
                // Could show an error message to the user
            }
        } else {
            std::cout << "Invalid server code. Please enter a 4-digit number between 1000-9999." << std::endl;
        }
    }
    
    void PrivateServerState::createServer() {
        // Generate random server code between 1000-9999
        int randomCode = 1000 + (rand() % 9000);
        std::string generatedCode = std::to_string(randomCode);
        uint32_t roomId = static_cast<uint32_t>(randomCode);
        
        std::cout << "Creating new private server with code: " << generatedCode << std::endl;
        
        // Connect to the server as admin to create the room
        bool connected = stateManager.getNetworkManager().connectToServer("127.0.0.1", 8080, username, roomId);
        
        if (connected) {
            std::cout << "Successfully created private server!" << std::endl;
            // Switch to private lobby state as the admin
            stateManager.changeState(std::make_unique<PrivateServerLobbyState>(stateManager, username, generatedCode, true));
        } else {
            std::cout << "Failed to create private server!" << std::endl;
            // Could show an error message to the user
        }
    }
    
    void PrivateServerState::onExit() {
        std::cout << "Exiting Private Server state" << std::endl;
        // Note: We don't disconnect here because we might be transitioning to the lobby
        // The lobby will manage the connection
    }
    
}
