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
#include "gui/GUIHelper.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

GameState::GameState(StateManager& stateManager)
    : m_stateManager(stateManager), m_parallaxSystem(SCREEN_WIDTH, SCREEN_HEIGHT) {
    setupGameOverUI();
}

/**
 * @brief Initialize the Game Over UI elements
 * 
 * Sets up all text objects for the game over menu using the centralized
 * GUIHelper utilities. This ensures consistent styling with other menus
 * and proper font loading from FontManager.
 */
void GameState::setupGameOverUI() {
    const sf::Font& font = GUIHelper::getFont();
    
    // Game Over title
    m_gameOverTitleText.setFont(font);
    m_gameOverTitleText.setString("GAME OVER");
    m_gameOverTitleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE);
    m_gameOverTitleText.setFillColor(GUIHelper::Colors::TEXT);
    m_gameOverTitleText.setStyle(sf::Text::Bold);
    
    // Restart button text
    m_restartText.setFont(font);
    m_restartText.setString("Restart");
    m_restartText.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
    m_restartText.setFillColor(GUIHelper::Colors::TEXT);
    
    // Leave button text
    m_menuText.setFont(font);
    m_menuText.setString("Leave");
    m_menuText.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
    m_menuText.setFillColor(GUIHelper::Colors::TEXT);
}

void GameState::handleEvent(const sf::Event& event) {
    // Handle in-game menu (pause or game over)
    if (m_gameStatus == GameStatus::InGameMenu) {
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
                    resumeGame(); // Clear input states and return to playing
                } else {
                    // Return to main menu
                    m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
                }
            } else if (event.key.code == sf::Keyboard::Escape && !m_isGameOver) {
                // ESC to resume (only if paused, not game over)
                resumeGame(); // Clear input states and return to playing
            }
        }
        
        // Mouse interaction
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                 static_cast<float>(event.mouseButton.y));
            
            // Button dimensions (same as in renderGameOverMenu)
            sf::FloatRect restartButton((SCREEN_WIDTH - 300.0f) * 0.5f, 340.0f, 300.0f, 60.0f);
            sf::FloatRect menuButton((SCREEN_WIDTH - 300.0f) * 0.5f, 420.0f, 300.0f, 60.0f);
            
            if (restartButton.contains(mousePos)) {
                // Resume/Restart
                if (m_isGameOver) {
                    resetGame();
                }
                resumeGame(); // Clear input states and return to playing
            } else if (menuButton.contains(mousePos)) {
                // Return to main menu
                m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
            }
        }
        
        // Mouse hover for visual feedback
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), 
                                 static_cast<float>(event.mouseMove.y));
            
            sf::FloatRect restartButton((SCREEN_WIDTH - 300.0f) * 0.5f, 340.0f, 300.0f, 60.0f);
            sf::FloatRect menuButton((SCREEN_WIDTH - 300.0f) * 0.5f, 420.0f, 300.0f, 60.0f);
            
            if (restartButton.contains(mousePos)) {
                m_selectedMenuOption = 0;
            } else if (menuButton.contains(mousePos)) {
                m_selectedMenuOption = 1;
            }
        }
        
        return; // Don't process game input when in menu
    }
    
    // Handle ESC to pause during gameplay
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "Game paused..." << std::endl;
        showInGameMenu(false); // Show pause menu
        return;
    }

    if (event.type == sf::Event::KeyPressed) {
        handleKeyPressed(event.key.code);
    } else if (event.type == sf::Event::KeyReleased) {
        handleKeyReleased(event.key.code);
    }
}

void GameState::handleKeyPressed(sf::Keyboard::Key key) {
    switch (key) {
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

void GameState::handleKeyReleased(sf::Keyboard::Key key) {
    switch (key) {
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

void GameState::update(float deltaTime) {
    // Don't update game logic if in menu
    if (m_gameStatus == GameStatus::InGameMenu) {
        return;
    }
    
    // Cap delta time to prevent physics issues
    deltaTime = std::min(deltaTime, 0.016f);
    
    // Update fire cooldown
    if (m_fireCooldown > 0.0f) {
        m_fireCooldown -= deltaTime;
    }
    
    updatePlayer(deltaTime);
    updateEnemies(deltaTime);
    updateProjectiles(deltaTime);
    updateEnemyProjectiles(deltaTime);
    m_parallaxSystem.update(deltaTime);
    checkCollisions();
    checkProjectileCollisions();
    checkEnemyProjectileCollisions();
}

void GameState::render(sf::RenderWindow& window) {
    // Clear with space background
    window.clear(sf::Color::Black);
    
    // Render parallax background
    m_parallaxSystem.render(window);
    
    // Render game elements
    renderPlayer(window);
    renderEnemies(window);
    renderProjectiles(window);
    renderEnemyProjectiles(window);
    renderHUD(window);
    
    // Render in-game menu if paused or game over
    if (m_gameStatus == GameStatus::InGameMenu) {
        renderGameOverMenu(window);
    }
}

void GameState::onEnter() {
    std::cout << "=== Space Invaders Game ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ZQSD/Arrow keys - Move ship" << std::endl;
    std::cout << "  SPACE - Fire projectiles" << std::endl;
    std::cout << "  ESC - Return to menu" << std::endl;
    std::cout << "Objective: Destroy enemies with your projectiles!" << std::endl;
    
    resetGame();
}

void GameState::onExit() {
    std::cout << "Exiting game state..." << std::endl;
}

// Private methods

void GameState::updatePlayer(float deltaTime) {
    // Update invulnerability timer
    if (m_player.invulnerabilityTimer > 0.0f) {
        m_player.invulnerabilityTimer -= deltaTime;
    }
    
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
        
        // Clamp to screen bounds (full screen movement)
        m_player.position.x = std::max(m_player.size.x * 0.5f, 
                                       std::min(m_player.position.x, SCREEN_WIDTH - m_player.size.x * 0.5f));
        m_player.position.y = std::max(m_player.size.y * 0.5f, 
                                       std::min(m_player.position.y, SCREEN_HEIGHT - m_player.size.y * 0.5f));
    }
    
    // Handle fire input
    if (m_keyFire && m_fireCooldown <= 0.0f) {
        fireProjectile();
        m_fireCooldown = FIRE_COOLDOWN;
    }
}

void GameState::updateEnemies(float deltaTime) {
    // Spawn new enemies
    m_enemySpawnTimer += deltaTime;
    if (m_enemySpawnTimer >= ENEMY_SPAWN_INTERVAL) {
        m_enemySpawnTimer = 0.0f;
        spawnEnemy();
    }
    
    // Update enemy positions and fire timers
    for (auto& enemy : m_enemies) {
        enemy.position.x -= enemy.speed * deltaTime;
        
        // Update fire timer and shoot if ready
        enemy.fireTimer += deltaTime;
        if (enemy.fireTimer >= ENEMY_FIRE_INTERVAL) {
            enemy.fireTimer = 0.0f;
            fireEnemyProjectile(enemy.position);
        }
    }
    
    // Remove off-screen enemies
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const Enemy& e) { return e.position.x < -e.size.x; }),
        m_enemies.end()
    );
}

void GameState::updateProjectiles(float deltaTime) {
    // Update projectile positions
    for (auto& projectile : m_projectiles) {
        projectile.position.x += projectile.speed * deltaTime;
    }
    
    // Remove off-screen projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return p.position.x > SCREEN_WIDTH + p.size.x; }),
        m_projectiles.end()
    );
}

void GameState::updateEnemyProjectiles(float deltaTime) {
    // Update enemy projectile positions (moving left)
    for (auto& projectile : m_enemyProjectiles) {
        projectile.position.x -= projectile.speed * deltaTime;
    }
    
    // Remove off-screen enemy projectiles
    m_enemyProjectiles.erase(
        std::remove_if(m_enemyProjectiles.begin(), m_enemyProjectiles.end(),
            [](const EnemyProjectile& p) { return p.position.x < -p.size.x; }),
        m_enemyProjectiles.end()
    );
}

void GameState::checkCollisions() {
    // Only check collision if player is not invulnerable
    if (m_player.isInvulnerable()) {
        return;
    }
    
    auto playerBounds = m_player.getBounds();
    
    for (auto& enemy : m_enemies) {
        if (playerBounds.intersects(enemy.getBounds())) {
            std::cout << "Hit by enemy! " << std::endl;
            damagePlayer();
            return;
        }
    }
}

void GameState::checkProjectileCollisions() {
    // Check each projectile against each enemy
    for (auto projIt = m_projectiles.begin(); projIt != m_projectiles.end(); ) {
        bool projectileHit = false;
        auto projBounds = projIt->getBounds();
        
        for (auto enemyIt = m_enemies.begin(); enemyIt != m_enemies.end(); ) {
            if (projBounds.intersects(enemyIt->getBounds())) {
                // Deal damage to enemy
                enemyIt->health -= projIt->damage;
                
                // Mark projectile as hit
                projectileHit = true;
                
                // Remove enemy if health <= 0
                if (enemyIt->health <= 0) {
                    std::cout << "Enemy destroyed!" << std::endl;
                    enemyIt = m_enemies.erase(enemyIt);
                } else {
                    ++enemyIt;
                }
                
                break; // Projectile can only hit one enemy
            } else {
                ++enemyIt;
            }
        }
        
        // Remove projectile if it hit something
        if (projectileHit) {
            projIt = m_projectiles.erase(projIt);
        } else {
            ++projIt;
        }
    }
}

void GameState::checkEnemyProjectileCollisions() {
    // Only check if player is not invulnerable
    if (m_player.isInvulnerable()) {
        return;
    }
    
    auto playerBounds = m_player.getBounds();
    
    // Check each enemy projectile against player
    for (auto projIt = m_enemyProjectiles.begin(); projIt != m_enemyProjectiles.end(); ) {
        if (playerBounds.intersects(projIt->getBounds())) {
            // Player hit by enemy projectile
            std::cout << "Hit by enemy projectile! ";
            damagePlayer();
            
            // Remove the projectile
            projIt = m_enemyProjectiles.erase(projIt);
            return; // Stop checking after one hit
        } else {
            ++projIt;
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
    // Random initial fire timer to stagger shots
    newEnemy.fireTimer = static_cast<float>(rand() % 1000) / 1000.0f * ENEMY_FIRE_INTERVAL;
    
    m_enemies.push_back(newEnemy);
}

void GameState::fireProjectile() {
    Projectile newProjectile;
    // Spawn projectile at player's right edge, centered vertically
    newProjectile.position.x = m_player.position.x + m_player.size.x * 0.5f;
    newProjectile.position.y = m_player.position.y;
    
    m_projectiles.push_back(newProjectile);
}

void GameState::fireEnemyProjectile(const sf::Vector2f& enemyPosition) {
    EnemyProjectile newProjectile;
    // Spawn projectile at enemy's left edge, centered vertically
    newProjectile.position.x = enemyPosition.x - 12.0f; // Offset to left of enemy
    newProjectile.position.y = enemyPosition.y;
    
    m_enemyProjectiles.push_back(newProjectile);
}

void GameState::damagePlayer() {
    m_player.lives--;
    std::cout << "Lives remaining: " << m_player.lives << std::endl;
    
    if (m_player.lives <= 0) {
        std::cout << "Game Over!" << std::endl;
        showInGameMenu(true); // Show game over menu
    } else {
        // Grant temporary invulnerability
        m_player.invulnerabilityTimer = INVULNERABILITY_DURATION;
    }
}

/**
 * @brief Show the in-game menu
 * 
 * Pauses the game and displays the in-game menu with options to
 * resume/restart or return to main menu.
 * 
 * @param isGameOver True if showing menu due to game over, false for pause
 */
void GameState::showInGameMenu(bool isGameOver) {
    m_gameStatus = GameStatus::InGameMenu;
    m_isGameOver = isGameOver;
    m_selectedMenuOption = 0; // Default to first option
    
    // Reset all input states to prevent keys from staying "pressed"
    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_keyFire = false;
    
    // Update title and button text based on context
    if (isGameOver) {
        m_gameOverTitleText.setString("GAME OVER");
        m_restartText.setString("Restart");
    } else {
        m_gameOverTitleText.setString("PAUSED");
        m_restartText.setString("Resume");
    }
}

/**
 * @brief Resume the game from in-game menu
 * 
 * Returns to playing state and resets all input states to ensure
 * no keys remain in "pressed" state from the menu.
 */
void GameState::resumeGame() {
    m_gameStatus = GameStatus::Playing;
    
    // Reset all input states to prevent keys from staying "pressed"
    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_keyFire = false;
}

void GameState::resetGame() {
    // Reset player to starting position
    m_player.position = sf::Vector2f(100.0f, SCREEN_HEIGHT * 0.5f);
    m_player.lives = 3;
    m_player.invulnerabilityTimer = 0.0f;
    
    // Clear all enemies and projectiles
    m_enemies.clear();
    m_projectiles.clear();
    m_enemyProjectiles.clear();
    
    // Reset timers
    m_enemySpawnTimer = 0.0f;
    m_fireCooldown = 0.0f;
    
    // Reset parallax system
    m_parallaxSystem.reset();
    
    std::cout << "Game reset!" << std::endl;
}

void GameState::renderPlayer(sf::RenderWindow& window) {
    // Skip rendering if invulnerable and flashing (blink effect)
    if (m_player.isInvulnerable()) {
        // Flash every 0.15 seconds
        int flashCycle = static_cast<int>(m_player.invulnerabilityTimer / 0.15f);
        if (flashCycle % 2 == 0) {
            return; // Skip this frame for blinking effect
        }
    }
    
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

void GameState::renderProjectiles(sf::RenderWindow& window) {
    // Draw all projectiles (yellow/white rectangles)
    for (const auto& projectile : m_projectiles) {
        sf::RectangleShape projectileShape(projectile.size);
        projectileShape.setPosition(projectile.position - projectile.size * 0.5f);
        projectileShape.setFillColor(sf::Color::Yellow);
        window.draw(projectileShape);
    }
}

void GameState::renderEnemyProjectiles(sf::RenderWindow& window) {
    // Draw all enemy projectiles (red rectangles)
    for (const auto& projectile : m_enemyProjectiles) {
        sf::RectangleShape projectileShape(projectile.size);
        projectileShape.setPosition(projectile.position - projectile.size * 0.5f);
        projectileShape.setFillColor(sf::Color(255, 100, 100)); // Light red
        window.draw(projectileShape);
    }
}

void GameState::renderHUD(sf::RenderWindow& window) {
    // Draw lives as icons (green squares)
    for (int i = 0; i < m_player.lives; ++i) {
        sf::RectangleShape lifeIcon(sf::Vector2f(20.0f, 20.0f));
        lifeIcon.setPosition(10.0f + i * 30.0f, 10.0f);
        lifeIcon.setFillColor(sf::Color::Green);
        window.draw(lifeIcon);
    }
}

void GameState::renderGameOverMenu(sf::RenderWindow& window) {
    // Semi-transparent black overlay
    sf::RectangleShape overlay(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Game Over title
    GUIHelper::centerText(m_gameOverTitleText, SCREEN_WIDTH * 0.5f, 240.0f);
    window.draw(m_gameOverTitleText);
    
    // Restart button
    sf::RectangleShape restartButton(sf::Vector2f(300.0f, 60.0f));
    restartButton.setPosition((SCREEN_WIDTH - 300.0f) * 0.5f, 340.0f);
    if (m_selectedMenuOption == 0) {
        restartButton.setFillColor(GUIHelper::Colors::BUTTON_HOVER);
        restartButton.setOutlineColor(GUIHelper::Colors::TEXT);
        restartButton.setOutlineThickness(3.0f);
    } else {
        restartButton.setFillColor(GUIHelper::Colors::BUTTON_NORMAL);
        restartButton.setOutlineColor(GUIHelper::Colors::TEXT);
        restartButton.setOutlineThickness(2.0f);
    }
    window.draw(restartButton);
    
    // Restart text
    GUIHelper::centerText(m_restartText, SCREEN_WIDTH * 0.5f, 370.0f);
    window.draw(m_restartText);
    
    // Menu button
    sf::RectangleShape menuButton(sf::Vector2f(300.0f, 60.0f));
    menuButton.setPosition((SCREEN_WIDTH - 300.0f) * 0.5f, 420.0f);
    if (m_selectedMenuOption == 1) {
        menuButton.setFillColor(GUIHelper::Colors::BUTTON_HOVER);
        menuButton.setOutlineColor(GUIHelper::Colors::TEXT);
        menuButton.setOutlineThickness(3.0f);
    } else {
        menuButton.setFillColor(GUIHelper::Colors::BUTTON_NORMAL);
        menuButton.setOutlineColor(GUIHelper::Colors::TEXT);
        menuButton.setOutlineThickness(2.0f);
    }
    window.draw(menuButton);
    
    // Menu text
    GUIHelper::centerText(m_menuText, SCREEN_WIDTH * 0.5f, 450.0f);
    window.draw(m_menuText);
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
