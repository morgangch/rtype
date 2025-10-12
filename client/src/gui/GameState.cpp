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
#include <algorithm>
#include <cmath>
#include <iostream>

namespace rtype::client::gui {

GameState::GameState(StateManager& stateManager)
    : m_stateManager(stateManager), m_parallaxSystem(SCREEN_WIDTH, SCREEN_HEIGHT) {
    setupGameOverUI();
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
    resetGame();
    m_gameStatus = GameStatus::Playing;

    // Load and play background music for the level
    const std::string musicPath = "assets/audio/music/level.mp3";
    if (m_musicManager.loadFromFile(musicPath)) {
        m_musicManager.setVolume(30.0f); // sensible default
        m_musicManager.play(true);
    } else {
        std::cerr << "GameState: could not load music: " << musicPath << std::endl;
    }

    // Load game sounds (lose life sound)
    if (!loadGameSounds()) {
        std::cerr << "GameState: warning: some game sounds failed to load" << std::endl;
    }
}

void GameState::onExit() {
    // Clear ECS world
    m_world.Clear();
    m_playerEntity = 0;

    // Stop music when leaving the game state
    m_musicManager.stop();
}

void GameState::setMusicMuted(bool muted) {
    m_musicManager.setMuted(muted);
}

bool GameState::isMusicMuted() const {
    return m_musicManager.isMuted();
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

    // Play game-over music when death (non-looping). Pause/mute otherwise.
    if (isGameOver) {
        const std::string gameOverMusic = "assets/audio/music/gameover.mp3";
        if (m_musicManager.loadFromFile(gameOverMusic)) {
            m_musicManager.setVolume(40.0f); // adjust level if needed
            m_musicManager.play(false); // do not loop
        } else {
            std::cerr << "GameState: could not load game over music: " << gameOverMusic << std::endl;
        }
    } else {
        // Pause background music while paused
        m_musicManager.setMuted(true);
    }
}

void GameState::resumeGame() {
    m_gameStatus = GameStatus::Playing;
    
    // Reset input states to prevent stuck keys
    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_keyFire = false;

    // Unmute / resume level music when resuming from pause
    m_musicManager.setMuted(false);
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

    // Ensure level background music is playing after a reset
    const std::string levelMusic = "assets/audio/music/level.mp3";
    if (m_musicManager.loadFromFile(levelMusic)) {
        m_musicManager.setVolume(30.0f);
        m_musicManager.play(true);
    } else {
        std::cerr << "GameState: could not load level music: " << levelMusic << std::endl;
    }
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
    } else {
        // Play a short sound to indicate a lost life (non-fatal hit)
        if (m_loseLifeBuffer.getSampleCount() > 0) {
            m_loseLifeSound.play();
        }
    }
}

bool GameState::loadGameSounds() {
    const std::string loseLifePath = "assets/audio/player/loselife.mp3";
    if (!m_loseLifeBuffer.loadFromFile(loseLifePath)) {
        std::cerr << "GameState: could not load lose-life sound: " << loseLifePath << std::endl;
        return false;
    }
    m_loseLifeSound.setBuffer(m_loseLifeBuffer);
    m_loseLifeSound.setVolume(80.0f);
    return true;
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
