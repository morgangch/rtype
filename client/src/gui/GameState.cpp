/**
 * @file GameState.cpp
 * @brief Implementation of the Space Invaders game state
 * 
 * This file implements the GameState class which provides a playable
 * Space Invaders style game with player movement, enemy spawning,
 * collision detection, and rendering.
 * 
 * The implementation follows the State pattern interface defined in State.hpp
 * and integrates seamlessly with the StateManager for state transitions.
 * 
 * Key implementation details:
 * - Uses SFML for rendering and input handling
 * - Implements normalized diagonal movement for consistent speed
 * - Static starfield for optimized rendering
 * - Simple AABB collision detection
 * - Automatic enemy cleanup when off-screen
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "gui/MainMenuState.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

GameState::GameState(StateManager& stateManager)
    : m_stateManager(stateManager) {
}

void GameState::handleEvent(const sf::Event& event) {
    // Handle ESC to return to menu
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "Returning to main menu..." << std::endl;
        m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
        return;
    }
    
    // Track key presses
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
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
            case sf::Keyboard::Space:
                m_keyFire = true;
                break;
            default:
                break;
        }
    }
    
    // Track key releases
    if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
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
            case sf::Keyboard::Space:
                m_keyFire = false;
                break;
            default:
                break;
        }
    }
}

void GameState::update(float deltaTime) {
    // Cap delta time to prevent physics issues
    deltaTime = std::min(deltaTime, 0.016f);
    
    updatePlayer(deltaTime);
    updateEnemies(deltaTime);
    checkCollisions();
}

void GameState::render(sf::RenderWindow& window) {
    // Clear with space background
    window.clear(sf::Color(5, 5, 15));
    
    // Render game elements
    renderStarfield(window);
    renderPlayer(window);
    renderEnemies(window);
}

void GameState::onEnter() {
    std::cout << "=== Space Invaders Game ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ZQSD/Arrow keys - Move ship" << std::endl;
    std::cout << "  SPACE - Fire (not implemented yet)" << std::endl;
    std::cout << "  ESC - Return to menu" << std::endl;
    
    resetGame();
}

void GameState::onExit() {
    std::cout << "Exiting game state..." << std::endl;
}

// Private methods

void GameState::updatePlayer(float deltaTime) {
    // Get movement direction
    sf::Vector2f movement = getMovementInput();
    
    // Apply movement if any
    if (movement.x != 0.0f || movement.y != 0.0f) {
        // Normalize diagonal movement to maintain constant speed
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        if (length > 0.0f) {
            movement /= length;
        }
        
        // Update position
        m_player.position += movement * m_player.speed * deltaTime;
        
        // Clamp to screen bounds
        m_player.position.x = std::max(m_player.size.x * 0.5f, 
                                       std::min(m_player.position.x, PLAYER_BOUNDS_RIGHT));
        m_player.position.y = std::max(m_player.size.y * 0.5f, 
                                       std::min(m_player.position.y, SCREEN_HEIGHT - m_player.size.y * 0.5f));
    }
    
    // Handle fire input (placeholder for future implementation)
    if (m_keyFire) {
        // TODO: Spawn projectile
        // For now, just print
        static bool fired = false;
        if (!fired) {
            std::cout << "FIRE! (Projectile system not yet implemented)" << std::endl;
            fired = true;
        }
    } else {
        static bool fired = false;
        fired = false;
    }
}

void GameState::updateEnemies(float deltaTime) {
    // Spawn new enemies
    m_enemySpawnTimer += deltaTime;
    if (m_enemySpawnTimer >= ENEMY_SPAWN_INTERVAL) {
        m_enemySpawnTimer = 0.0f;
        spawnEnemy();
    }
    
    // Update enemy positions
    for (auto& enemy : m_enemies) {
        enemy.position.x -= enemy.speed * deltaTime;
    }
    
    // Remove off-screen enemies
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const Enemy& e) { return e.position.x < -e.size.x; }),
        m_enemies.end()
    );
}

void GameState::checkCollisions() {
    auto playerBounds = m_player.getBounds();
    
    for (auto& enemy : m_enemies) {
        if (playerBounds.intersects(enemy.getBounds())) {
            std::cout << "Collision! Restarting game..." << std::endl;
            resetGame();
            return;
        }
    }
}

void GameState::spawnEnemy() {
    // Limit total number of enemies
    if (m_enemies.size() >= MAX_ENEMIES) {
        return;
    }
    
    Enemy newEnemy;
    newEnemy.position.x = SCREEN_WIDTH + newEnemy.size.x;
    // Random Y position with padding from edges
    newEnemy.position.y = 50.0f + static_cast<float>(rand() % static_cast<int>(SCREEN_HEIGHT - 100.0f));
    
    m_enemies.push_back(newEnemy);
}

void GameState::resetGame() {
    // Reset player to starting position
    m_player.position = sf::Vector2f(100.0f, SCREEN_HEIGHT * 0.5f);
    
    // Clear all enemies
    m_enemies.clear();
    
    // Reset spawn timer
    m_enemySpawnTimer = 0.0f;
    
    std::cout << "Game reset!" << std::endl;
}

void GameState::renderStarfield(sf::RenderWindow& window) {
    // Create a simple starfield effect
    static std::vector<sf::CircleShape> stars;
    
    // Generate stars on first call
    if (stars.empty()) {
        stars.reserve(50);
        for (int x = 0; x < static_cast<int>(SCREEN_WIDTH); x += 150) {
            for (int y = 0; y < static_cast<int>(SCREEN_HEIGHT); y += 150) {
                sf::CircleShape star(1.0f);
                star.setPosition(
                    static_cast<float>(x + (rand() % 50)),
                    static_cast<float>(y + (rand() % 50))
                );
                star.setFillColor(sf::Color(255, 255, 255, 128));
                stars.push_back(star);
            }
        }
    }
    
    // Draw all stars
    for (const auto& star : stars) {
        window.draw(star);
    }
}

void GameState::renderPlayer(sf::RenderWindow& window) {
    // Draw player ship (green rectangle)
    sf::RectangleShape playerShip(m_player.size);
    playerShip.setPosition(m_player.position - m_player.size * 0.5f);
    playerShip.setFillColor(sf::Color::Green);
    window.draw(playerShip);
    
    // Draw engine effect (orange)
    sf::RectangleShape engine(sf::Vector2f(8.0f, 8.0f));
    engine.setPosition(
        m_player.position.x - m_player.size.x * 0.5f - 8.0f,
        m_player.position.y - 4.0f
    );
    engine.setFillColor(sf::Color(255, 136, 0));
    window.draw(engine);
}

void GameState::renderEnemies(sf::RenderWindow& window) {
    // Draw all enemies (red rectangles)
    for (const auto& enemy : m_enemies) {
        sf::RectangleShape enemyShape(enemy.size);
        enemyShape.setPosition(enemy.position - enemy.size * 0.5f);
        enemyShape.setFillColor(sf::Color::Red);
        window.draw(enemyShape);
    }
}

sf::Vector2f GameState::getMovementInput() const {
    sf::Vector2f movement(0.0f, 0.0f);
    
    if (m_keyUp) movement.y -= 1.0f;
    if (m_keyDown) movement.y += 1.0f;
    if (m_keyLeft) movement.x -= 1.0f;
    if (m_keyRight) movement.x += 1.0f;
    
    return movement;
}

} // namespace rtype::client::gui
