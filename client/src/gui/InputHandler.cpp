/**
 * @file InputHandler.cpp
 * @brief Input event processing for gameplay and menus
 * 
 * This file contains all input event processing:
 * - Keyboard input (gameplay and menu navigation)
 * - Mouse input (menu interaction and hover effects)
 * - Key press/release state management
 * 
 * Part of the modular GameState implementation.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "gui/MainMenuState.h"

namespace rtype::client::gui {

// =============================================================================
// EVENT HANDLING
// =============================================================================

void GameState::handleEvent(const sf::Event& event) {
    // Handle in-game menu (pause or game over)
    if (m_gameStatus == GameStatus::InGameMenu) {
        handleMenuInput(event);
        return;
    }
    
    // Handle gameplay input
    if (event.type == sf::Event::KeyPressed) {
        handleKeyPressed(event.key.code);
    } else if (event.type == sf::Event::KeyReleased) {
        handleKeyReleased(event.key.code);
    }
}

// =============================================================================
// MENU INPUT HANDLING
// =============================================================================

void GameState::handleMenuInput(const sf::Event& event) {
    // Keyboard navigation
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Z) {
            m_selectedMenuOption = 0;
        } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            m_selectedMenuOption = 1;
        } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
            if (m_selectedMenuOption == 0) {
                // Resume/Restart game
                if (m_isGameOver) {
                    resetGame();
                }
                resumeGame();
            } else {
                // Return to main menu
                m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
            }
        } else if (event.key.code == sf::Keyboard::Escape && !m_isGameOver) {
            // ESC to resume (only if paused, not game over)
            resumeGame();
        }
    }
    
    // Mouse hover detection
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
                             static_cast<float>(event.mouseMove.y));
        
        // Button dimensions (same as in renderGameOverMenu)
        const float buttonWidth = 300.0f;
        const float buttonHeight = 60.0f;
        const float buttonX = (SCREEN_WIDTH - buttonWidth) * 0.5f;
        const float button1Y = 340.0f;
        const float button2Y = 420.0f;
        
        sf::FloatRect restartButton(buttonX, button1Y, buttonWidth, buttonHeight);
        sf::FloatRect menuButton(buttonX, button2Y, buttonWidth, buttonHeight);
        
        if (restartButton.contains(mousePos)) {
            m_selectedMenuOption = 0;
        } else if (menuButton.contains(mousePos)) {
            m_selectedMenuOption = 1;
        }
    }
    
    // Mouse interaction
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                             static_cast<float>(event.mouseButton.y));
        
        // Button dimensions (same as in renderGameOverMenu)
        const float buttonWidth = 300.0f;
        const float buttonHeight = 60.0f;
        const float buttonX = (SCREEN_WIDTH - buttonWidth) * 0.5f;
        const float button1Y = 340.0f;
        const float button2Y = 420.0f;
        
        sf::FloatRect restartButton(buttonX, button1Y, buttonWidth, buttonHeight);
        sf::FloatRect menuButton(buttonX, button2Y, buttonWidth, buttonHeight);
        
        if (restartButton.contains(mousePos)) {
            if (m_isGameOver) {
                resetGame();
            }
            resumeGame();
        } else if (menuButton.contains(mousePos)) {
            m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
        }
    }
}

// =============================================================================
// GAMEPLAY KEY PRESS/RELEASE
// =============================================================================

void GameState::handleKeyPressed(sf::Keyboard::Key key) {
    switch (key) {
        // Movement keys
        case sf::Keyboard::Z:
        case sf::Keyboard::Up:
            m_keyUp = true;
            break;
        
        case sf::Keyboard::S:
        case sf::Keyboard::Down:
            m_keyDown = true;
            break;
        
        case sf::Keyboard::Q:
        case sf::Keyboard::Left:
            m_keyLeft = true;
            break;
        
        case sf::Keyboard::D:
        case sf::Keyboard::Right:
            m_keyRight = true;
            break;
        
        // Fire key - Start charging
        case sf::Keyboard::Space:
            m_keyFire = true;
            // Start charging for player
            {
                auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
                if (players) {
                    for (auto& [entity, playerPtr] : *players) {
                        auto* chargedShot = m_world.GetComponent<rtype::common::components::ChargedShot>(entity);
                        if (chargedShot) {
                            chargedShot->startCharge();
                        }
                    }
                }
            }
            break;
        
        // DEBUG: Spawn boss with B key
        case sf::Keyboard::B:
            if (!isBossActive()) {
                createBoss(SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT * 0.5f);
            }
            break;
        
        // Pause/Menu key
        case sf::Keyboard::Escape:
            showInGameMenu(false); // Pause game
            break;
        
        default:
            break;
    }
}

void GameState::handleKeyReleased(sf::Keyboard::Key key) {
    switch (key) {
        // Movement keys
        case sf::Keyboard::Z:
        case sf::Keyboard::Up:
            m_keyUp = false;
            break;
        
        case sf::Keyboard::S:
        case sf::Keyboard::Down:
            m_keyDown = false;
            break;
        
        case sf::Keyboard::Q:
        case sf::Keyboard::Left:
            m_keyLeft = false;
            break;
        
        case sf::Keyboard::D:
        case sf::Keyboard::Right:
            m_keyRight = false;
            break;
        
        // Fire key - Release charged shot
        case sf::Keyboard::Space:
            m_keyFire = false;
            // Fire charged shot if applicable
            {
                auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
                if (players) {
                    for (auto& [entity, playerPtr] : *players) {
                        auto* chargedShot = m_world.GetComponent<rtype::common::components::ChargedShot>(entity);
                        auto* pos = m_world.GetComponent<rtype::common::components::Position>(entity);
                        auto* fireRate = m_world.GetComponent<rtype::common::components::FireRate>(entity);
                        
                        if (chargedShot && pos && fireRate && chargedShot->isCharging) {
                            // Fire charged shot if fully charged, otherwise normal shot
                            if (chargedShot->isFullyCharged && fireRate->canFire()) {
                                createChargedProjectile(pos->x + 32.0f, pos->y);
                                fireRate->shoot();
                            } else if (fireRate->canFire()) {
                                // Normal shot if not fully charged
                                createPlayerProjectile(pos->x + 32.0f, pos->y);
                                fireRate->shoot();
                            }
                            chargedShot->release();
                        }
                    }
                }
            }
            break;
        
        default:
            break;
    }
}

} // namespace rtype::client::gui
