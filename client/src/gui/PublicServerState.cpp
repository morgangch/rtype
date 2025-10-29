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
#include "gui/AssetPaths.h"

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
        
        // Ready button sprite (replaces text button)
        readySpriteLoaded = readyTexture.loadFromFile(rtype::client::assets::ui::READY_BUTTON);
        if (!readySpriteLoaded) {
            // Fallback to simple rect for clicks; text remains empty and won't be drawn
            GUIHelper::setupButton(readyButton, readyButtonRect, "", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        } else {
            readyTexture.setSmooth(true);
            readySprite.setTexture(readyTexture);
            sf::Vector2u sz = readyTexture.getSize();
            readySprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
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
        
        updatePlayersReadyText();
    }
    
    void PublicServerState::onEnter() {
        std::cout << "Entered Public Server state with username: " << username << std::endl;
        std::cout << "Connecting to public server..." << std::endl;
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
        
    // Ready button positioning (below the text) using sprite
    float buttonWidth = 520.0f;
    float buttonHeight = 180.0f;
        float buttonY = centerY + 50.0f;
        
        if (readySpriteLoaded) {
            sf::Vector2u tex = readyTexture.getSize();
            if (tex.x > 0 && tex.y > 0) {
                float scale = std::min(buttonWidth / static_cast<float>(tex.x),
                                       buttonHeight / static_cast<float>(tex.y));
                readySprite.setScale(scale, scale);
                float scaledW = static_cast<float>(tex.x) * scale;
                float scaledH = static_cast<float>(tex.y) * scale;
                readySprite.setPosition(centerX, buttonY + scaledH * 0.5f);
                // Clickable rect matches the sprite bounds
                readyButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
                readyButtonRect.setPosition(centerX - scaledW * 0.5f, buttonY);
            }
        } else {
            readyButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            readyButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
            // Text hidden; no need to center
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
        
    // Hover detection for ready sprite (visual applied in render if needed)
    readyHovered = GUIHelper::isPointInRect(mousePos, readyButtonRect);
        
    // Return hover flag only (visuals applied at render)
    returnHovered = GUIHelper::isPointInRect(mousePos, returnButtonRect);
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
        
        // Render ready button (sprite)
        if (readySpriteLoaded) {
            // Glow effect when pressed (ready state)
            if (isReady) {
                sf::Sprite glow = readySprite;
                sf::Vector2f os = glow.getScale();
                glow.setScale(os.x * 1.12f, os.y * 1.12f);
                glow.setColor(sf::Color(0, 255, 255, 120));
                sf::RenderStates states;
                states.blendMode = sf::BlendAdd;
                window.draw(glow, states);
            }
            // Draw the base sprite
            window.draw(readySprite);
        } else {
            // Fallback: invisible rect still clickable; not drawing text per requirement
            // window.draw(readyButtonRect); // intentionally not drawn
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
            // Use last known level index from StateManager to keep menu visuals consistent
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
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
            // Use last known level index from StateManager to keep menu visuals consistent
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
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
            // Use last known level index from StateManager to keep menu visuals consistent
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
    }
    
    void PublicServerState::toggleReady() {
        isReady = !isReady;
        
        if (isReady) {
            playersReady++;
            // Visual handled by sprite glow; text removed
            std::cout << username << " is now ready!" << std::endl;
        } else {
            playersReady--;
            // Visual handled by absence of glow; text removed
            std::cout << username << " is no longer ready!" << std::endl;
        }
        
        updatePlayersReadyText();
    }
    
    void PublicServerState::updatePlayersReadyText() {
        playersReadyText.setString("Amount of players ready " + std::to_string(playersReady));
    }
}
