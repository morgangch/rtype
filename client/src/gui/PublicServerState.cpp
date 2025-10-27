/**
 * @file PublicServerState.cpp
 * @brief Implementation of the public server lobby state for the R-Type client GUI
 * 
 * This file contains the implementation of the PublicServerState class, which handles
 * the public server lobby interface where players can mark themselves as ready
 * for matchmaking. The state provides a simple ready/not ready toggle system
 * for quick game matching.
 * 
 * Key features:
 * - Ready state toggle for players
 * - Real-time player count display
 * - Simple lobby interface for public matchmaking
 * - Automatic game start when sufficient players are ready
 * - Network integration for player state synchronization
 * 
 * This state is designed for casual play where players don't need to coordinate
 * private server codes and can quickly join available games.
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/PublicServerState.h"
#include "gui/MainMenuState.h"
#include "gui/ParallaxSystem.h"
#include "gui/GameState.h"
#include <iostream>
#include <cstdlib>
#include <string>

#include "network/network.h"

namespace rtype::client::gui {
    PublicServerState::~PublicServerState() = default;

    PublicServerState::PublicServerState(StateManager& stateManager, const std::string& username)
        : stateManager(stateManager), username(username), isReady(false), playersReady(0) {
        setupUI();
        m_overlay.setFillColor(sf::Color(0,0,0,150));
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
        std::cout << "Connecting to public server..." << std::endl;
        
        // Connect to the public server using the NetworkManager
        // Public server uses room ID 0 by convention
// network::start_room_connection("127.0.0.1", 8080, username, 0);
    }
    
    void PublicServerState::onExit() {
        std::cout << "Exiting Public Server state" << std::endl;
        // TODO: send a disconnect packet.
    }
    
    void PublicServerState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Players ready text positioning (center)
        GUIHelper::centerText(playersReadyText, centerX, centerY - 50.0f);
        
        // Ready button positioning (below the text)
        float buttonWidth = 300.0f;
        float buttonHeight = 80.0f;
        float buttonY = centerY + 50.0f;
        
        readyButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        readyButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
        GUIHelper::centerText(readyButton,
                  readyButtonRect.getPosition().x + buttonWidth / 2,
                  readyButtonRect.getPosition().y + buttonHeight / 2);
        
        // Return button positioning (top left)
        float returnButtonWidth = 150.0f;
        float returnButtonHeight = 50.0f;
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(20.0f, 20.0f);
        GUIHelper::centerText(returnButton,
                  returnButtonRect.getPosition().x + returnButtonWidth / 2,
                  returnButtonRect.getPosition().y + returnButtonHeight / 2);
    }
    
    void PublicServerState::handleEvent(const sf::Event& event) {
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
    
    void PublicServerState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void PublicServerState::handleKeyboardEvent(const sf::Event& event) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Go back to main menu
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
        }
    }
    
    void PublicServerState::handleMouseButtonEvent(const sf::Event& event) {
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
    
    void PublicServerState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        // Button hover effects using GUIHelper
        GUIHelper::applyButtonHover(readyButtonRect, readyButton, 
                                  GUIHelper::isPointInRect(mousePos, readyButtonRect),
                                  GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
        
        GUIHelper::applyButtonHover(returnButtonRect, returnButton, 
                                  GUIHelper::isPointInRect(mousePos, returnButtonRect),
                                  GUIHelper::Colors::RETURN_BUTTON, sf::Color(150, 70, 70, 200));
    }
    
    void PublicServerState::update(float deltaTime) {
        // Update parallax if present
        if (m_parallaxSystem) {
            m_parallaxSystem->update(deltaTime);
        }
    }
    
    void PublicServerState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        // Ensure parallax exists and is sized to window
        ensureParallaxInitialized(window);

        if (m_parallaxSystem) {
            m_parallaxSystem->render(window);
        }

        // Draw overlay
        window.draw(m_overlay);
        
        // Render players ready text
        window.draw(playersReadyText);
        
        // Render ready button
        window.draw(readyButtonRect);
        window.draw(readyButton);
        
        // Render return button
        window.draw(returnButtonRect);
        window.draw(returnButton);
    }

    void PublicServerState::ensureParallaxInitialized(const sf::RenderWindow& window) {
        if (m_parallaxInitialized) return;
        m_parallaxSystem = std::make_unique<ParallaxSystem>(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        );
        // Select theme from current GameState if available
        if (g_gameState) {
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(g_gameState->getLevelIndex()), true);
        } else {
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(0), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
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
        playersReadyText.setString("Amount of players ready " + std::to_string(playersReady));
        
        // Update button text based on ready state
        if (isReady) {
            readyButton.setString("Ready");
        } else {
            readyButton.setString("Not ready");
        }
    }
}
