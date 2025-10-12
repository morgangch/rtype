/**
 * @file GameState.cpp
 * @brief Core GameState implementation - Orchestration and lifecycle
 * 
 * This file contains the main GameState implementation including:
 * - Constructor and initialization
 * - State lifecycle (onEnter/onExit)
 * - Main update and render orchestration
 * - Game reset logic
 * - Helper methods
 * 
 * The modular implementation splits GameState into 5 files:
 * - GameState.cpp (this file): Core orchestration and lifecycle
 * - EntityFactory.cpp: Entity creation methods
 * - GameLogicSystems.cpp: ECS systems implementation
 * - GameRenderer.cpp: Rendering logic
 * - InputHandler.cpp: Input event processing
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/GameState.h"
#include "gui/MainMenuState.h"
#include "gui/GUIHelper.h"
#include "gui/AssetPaths.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

GameState::GameState(StateManager& stateManager)
    : m_stateManager(stateManager), m_parallaxSystem(SCREEN_WIDTH, SCREEN_HEIGHT) {
    setupGameOverUI();
}

void GameState::loadHUDTextures() {
    if (m_texturesLoaded) return;

    // Load heart texture for HUD
    if (!m_heartTexture.loadFromFile(rtype::client::assets::hearts::HEART_SPRITE)) {
        std::cerr << "Failed to load heart texture!" << std::endl;
        m_texturesLoaded = false;
        return;
    }

    // Heart spritesheet: 3072x4096, frames are 248x216
    // Starting at (32, 256)
    const int frameWidth = 248;
    const int frameHeight = 216;
    const int startX = 32;
    const int startY = 256;

    // Frame 1 (col 0-3, row 0-1): Full heart - s'étend sur 4 colonnes x 2 lignes
    m_fullHeartSprite.setTexture(m_heartTexture);
    m_fullHeartSprite.setTextureRect(sf::IntRect(startX, startY, frameWidth * 4, frameHeight * 2));
    m_fullHeartSprite.setScale(0.08f, 0.08f);  // Scale down (992*0.08 = 79px, 432*0.08 = 35px)

    // Frame 3 (col 8-11, row 0-1): Empty/lost heart - 4 colonnes x 2 lignes
    m_emptyHeartSprite.setTexture(m_heartTexture);
    m_emptyHeartSprite.setTextureRect(sf::IntRect(startX + frameWidth * 8, startY, frameWidth * 4, frameHeight * 2));
    m_emptyHeartSprite.setScale(0.08f, 0.08f);  // Scale down (992*0.08 = 79px, 432*0.08 = 35px)

    m_texturesLoaded = true;
}

void GameState::setupGameOverUI() {
    const sf::Font& font = GUIHelper::getFont();
    
    // Game Over title
    m_gameOverTitleText.setFont(font);
    m_gameOverTitleText.setString("GAME OVER");
    m_gameOverTitleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE);
    m_gameOverTitleText.setFillColor(GUIHelper::Colors::TEXT);
    m_gameOverTitleText.setStyle(sf::Text::Bold);
    GUIHelper::centerText(m_gameOverTitleText, SCREEN_WIDTH / 2.0f, 150.0f);
    
    // Restart/Resume button
    m_restartText.setFont(font);
    m_restartText.setString("Restart");  // Will change to "Resume" if paused
    m_restartText.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
    m_restartText.setFillColor(GUIHelper::Colors::TEXT);
    GUIHelper::centerText(m_restartText, SCREEN_WIDTH / 2.0f, 300.0f);
    
    // Leave button
    m_menuText.setFont(font);
    m_menuText.setString("Leave");
    m_menuText.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
    m_menuText.setFillColor(GUIHelper::Colors::TEXT);
    GUIHelper::centerText(m_menuText, SCREEN_WIDTH / 2.0f, 380.0f);
}

void GameState::onEnter() {
    // Load HUD textures during state entry (avoid I/O in constructor)
    loadHUDTextures();

    resetGame();
    m_gameStatus = GameStatus::Playing;
}

void GameState::onExit() {
    // Clear ECS world
    m_world.Clear();
    m_playerEntity = 0;
}

void GameState::showInGameMenu(bool isGameOver) {
    m_gameStatus = GameStatus::InGameMenu;
    m_isGameOver = isGameOver;
    m_selectedMenuOption = 0;
    
    // Update restart button text based on context
    if (isGameOver) {
        m_restartText.setString("Restart");
        m_gameOverTitleText.setString("GAME OVER");
    } else {
        m_restartText.setString("Resume");
        m_gameOverTitleText.setString("PAUSED");
    }
    GUIHelper::centerText(m_restartText, SCREEN_WIDTH / 2.0f, 300.0f);
    GUIHelper::centerText(m_gameOverTitleText, SCREEN_WIDTH / 2.0f, 150.0f);
    
    // Reset input states to prevent stuck keys
    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_keyFire = false;
}

void GameState::resumeGame() {
    m_gameStatus = GameStatus::Playing;
    
    // Reset input states to prevent stuck keys
    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_keyFire = false;
}

void GameState::resetGame() {
    // Clear ECS world
    m_world.Clear();
    
    // Create player entity
    m_playerEntity = createPlayer();
    
    // Reset timers
    m_enemySpawnTimer = 0.0f;
    m_bossSpawnTimer = 0.0f;
    
    // Reset flags
    m_isGameOver = false;
    m_gameStatus = GameStatus::Playing;
}

int GameState::getPlayerLives() const {
    if (m_playerEntity == 0) return 0;
    
    // Cast away const to access component (ECS::World doesn't have const GetComponent)
    auto* health = const_cast<ECS::World&>(m_world).GetComponent<rtype::common::components::Health>(m_playerEntity);
    if (!health) return 0;
    
    return health->currentHp;
}

void GameState::damagePlayer(int damage) {
    if (m_playerEntity == 0) return;
    
    // Get health component
    auto* health = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
    if (!health) return;
    
    // Check invulnerability (built into Health component)
    if (health->invulnerable) {
        return; // Player is invulnerable
    }
    
    // Apply damage
    health->currentHp -= damage;
    
    // Grant invulnerability
    health->invulnerable = true;
    health->invulnerabilityTimer = INVULNERABILITY_DURATION;
    
    // Check for game over
    if (health->currentHp <= 0) {
        showInGameMenu(true); // Game Over
    }
}

bool GameState::isBossActive() {
    // Pure ECS approach: query the world for boss entities
    auto* enemyTypes = m_world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (!enemyTypes) return false;
    
    for (auto& [entity, enemyTypePtr] : *enemyTypes) {
        if (enemyTypePtr->type == rtype::common::components::EnemyType::Boss) {
            // Check if the boss is still alive (has Health component with HP > 0)
            auto* health = m_world.GetComponent<rtype::common::components::Health>(entity);
            if (health && health->currentHp > 0) {
                return true;
            }
        }
    }
    
    return false;
}

void GameState::update(float deltaTime) {
    if (m_gameStatus == GameStatus::InGameMenu) {
        return; // Don't update game logic when in menu
    }
    
    // Update parallax background
    m_parallaxSystem.update(deltaTime);
    
    // Run ECS systems in order
    updateInputSystem(deltaTime);
    updateFireRateSystem(deltaTime);
    updateChargedShotSystem(deltaTime);
    updateInvulnerabilitySystem(deltaTime);
    updateAnimationSystem(deltaTime);
    updateMovementSystem(deltaTime);
    updateEnemySpawnSystem(deltaTime);
    updateEnemyAISystem(deltaTime);
    updateCleanupSystem(deltaTime);
    updateCollisionSystem();
}

void GameState::render(sf::RenderWindow& window) {
    // Render parallax background
    m_parallaxSystem.render(window);
    
    // Render all entities
    renderEntities(window);
    
    // Render HUD
    renderHUD(window);
    
    // Render menu if in menu state
    if (m_gameStatus == GameStatus::InGameMenu) {
        renderGameOverMenu(window);
    }
}

} // namespace rtype::client::gui
