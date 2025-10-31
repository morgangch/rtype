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
#include "gui/AudioFactory.h"
#include "gui/GUIHelper.h"
#include "gui/AssetPaths.h"
#include "gui/TextureCache.h"
#include <common/systems/MovementSystem.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "gui/Accessibility.h"

namespace rtype::client::gui {

// Global GameState pointer definition
GameState* g_gameState = nullptr;

// Helper: create enemy from server info and store mapping
ECS::EntityID GameState::createEnemyFromServer(uint32_t serverId, float x, float y, uint16_t hp, uint16_t enemyType) {
    auto it = m_serverEntityMap.find(serverId);
    if (it != m_serverEntityMap.end()) {
        std::cout << "[GameState] Enemy " << serverId << " already exists (clientId=" << it->second << "), updating position" << std::endl;
        ECS::EntityID existing = it->second;
        auto* pos = m_world.GetComponent<rtype::common::components::Position>(existing);
        if (pos) { pos->x = x; pos->y = y; }
        auto* health = m_world.GetComponent<rtype::common::components::Health>(existing);
        if (health) { health->currentHp = hp; }
        return existing;
    }

    std::cout << "[GameState] Creating NEW enemy: serverId=" << serverId << " type=" << enemyType << " pos=(" << x << "," << y << ")" << std::endl;

    ECS::EntityID e;
    auto type = static_cast<rtype::common::components::EnemyType>(enemyType);
    
    switch (type) {
        case rtype::common::components::EnemyType::TankDestroyer:
            e = createTankDestroyer(x, y);
            break;
        case rtype::common::components::EnemyType::Shooter:
            e = createShooterEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Snake:
            e = createSnakeEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Suicide:
            e = createSuicideEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Basic:
        default:
            e = createEnemy(x, y);
            break;
    }
    
    // Override HP with server value (server is authoritative)
    auto* health = m_world.GetComponent<rtype::common::components::Health>(e);
    if (health) {
        health->currentHp = hp;
        health->maxHp = hp;
    }
    
    // Override position with server value
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(e);
    if (pos) {
        pos->x = x;
        pos->y = y;
    }

    // Store mapping
    m_serverEntityMap[serverId] = e;
    std::cout << "[GameState] ✓ Created enemy: clientId=" << e << " serverId=" << serverId << std::endl;
    return e;
}

ECS::EntityID GameState::createRemotePlayer(const std::string &name, uint32_t serverId) {
    std::cout << "[GameState] Creating remote player: name=" << name << " serverId=" << serverId << std::endl;
    
    // Check if we already have this player (shouldn't happen, but be safe)
    auto it = m_serverEntityMap.find(serverId);
    if (it != m_serverEntityMap.end()) {
        std::cout << "[GameState] WARNING: Remote player with serverId=" << serverId << " already exists (clientId=" << it->second << ")" << std::endl;
        return it->second;
    }
    
    // Create a player-like entity (non-controllable)
    ECS::EntityID e = m_world.CreateEntity();
    m_world.AddComponent<rtype::common::components::Position>(e, 100.0f, 360.0f, 0.0f);
    m_world.AddComponent<rtype::client::components::Sprite>(e, rtype::client::assets::player::PLAYER_SPRITE, sf::Vector2f(33.0f, 17.0f), true, sf::IntRect(0,0,33,17), 3.0f);
    m_world.AddComponent<rtype::common::components::Player>(e, name, serverId);
    m_world.AddComponent<rtype::common::components::Health>(e, 3);
    m_world.AddComponent<rtype::common::components::Team>(e, rtype::common::components::TeamType::Player);

    m_serverEntityMap[serverId] = e;
    std::cout << "[GameState] ✓ Created remote player entity: clientId=" << e << " serverId=" << serverId << std::endl;
    return e;
}

ECS::EntityID GameState::createProjectileFromServer(uint32_t serverId, uint32_t ownerId, float x, float y, float vx, float vy, uint16_t damage, bool piercing, bool isCharged) {
    std::cout << "[GameState] Creating projectile from server: serverId=" << serverId << " owner=" << ownerId << " pos=(" << x << "," << y << ") vel=(" << vx << "," << vy << ") charged=" << isCharged << std::endl;
    
    // Check if we already have this projectile (shouldn't happen)
    auto it = m_serverEntityMap.find(serverId);
    if (it != m_serverEntityMap.end()) {
        std::cout << "[GameState] WARNING: Projectile with serverId=" << serverId << " already exists" << std::endl;
        return it->second;
    }
    
    // Create projectile entity
    auto entity = m_world.CreateEntity();
    
    // Position
    m_world.AddComponent<rtype::common::components::Position>(entity, x, y, 0.0f);
    
    // Velocity
    m_world.AddComponent<rtype::common::components::Velocity>(entity, vx, vy, std::sqrt(vx*vx + vy*vy));
    
    // Sprite - Different sprite based on team and charged state
    if (ownerId == 0) {
        // Enemy projectile - PROJECTILE_2 (orange)
        m_world.AddComponent<rtype::client::components::Sprite>(
            entity,
            rtype::client::assets::projectiles::PROJECTILE_2,
            sf::Vector2f(81.0f, 17.0f),
            true,
            sf::IntRect(185, 0, 81, 17), // Frame 2, line 1: orange
            0.4f); // Scale 0.4x (smaller than player projectiles)
        std::cout << "[GameState]   Using ENEMY sprite (PROJECTILE_2, orange)" << std::endl;
    } else if (isCharged) {
        // Charged player projectile - PROJECTILE_4 (rose/magenta)
        m_world.AddComponent<rtype::client::components::Sprite>(
            entity,
            rtype::client::assets::projectiles::PROJECTILE_4,
            sf::Vector2f(81.0f, 17.0f),
            true,
            sf::IntRect(185, 17, 81, 17), // Frame 2, line 2: denser visual
            0.6f); // Scale 0.6x (bigger than normal)
        std::cout << "[GameState]   Using CHARGED player sprite (PROJECTILE_4)" << std::endl;
    } else {
        // Normal player projectile - PROJECTILE_1
        m_world.AddComponent<rtype::client::components::Sprite>(
            entity,
            rtype::client::assets::projectiles::PROJECTILE_1,
            sf::Vector2f(81.0f, 17.0f),
            true,
            sf::IntRect(185, 0, 81, 17),
            0.5f);
        std::cout << "[GameState]   Using NORMAL player sprite (PROJECTILE_1)" << std::endl;
    }
    
    // Team - Determine based on owner (0 = enemy, non-zero = player)
    rtype::common::components::TeamType team = (ownerId == 0)
        ? rtype::common::components::TeamType::Enemy
        : rtype::common::components::TeamType::Player;
    m_world.AddComponent<rtype::common::components::Team>(entity, team);

    // Projectile data - mark as server-owned for server authority
    m_world.AddComponent<rtype::common::components::Projectile>(entity, damage, piercing, true /* serverOwned */);
    
    // Map server ID to local entity
    m_serverEntityMap[serverId] = entity;
    
    std::cout << "[GameState] ✓ Created projectile entity: clientId=" << entity << " serverId=" << serverId << " damage=" << damage << " piercing=" << piercing << " serverOwned=TRUE" << std::endl;
    return entity;
}

void GameState::updateEntityStateFromServer(uint32_t serverId, float x, float y, uint16_t hp, bool invulnerable) {
    auto it = m_serverEntityMap.find(serverId);
    if (it == m_serverEntityMap.end()) return;

    ECS::EntityID e = it->second;

    auto* health = m_world.GetComponent<rtype::common::components::Health>(e);
    if (health) {
        if (serverId == m_localPlayerServerId) {
            health->invulnerable = invulnerable;
            return;
        }

        health->currentHp = hp;
        health->invulnerable = invulnerable;
    }

    auto* pos = m_world.GetComponent<rtype::common::components::Position>(e);
    if (pos) {
        pos->x = x;
        pos->y = y;
    }
}

void GameState::setLocalPlayerServerId(uint32_t serverId) {
    m_localPlayerServerId = serverId;
    std::cout << "[GameState] Local player server ID set to: " << serverId << std::endl;

    // CRITICAL: Map local player entity to server ID if it exists
    // This allows server to update local player's HP via PLAYER_STATE packets
    // Note: Player entity might not be created yet (onEnter() not called),
    // but mapping will be done in resetGame() when the entity is created
    if (m_playerEntity != 0) {
        m_serverEntityMap[serverId] = m_playerEntity;
        std::cout << "[GameState] Mapped local player: serverId=" << serverId << " -> clientEntity=" << m_playerEntity << std::endl;
    } else {
        std::cout << "[GameState] Local player entity not created yet - will be mapped in resetGame()" << std::endl;
    }
}

void GameState::setIsAdmin(bool isAdmin) {
    m_isAdmin = isAdmin;
    std::cout << "[GameState] Player admin status set to: " << (isAdmin ? "ADMIN" : "PLAYER") << std::endl;
}

void GameState::destroyEntityByServerId(uint32_t serverId) {
    auto it = m_serverEntityMap.find(serverId);
    if (it == m_serverEntityMap.end()) {
        // Entity not found - this is OK if we already destroyed it in client-side prediction
        std::cout << "[GameState] Entity with serverId=" << serverId << " not found (already destroyed locally?)" << std::endl;
        return;
    }
    ECS::EntityID e = it->second;
    
    // Check if entity still exists in world (might have been destroyed by client-side prediction)
    // We can't directly check, so we'll try to get a component
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(e);
    if (!pos) {
        // Entity doesn't exist anymore - was already destroyed locally
        std::cout << "[GameState] Entity clientId=" << e << " serverId=" << serverId << " already destroyed locally, cleaning up mapping" << std::endl;
        m_serverEntityMap.erase(it);
        return;
    }
    
    std::cout << "[GameState] Destroying entity (server confirmation): clientId=" << e << " serverId=" << serverId << std::endl;
    m_world.DestroyEntity(e);
    m_serverEntityMap.erase(it);
}


GameState::GameState(StateManager& stateManager)
    : m_stateManager(stateManager), m_parallaxSystem(SCREEN_WIDTH, SCREEN_HEIGHT) {
    // Load keybinds and settings from config file
    m_config.load();
    // Apply daltonism mode globally
    rtype::client::gui::Accessibility::instance().setMode(m_config.getDaltonismMode());
    
    setupGameOverUI();
    // set global pointer so network handlers can access the active GameState
    g_gameState = this;
}

GameState::~GameState() {
    if (g_gameState == this) g_gameState = nullptr;
}

int GameState::getLevelIndex() const {
    return m_levelIndex;
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

    // Pre-load projectile textures to avoid loading in hot path (entity creation)
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_1);
    rtype::client::gui::TextureCache::getInstance().loadTexture(rtype::client::assets::projectiles::PROJECTILE_4);

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

    // Start level music
    loadLevelMusic();

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
        const std::string gameOverMusic = AudioFactory::getMusicPath(AudioFactory::MusicId::GameOver);
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

    // CRITICAL: Map local player entity to server ID if we already have one
    // This ensures PLAYER_STATE packets can update the local player's HP
    if (m_localPlayerServerId != 0) {
        m_serverEntityMap[m_localPlayerServerId] = m_playerEntity;
        std::cout << "[GameState::resetGame] Mapped local player: serverId=" << m_localPlayerServerId << " -> clientEntity=" << m_playerEntity << std::endl;
    }

    // Reset flags
    m_isGameOver = false;
    m_gameStatus = GameStatus::Playing;

    // Clear boss music/flag so a prior boss state doesn't trigger level advance
    m_bossMusicActive = false;

    // Ensure level background music is playing after a reset
    loadLevelMusic();
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
        if (m_soundManager.has(AudioFactory::SfxId::LoseLife)) {
            m_soundManager.play(AudioFactory::SfxId::LoseLife);
        }
    }
}

bool GameState::loadGameSounds() {
    bool ok = m_soundManager.loadAll();
    // Configure volumes (only if loaded)
    if (m_soundManager.has(AudioFactory::SfxId::LoseLife)) m_soundManager.setVolume(AudioFactory::SfxId::LoseLife, 80.0f);
    if (m_soundManager.has(AudioFactory::SfxId::Shoot)) m_soundManager.setVolume(AudioFactory::SfxId::Shoot, 70.0f);
    if (m_soundManager.has(AudioFactory::SfxId::ChargedShoot)) m_soundManager.setVolume(AudioFactory::SfxId::ChargedShoot, 75.0f);
    if (m_soundManager.has(AudioFactory::SfxId::EnemyDeath)) m_soundManager.setVolume(AudioFactory::SfxId::EnemyDeath, 80.0f);
    if (m_soundManager.has(AudioFactory::SfxId::BossDeath)) m_soundManager.setVolume(AudioFactory::SfxId::BossDeath, 85.0f);

    // Note: bossfight music is handled by the MusicManager; we will load it on boss spawn
    return ok;
}

bool GameState::isBossActive() {
    // Pure ECS approach: query the world for boss entities
    auto* enemyTypes = m_world.GetAllComponents<rtype::common::components::EnemyTypeComponent>();
    if (!enemyTypes) return false;
    
    for (auto& [entity, enemyTypePtr] : *enemyTypes) {
        if (enemyTypePtr->type == rtype::common::components::EnemyType::TankDestroyer) { // to do add other boss types
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
    // Ensure boss music follows boss alive state (covers debug spawn)
    updateBossMusicState();

    if (m_gameStatus == GameStatus::InGameMenu) {
        return; // Don't update game logic when in menu
    }

    // Check if player is dead (HP <= 0) - show game over screen
    if (m_playerEntity != 0) {
        auto* health = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
        if (health && health->currentHp <= 0) {
            std::cout << "[GameState] Player died - showing game over screen" << std::endl;
            showInGameMenu(true); // true = game over
            return;
        }
    }

    // Update parallax background
    m_parallaxSystem.update(deltaTime);

    // Run ECS systems in order
    updateInputSystem(deltaTime);
    updateFireRateSystem(deltaTime);
    updateEnemyAISystem(deltaTime); // Local prediction for enemy shooting
    updateChargedShotSystem(deltaTime);
    updateInvulnerabilitySystem(deltaTime);
    updateAnimationSystem(deltaTime);
    rtype::common::systems::MovementSystem::update(m_world, deltaTime); // shared movement system
    updateCollisionSystem();
    updateCleanupSystem(deltaTime);
}

void GameState::updateBossMusicState() {
    bool bossAlive = isBossActive();
    if (bossAlive && !m_bossMusicActive) {
        // Start boss music
        const std::string bossMusic = AudioFactory::getMusicPath(AudioFactory::MusicId::BossFight1);
        if (m_musicManager.loadFromFile(bossMusic)) {
            m_musicManager.setVolume(35.0f);
            m_musicManager.play(true);
            m_bossMusicActive = true;
        } else {
            std::cerr << "GameState: could not load boss music: " << bossMusic << std::endl;
        }
    } else if (!bossAlive && m_bossMusicActive) {
        // Boss died: advance level (plays test music and swaps background)
        advanceLevel();
        // Boss died: advance level (plays test music and swaps background)
        advanceLevel();
        m_bossMusicActive = false;
    }
}

void GameState::advanceLevel() {
    m_levelIndex += 1;
    std::cout << "[GameState] Advancing to level index: " << m_levelIndex << std::endl;

    // If we've exceeded the final level (3), return to main menu
    if (m_levelIndex >= 3) {
        std::cout << "[GameState] Final level cleared. Returning to main menu." << std::endl;
        m_musicManager.stop();
        // Persist last level index for menu parallax
        m_stateManager.setLastLevelIndex(m_levelIndex);
        m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
        return;
    }

    // Use music from level 2 after defeating the boss of the level 1
    const std::string level2Music = AudioFactory::getMusicPath(AudioFactory::MusicId::Level2);
    if (m_musicManager.loadFromFile(level2Music)) {
        m_musicManager.setVolume(40.0f);
        m_musicManager.play(true);
    } else {
        std::cerr << "GameState: could not load level 2 music: " << level2Music << std::endl;
    }

    // Transition parallax to hallway theme for level 2
    m_parallaxSystem.transitionToTheme(ParallaxSystem::Theme::HallwayLevel2, 1.0f);
}

void GameState::loadLevelMusic() {
    // Choose music based on current level index so resets keep the correct track
    AudioFactory::MusicId id = AudioFactory::MusicId::Level1;
    if (m_levelIndex == 1) id = AudioFactory::MusicId::Level2;
    else if (m_levelIndex == 2) id = AudioFactory::MusicId::Level3;

    const std::string levelMusic = AudioFactory::getMusicPath(id);
    if (m_musicManager.loadFromFile(levelMusic)) {
        m_musicManager.setVolume(30.0f);
        m_musicManager.play(true);
    } else {
        std::cerr << "GameState: could not load level music: " << levelMusic << std::endl;
    }
}

void GameState::render(sf::RenderWindow& window) {
    // Render parallax background or a temporary white background when forced
    if (m_forceWhiteBackground) {
        sf::RectangleShape bg(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
        bg.setFillColor(sf::Color::White);
        window.draw(bg);
    } else {
        m_parallaxSystem.render(window);
    }
    
    // Render all entities
    renderEntities(window);
    
    // Render HUD
    renderHUD(window);
    
    // Render menu if in menu state
    if (m_gameStatus == GameStatus::InGameMenu) {
        renderGameOverMenu(window);
    }

    // Apply colorblind post-process over the whole frame
    if (Accessibility::instance().isEnabled()) {
        static sf::Texture screenTexture;
        sf::Vector2u size = window.getSize();
        if (screenTexture.getSize() != size) {
            screenTexture.create(size.x, size.y);
        }
        screenTexture.update(window);
        sf::Sprite screenSprite(screenTexture);
        if (auto* shader = Accessibility::instance().getShader()) {
            sf::RenderStates states;
            states.shader = shader;
            window.draw(screenSprite, states);
        }
    }
}

} // namespace rtype::client::gui
