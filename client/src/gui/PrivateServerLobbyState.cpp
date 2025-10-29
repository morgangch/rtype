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
#include "gui/ParallaxSystem.h"
#include "network/senders.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include "gui/AssetPaths.h"

// External declaration of global player server ID set by JOIN_ROOM_ACCEPTED handler
extern uint32_t g_playerServerId;

namespace rtype::client::gui {
    // Global pointer to current lobby state (for network callbacks)
    PrivateServerLobbyState* g_lobbyState = nullptr;
    PrivateServerLobbyState::~PrivateServerLobbyState() = default;

    PrivateServerLobbyState::PrivateServerLobbyState(StateManager& stateManager, const std::string& username, 
                                                     const std::string& serverCode, bool isAdmin)
        : stateManager(stateManager), username(username), serverCode(serverCode), isAdmin(isAdmin) {
        setupUI();
        m_overlay.setFillColor(sf::Color(0,0,0,150));
        g_lobbyState = this; // Register this instance globally for network callbacks
    }
    
    void PrivateServerLobbyState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Waiting text setup (for non-admin players)
        playersWaitingText.setFont(font);
        playersWaitingText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 28);
        playersWaitingText.setFillColor(GUIHelper::Colors::TEXT);
        
        // Action button sprite (admin only)
        if (isAdmin) {
            actionSpriteLoaded = actionTexture.loadFromFile(rtype::client::assets::ui::READY_BUTTON);
            if (!actionSpriteLoaded) {
                GUIHelper::setupButton(actionButton, actionButtonRect, "", GUIHelper::Sizes::BUTTON_FONT_SIZE);
            } else {
                actionTexture.setSmooth(true);
                actionSprite.setTexture(actionTexture);
                sf::Vector2u sz = actionTexture.getSize();
                actionSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
            }
        }
        
        // Return button sprite
        returnSpriteLoaded = returnTexture.loadFromFile(rtype::client::assets::ui::RETURN_BUTTON);
        if (returnSpriteLoaded) {
            returnTexture.setSmooth(true);
            returnSprite.setTexture(returnTexture);
            sf::Vector2u sz = returnTexture.getSize();
            returnSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
        } else {
            GUIHelper::setupReturnButton(returnButton, returnButtonRect);
        }
        
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
            float buttonWidth = 520.0f;
            float buttonHeight = 180.0f;
            float buttonY = centerY + 50.0f;
            if (actionSpriteLoaded) {
                sf::Vector2u tex = actionTexture.getSize();
                if (tex.x > 0 && tex.y > 0) {
                    float scale = std::min(buttonWidth / static_cast<float>(tex.x),
                                           buttonHeight / static_cast<float>(tex.y));
                    actionSprite.setScale(scale, scale);
                    float scaledW = static_cast<float>(tex.x) * scale;
                    float scaledH = static_cast<float>(tex.y) * scale;
                    actionSprite.setPosition(centerX, buttonY + scaledH * 0.5f);
                    actionButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
                    actionButtonRect.setPosition(centerX - scaledW * 0.5f, buttonY);
                }
            } else {
                actionButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
                actionButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
            }
        }
        
    // Return button positioning (top left)
    float returnButtonWidth = 300.0f;
    float returnButtonHeight = 120.0f;
        float leftMargin = 8.0f;
        float topMargin = 10.0f;
        if (returnSpriteLoaded) {
            sf::Vector2u tex = returnTexture.getSize();
            if (tex.x > 0 && tex.y > 0) {
                float scale = std::min(returnButtonWidth / static_cast<float>(tex.x),
                                       returnButtonHeight / static_cast<float>(tex.y));
                returnSprite.setScale(scale, scale);
                float scaledW = static_cast<float>(tex.x) * scale;
                float scaledH = static_cast<float>(tex.y) * scale;
                returnSprite.setPosition(leftMargin + scaledW * 0.5f, topMargin + scaledH * 0.5f);
                returnButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
                returnButtonRect.setPosition(leftMargin, topMargin);
            }
        } else {
            returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
            returnButtonRect.setPosition(leftMargin, topMargin);
            GUIHelper::centerText(returnButton,
                      returnButtonRect.getPosition().x + returnButtonWidth / 2,
                      returnButtonRect.getPosition().y + returnButtonHeight / 2);
        }
        
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
            case sf::Event::MouseButtonReleased:
                if (isAdmin) actionPressed = false;
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
                actionPressed = true;
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
            actionHovered = GUIHelper::isPointInRect(mousePos, actionButtonRect);
        }
        
    // Return hover flag only (visuals applied at render)
    returnHovered = GUIHelper::isPointInRect(mousePos, returnButtonRect);
    }
    
    void PrivateServerLobbyState::update(float deltaTime) {
        // Update parallax if present
        if (m_parallaxSystem) {
            m_parallaxSystem->update(deltaTime);
        }
    }
    
    void PrivateServerLobbyState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        // Ensure and render parallax behind UI
        ensureParallaxInitialized(window);
        if (m_parallaxSystem) {
            m_parallaxSystem->render(window);
        }
        window.draw(m_overlay);
        
        // Render waiting text
        window.draw(playersWaitingText);
        
        // Render action button (admin only)
        if (isAdmin) {
            if (actionSpriteLoaded) {
                if (actionPressed) {
                    sf::Sprite glow = actionSprite;
                    sf::Vector2f os = glow.getScale();
                    glow.setScale(os.x * 1.12f, os.y * 1.12f);
                    glow.setColor(sf::Color(255, 255, 0, 120));
                    sf::RenderStates states;
                    states.blendMode = sf::BlendAdd;
                    window.draw(glow, states);
                }
                window.draw(actionSprite);
            } else {
                // Fallback: do not draw text per requirement
            }
        }
        
        // Render return button
        if (returnSpriteLoaded) {
            sf::Vector2f originalScale = returnSprite.getScale();
            if (returnHovered) returnSprite.setScale(originalScale.x * 0.94f, originalScale.y * 0.94f);
            window.draw(returnSprite);
            if (returnHovered) returnSprite.setScale(originalScale);
        } else {
            window.draw(returnButtonRect);
            window.draw(returnButton);
        }
        
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

    void PrivateServerLobbyState::ensureParallaxInitialized(const sf::RenderWindow& window) {
        if (m_parallaxInitialized) return;
        m_parallaxSystem = std::make_unique<ParallaxSystem>(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        );
        if (g_gameState) {
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(g_gameState->getLevelIndex()), true);
        } else {
            // Use persisted last level when returning from a session
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
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
