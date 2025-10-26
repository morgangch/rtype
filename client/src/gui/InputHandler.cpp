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
#include "network/senders.h"
#include <iostream>

namespace rtype::client::gui {

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

void GameState::handleKeyPressed(sf::Keyboard::Key key) {
    // Get keybinds from config
    sf::Keyboard::Key upKey = m_config.getKeybind("up");
    sf::Keyboard::Key downKey = m_config.getKeybind("down");
    sf::Keyboard::Key leftKey = m_config.getKeybind("left");
    sf::Keyboard::Key rightKey = m_config.getKeybind("right");
    sf::Keyboard::Key shootKey = m_config.getKeybind("shoot");
    
    // Check if key matches configured keybinds
    if (key == upKey) {
        m_keyUp = true;
    } else if (key == downKey) {
        m_keyDown = true;
    } else if (key == leftKey) {
        m_keyLeft = true;
    } else if (key == rightKey) {
        m_keyRight = true;
    } else if (key == shootKey) {
        // Fire key - Start charging
        m_keyFire = true;
        auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
        if (players) {
            for (auto& [entity, playerPtr] : *players) {
                auto* chargedShot = m_world.GetComponent<rtype::common::components::ChargedShot>(entity);
                if (chargedShot) {
                    chargedShot->startCharge();
                }
            }
        }
    } else {
        // Handle non-configurable/reserved keys
        // WARNING: These keys are RESERVED and cannot be assigned to game actions in settings:
        // - Escape: Menu navigation and pause
        // - B: Admin command (boss spawn)
        // If adding new reserved keys, update SettingsState.cpp keybind validation
        switch (key) {
            // Admin-only: Spawn boss with B key (RESERVED)
            case sf::Keyboard::B:
                if (m_isAdmin) {
                    std::cout << "CLIENT: Admin requesting boss spawn (B key pressed)" << std::endl;
                    rtype::client::network::senders::send_spawn_boss_request();
                } else {
                    std::cout << "CLIENT: Non-admin player cannot spawn boss (B key ignored)" << std::endl;
                }
                break;
            
            // Pause/Menu key (RESERVED)
            case sf::Keyboard::Escape:
                showInGameMenu(false); // Pause game
                break;
            
            default:
                break;
        }
    }
}

void GameState::handleKeyReleased(sf::Keyboard::Key key) {
    // Get keybinds from config
    sf::Keyboard::Key upKey = m_config.getKeybind("up");
    sf::Keyboard::Key downKey = m_config.getKeybind("down");
    sf::Keyboard::Key leftKey = m_config.getKeybind("left");
    sf::Keyboard::Key rightKey = m_config.getKeybind("right");
    sf::Keyboard::Key shootKey = m_config.getKeybind("shoot");
    
    // Check if key matches configured keybinds
    if (key == upKey) {
        m_keyUp = false;
    } else if (key == downKey) {
        m_keyDown = false;
    } else if (key == leftKey) {
        m_keyLeft = false;
    } else if (key == rightKey) {
        m_keyRight = false;
    } else if (key == shootKey) {
        // Fire key - Release charged shot
        m_keyFire = false;
        // Fire charged shot if applicable
        auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
        if (players) {
            for (auto& [entity, playerPtr] : *players) {
                auto* chargedShot = m_world.GetComponent<rtype::common::components::ChargedShot>(entity);
                auto* pos = m_world.GetComponent<rtype::common::components::Position>(entity);
                auto* fireRate = m_world.GetComponent<rtype::common::components::FireRate>(entity);
                
                if (chargedShot && pos && fireRate && chargedShot->isCharging) {
                    // Release charge and get if it was fully charged
                    bool wasFullyCharged = chargedShot->release();
                    
                    if (fireRate->canFire()) {
                        // Send shoot request to server with charged state and current position
                        rtype::client::network::senders::send_player_shoot(wasFullyCharged, pos->x, pos->y);
                        
                        // Play appropriate sound for immediate feedback
                        if (wasFullyCharged) {
                            // Play charged shoot sound if available
                            if (m_soundManager.has(AudioFactory::SfxId::ChargedShoot)) {
                                m_soundManager.play(AudioFactory::SfxId::ChargedShoot);
                            }
                        } else {
                            // Play regular shoot sound if available
                            if (m_soundManager.has(AudioFactory::SfxId::Shoot)) {
                                m_soundManager.play(AudioFactory::SfxId::Shoot);
                            }
                        }
                        fireRate->shoot();
                    }
                }
            }
        }
    }
}

} // namespace rtype::client::gui
