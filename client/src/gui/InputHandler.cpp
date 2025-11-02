/**
 * @file InputHandler.cpp
 * @brief Input event processing for gameplay and menus
 * 
 * This file contains all input event processing:
 * - Keyboard input (gameplay and menu navigation)
 * - Mouse input (menu interaction and hover effects)
 * - Joystick/controller input (buttons and axes)
 * - Charged shot workflow helpers
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

// ============================================================================
// Helper Methods: Charged Shot Logic
// ============================================================================

void GameState::startChargedShot() {
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

void GameState::releaseChargedShot() {
    auto* players = m_world.GetAllComponents<rtype::common::components::Player>();
    if (!players) return;

    for (auto& [entity, playerPtr] : *players) {
        auto* chargedShot = m_world.GetComponent<rtype::common::components::ChargedShot>(entity);
        auto* pos = m_world.GetComponent<rtype::common::components::Position>(entity);
        auto* fireRate = m_world.GetComponent<rtype::common::components::FireRate>(entity);
        
        if (!chargedShot || !pos || !fireRate || !chargedShot->isCharging) {
            continue;
        }

        bool wasFullyCharged = chargedShot->release();
        if (!fireRate->canFire()) {
            continue;
        }

        rtype::client::network::senders::send_player_shoot(wasFullyCharged, pos->x, pos->y);

        if (wasFullyCharged && m_soundManager.has(AudioFactory::SfxId::ChargedShoot)) {
            m_soundManager.play(AudioFactory::SfxId::ChargedShoot);
        } else if (!wasFullyCharged && m_soundManager.has(AudioFactory::SfxId::Shoot)) {
            m_soundManager.play(AudioFactory::SfxId::Shoot);
        }

        fireRate->shoot();
    }
}

// ============================================================================
// Helper Methods: Joystick Input
// ============================================================================

void GameState::handleJoystickAxis(const sf::Event& event) {
    int axisIndex = static_cast<int>(event.joystickMove.axis);
    float pos = event.joystickMove.position; // -100..100
    const float DEADZONE = 20.0f;

    // Axis code encoding: 30000 + axisIndex*10 + dir(0=neg,1=pos)
    int axisNegCode = 30000 + axisIndex * 10 + 0;
    int axisPosCode = 30000 + axisIndex * 10 + 1;

    int upSec = m_config.getSecondaryKeybind("up");
    int downSec = m_config.getSecondaryKeybind("down");
    int leftSec = m_config.getSecondaryKeybind("left");
    int rightSec = m_config.getSecondaryKeybind("right");

    // Try matching axis codes to configured secondary bindings
    bool consumed = false;
    if (pos < -DEADZONE) {
        if (axisNegCode == upSec) { m_keyUp = true; consumed = true; }
        if (axisNegCode == leftSec) { m_keyLeft = true; consumed = true; }
    } else {
        if (axisNegCode == upSec) m_keyUp = false;
        if (axisNegCode == leftSec) m_keyLeft = false;
    }

    if (pos > DEADZONE) {
        if (axisPosCode == downSec) { m_keyDown = true; consumed = true; }
        if (axisPosCode == rightSec) { m_keyRight = true; consumed = true; }
    } else {
        if (axisPosCode == downSec) m_keyDown = false;
        if (axisPosCode == rightSec) m_keyRight = false;
    }

    // Fallback: use common axis mapping (X = horizontal, Y = vertical)
    if (!consumed) {
        if (event.joystickMove.axis == sf::Joystick::X) {
            m_keyLeft = (pos < -DEADZONE);
            m_keyRight = (pos > DEADZONE);
        } else if (event.joystickMove.axis == sf::Joystick::Y) {
            m_keyUp = (pos < -DEADZONE);
            m_keyDown = (pos > DEADZONE);
        }
    }
}

void GameState::handleJoystickButtonPressed(const sf::Event& event) {
    int btn = event.joystickButton.button;
    std::cout << "Joystick Button Pressed: Button " << btn << std::endl;
    
    int code = 10000 + btn; // joystick button encoding

    int upSec = m_config.getSecondaryKeybind("up");
    int downSec = m_config.getSecondaryKeybind("down");
    int leftSec = m_config.getSecondaryKeybind("left");
    int rightSec = m_config.getSecondaryKeybind("right");
    int shootSec = m_config.getSecondaryKeybind("shoot");

    if (code == upSec) m_keyUp = true;
    if (code == downSec) m_keyDown = true;
    if (code == leftSec) m_keyLeft = true;
    if (code == rightSec) m_keyRight = true;
    if (code == shootSec) {
        m_keyFire = true;
        startChargedShot();
    }

    // Fallback: button 9 always opens pause menu
    if (btn == 9) {
        showInGameMenu(false);
    }
}

void GameState::handleJoystickButtonReleased(const sf::Event& event) {
    int btn = event.joystickButton.button;
    std::cout << "Joystick Button Released: Button " << btn << std::endl;
    
    int code = 10000 + btn;

    int upSec = m_config.getSecondaryKeybind("up");
    int downSec = m_config.getSecondaryKeybind("down");
    int leftSec = m_config.getSecondaryKeybind("left");
    int rightSec = m_config.getSecondaryKeybind("right");
    int shootSec = m_config.getSecondaryKeybind("shoot");

    if (code == upSec) m_keyUp = false;
    if (code == downSec) m_keyDown = false;
    if (code == leftSec) m_keyLeft = false;
    if (code == rightSec) m_keyRight = false;
    if (code == shootSec) {
        m_keyFire = false;
        releaseChargedShot();
    }
}

// ============================================================================
// Helper Methods: Mouse Input
// ============================================================================

void GameState::handleMouseButtonPressed(const sf::Event& event) {
    if (event.mouseButton.button != sf::Mouse::Left) return;

    // Always allow left mouse as shoot (fallback behavior)
    m_keyFire = true;
    startChargedShot();
}

void GameState::handleMouseButtonReleased(const sf::Event& event) {
    if (event.mouseButton.button != sf::Mouse::Left) return;

    int code = 20000 + static_cast<int>(event.mouseButton.button);
    int shootSec = m_config.getSecondaryKeybind("shoot");

    // Always allow left mouse as shoot (fallback behavior)
    if (code == shootSec) {
        m_keyFire = false;
        releaseChargedShot();
    }
}

// ============================================================================
// Main Event Dispatcher
// ============================================================================

void GameState::handleEvent(const sf::Event& event) {
    // Handle in-game menu (pause or game over)
    if (m_gameStatus == GameStatus::InGameMenu) {
        handleMenuInput(event);
        return;
    }

    // Dispatch to specialized handlers based on event type
    switch (event.type) {
        case sf::Event::KeyPressed:
            handleKeyPressed(event.key.code);
            break;

        case sf::Event::KeyReleased:
            handleKeyReleased(event.key.code);
            break;

        case sf::Event::JoystickMoved:
            handleJoystickAxis(event);
            break;

        case sf::Event::JoystickButtonPressed:
            handleJoystickButtonPressed(event);
            break;

        case sf::Event::JoystickButtonReleased:
            handleJoystickButtonReleased(event);
            break;

        case sf::Event::MouseButtonPressed:
            handleMouseButtonPressed(event);
            break;

        case sf::Event::MouseButtonReleased:
            handleMouseButtonReleased(event);
            break;

        default:
            // Unhandled event types
            break;
    }
}

void GameState::handleMenuInput(const sf::Event& event) {
    // Keyboard navigation
    if (event.type == sf::Event::KeyPressed) {
        if (m_isVictory) {
            // In victory screen, Enter always quits to menu; Up/Down ignored
            if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                m_stateManager.setLastLevelIndex(m_levelIndex);
                m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
            }
        } else {
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
                    m_stateManager.setLastLevelIndex(m_levelIndex);
                    m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
                }
            } else if (event.key.code == sf::Keyboard::Escape && !m_isGameOver) {
                // ESC to resume (only if paused, not game over)
                resumeGame();
            }
        }
    }
    
    // Mouse hover detection
    if (!m_isVictory && event.type == sf::Event::MouseMoved) {
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
        const float button2Y = m_isVictory ? 360.0f : 420.0f;
        
        sf::FloatRect restartButton(buttonX, button1Y, buttonWidth, buttonHeight);
        sf::FloatRect menuButton(buttonX, button2Y, buttonWidth, buttonHeight);
        
        if (!m_isVictory && restartButton.contains(mousePos)) {
            if (m_isGameOver) {
                resetGame();
            }
            resumeGame();
        } else if (menuButton.contains(mousePos)) {
            m_stateManager.setLastLevelIndex(m_levelIndex);
            m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
        }
    }
}

// ============================================================================
// Keyboard Input Handlers
// ============================================================================

void GameState::handleKeyPressed(sf::Keyboard::Key key) {
    sf::Keyboard::Key upKey = m_config.getKeybind("up");
    sf::Keyboard::Key downKey = m_config.getKeybind("down");
    sf::Keyboard::Key leftKey = m_config.getKeybind("left");
    sf::Keyboard::Key rightKey = m_config.getKeybind("right");
    sf::Keyboard::Key shootKey = m_config.getKeybind("shoot");
    
    if (key == upKey) {
        m_keyUp = true;
    } else if (key == downKey) {
        m_keyDown = true;
    } else if (key == leftKey) {
        m_keyLeft = true;
    } else if (key == rightKey) {
        m_keyRight = true;
    } else if (key == shootKey) {
        m_keyFire = true;
        startChargedShot();
    } else {
        // Handle reserved keys (Escape, B)
        switch (key) {
            case sf::Keyboard::B:
                if (m_isAdmin) {
                    std::cout << "CLIENT: Admin requesting boss spawn (B key pressed)" << std::endl;
                    rtype::client::network::senders::send_spawn_boss_request();
                } else {
                    std::cout << "CLIENT: Non-admin player cannot spawn boss (B key ignored)" << std::endl;
                }
                break;
            
            case sf::Keyboard::Escape:
                showInGameMenu(false);
                break;
            
            default:
                break;
        }
    }
}

void GameState::handleKeyReleased(sf::Keyboard::Key key) {
    sf::Keyboard::Key upKey = m_config.getKeybind("up");
    sf::Keyboard::Key downKey = m_config.getKeybind("down");
    sf::Keyboard::Key leftKey = m_config.getKeybind("left");
    sf::Keyboard::Key rightKey = m_config.getKeybind("right");
    sf::Keyboard::Key shootKey = m_config.getKeybind("shoot");
    
    if (key == upKey) {
        m_keyUp = false;
    } else if (key == downKey) {
        m_keyDown = false;
    } else if (key == leftKey) {
        m_keyLeft = false;
    } else if (key == rightKey) {
        m_keyRight = false;
    } else if (key == shootKey) {
        m_keyFire = false;
        releaseChargedShot();
    }
}

} // namespace rtype::client::gui
