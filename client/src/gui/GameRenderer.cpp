/**
 * @file GameRenderer.cpp
 * @brief Rendering systems for game visualization
 * 
 * This file contains all rendering logic:
 * - Entity rendering (player, enemies, projectiles)
 * - HUD rendering (lives/hearts)
 * - Game over menu rendering
 * 
 * Part of the modular GameState implementation.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "gui/GUIHelper.h"
#include <sstream>
#include <cmath>

namespace rtype::client::gui {

void GameState::renderEntities(sf::RenderWindow& window) {
    auto* positions = m_world.GetAllComponents<rtype::common::components::Position>();
    if (!positions) return;
    
    for (auto& [entity, posPtr] : *positions) {
        auto& pos = *posPtr;
        auto* sprite = m_world.GetComponent<rtype::client::components::Sprite>(entity);
        
        if (!sprite || !sprite->visible) continue;
        
        // Check invulnerability for blinking effect (uses Health.invulnerable)
        auto* health = m_world.GetComponent<rtype::common::components::Health>(entity);
        if (health && health->invulnerable) {
            // Blink effect: visible/invisible alternation
            constexpr float BLINK_FREQUENCY = 8.0f;
            float phase = std::fmod(health->invulnerabilityTimer * BLINK_FREQUENCY, 1.0f);
            if (phase >= 0.5f) {
                continue; // Skip rendering for blink effect
            }
        }
        
        // Draw entity with texture or colored rectangle
        if (sprite->useTexture) {
            // Load texture if not already loaded
            if (!sprite->textureLoaded) {
                if (!sprite->texture.loadFromFile(sprite->texturePath)) {
                    // Failed to load - fallback to colored shape
                    sprite->useTexture = false;
                    sprite->textureLoaded = false;
                } else {
                    // Texture loaded successfully - configure sprite
                    sprite->sprite.setTexture(sprite->texture);
                    
                    // Set texture rect (use full texture if not specified)
                    if (sprite->textureRect.width == 0 || sprite->textureRect.height == 0) {
                        sprite->textureRect = sf::IntRect(0, 0, 
                            sprite->texture.getSize().x, sprite->texture.getSize().y);
                    }
                    sprite->sprite.setTextureRect(sprite->textureRect);
                    sprite->sprite.setOrigin(sprite->textureRect.width / 2.0f, 
                                            sprite->textureRect.height / 2.0f);
                    sprite->sprite.setScale(sprite->scale, sprite->scale);
                    sprite->textureLoaded = true;
                }
            }
            
            // Draw sprite with texture (if loaded successfully)
            if (sprite->useTexture) {
                // Update texture rect (in case animation changed it)
                sprite->sprite.setTextureRect(sprite->textureRect);
                sprite->sprite.setPosition(pos.x, pos.y);
                window.draw(sprite->sprite);
            }
        }
        
        // Draw as colored rectangle if no texture or texture loading failed
        if (!sprite->useTexture) {
            sf::RectangleShape shape(sprite->size);
            shape.setPosition(pos.x - sprite->size.x * 0.5f, 
                             pos.y - sprite->size.y * 0.5f);
            shape.setFillColor(sprite->color);
            window.draw(shape);
        }
    }
}

void GameState::renderHUD(sf::RenderWindow& window) {
    // Get player lives
    int lives = getPlayerLives();
    
    // Draw hearts for lives
    const float heartSize = 20.0f;
    const float heartSpacing = 25.0f;
    const float heartY = 20.0f;
    
    for (int i = 0; i < lives; ++i) {
        sf::CircleShape heart(heartSize * 0.5f);
        heart.setFillColor(sf::Color::Red);
        heart.setPosition(20.0f + i * heartSpacing, heartY);
        window.draw(heart);
    }
}

void GameState::renderGameOverMenu(sf::RenderWindow& window) {
    // Semi-transparent overlay
    sf::RectangleShape overlay(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 180)); // Dark overlay
    window.draw(overlay);
    
    // Title positioning
    sf::FloatRect titleBounds = m_gameOverTitleText.getLocalBounds();
    m_gameOverTitleText.setPosition(
        (SCREEN_WIDTH - titleBounds.width) * 0.5f,
        150.0f
    );
    window.draw(m_gameOverTitleText);
    
    // Button dimensions
    const float buttonWidth = 300.0f;
    const float buttonHeight = 60.0f;
    const float buttonX = (SCREEN_WIDTH - buttonWidth) * 0.5f;
    const float button1Y = 340.0f;
    const float button2Y = 420.0f;
    
    // Restart/Resume button
    sf::RectangleShape restartButton(sf::Vector2f(buttonWidth, buttonHeight));
    restartButton.setPosition(buttonX, button1Y);
    restartButton.setFillColor(m_selectedMenuOption == 0 ? 
                               GUIHelper::Colors::BUTTON_HOVER : 
                               GUIHelper::Colors::BUTTON_NORMAL);
    restartButton.setOutlineColor(GUIHelper::Colors::TEXT);
    restartButton.setOutlineThickness(2.0f);
    window.draw(restartButton);
    
    // Restart text
    if (m_isGameOver) {
        m_restartText.setString("Restart");
    } else {
        m_restartText.setString("Resume");
    }
    sf::FloatRect restartBounds = m_restartText.getLocalBounds();
    m_restartText.setPosition(
        buttonX + (buttonWidth - restartBounds.width) * 0.5f,
        button1Y + (buttonHeight - restartBounds.height) * 0.5f - 5.0f
    );
    window.draw(m_restartText);
    
    // Menu button
    sf::RectangleShape menuButton(sf::Vector2f(buttonWidth, buttonHeight));
    menuButton.setPosition(buttonX, button2Y);
    menuButton.setFillColor(m_selectedMenuOption == 1 ? 
                            GUIHelper::Colors::BUTTON_HOVER : 
                            GUIHelper::Colors::BUTTON_NORMAL);
    menuButton.setOutlineColor(GUIHelper::Colors::TEXT);
    menuButton.setOutlineThickness(2.0f);
    window.draw(menuButton);
    
    // Menu text
    sf::FloatRect menuBounds = m_menuText.getLocalBounds();
    m_menuText.setPosition(
        buttonX + (buttonWidth - menuBounds.width) * 0.5f,
        button2Y + (buttonHeight - menuBounds.height) * 0.5f - 5.0f
    );
    window.draw(m_menuText);
}

} // namespace rtype::client::gui
