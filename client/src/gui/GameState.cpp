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
#include <common/systems/FortressShieldSystem.h>
#include <common/components/Shield.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include "gui/Accessibility.h"

// Global username defined in client/src/network/network.cpp
extern std::string g_username;

namespace rtype::client::gui {

// Global GameState pointer definition
GameState* g_gameState = nullptr;

// Helper: create enemy from server info and store mapping
ECS::EntityID GameState::createEnemyFromServer(uint32_t serverId, float x, float y, uint16_t hp, uint16_t enemyType) {
    // If we already have an entity mapped, update it instead
    auto it = m_serverEntityMap.find(serverId);
    if (it != m_serverEntityMap.end()) {
        ECS::EntityID existing = it->second;
        auto* pos = m_world.GetComponent<rtype::common::components::Position>(existing);
        if (pos) { pos->x = x; pos->y = y; }
        auto* health = m_world.GetComponent<rtype::common::components::Health>(existing);
        if (health) { health->currentHp = hp; }
        return existing;
    }

    // Create a new enemy based on enemyType
    ECS::EntityID e;
    auto type = static_cast<rtype::common::components::EnemyType>(enemyType);

    switch (type) {
        // Basic enemies
        case rtype::common::components::EnemyType::Basic:
            e = createEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Snake:
            e = createSnakeEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Suicide:
            e = createSuicideEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Pata:
            e = createPataEnemy(x, y);
            break;

        // Advanced enemies
        case rtype::common::components::EnemyType::Shielded:
            e = createShieldedEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Flanker:
            e = createFlankerEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Turret:
            e = createTurretEnemy(x, y);
            break;
        case rtype::common::components::EnemyType::Waver:
            e = createWaverEnemy(x, y);
            break;

        // Obstacles
        case rtype::common::components::EnemyType::Meteorite:
            e = createMeteorite(x, y);
            break;
        case rtype::common::components::EnemyType::Debri:
            e = createDebri(x, y);
            break;

        // Boss enemies
        case rtype::common::components::EnemyType::TankDestroyer:
            e = createTankDestroyer(x, y);
            break;
        case rtype::common::components::EnemyType::Serpent:
            e = createSerpentBoss(x, y);
            break;
        case rtype::common::components::EnemyType::Fortress:
            e = createFortressBoss(x, y);
            break;
        case rtype::common::components::EnemyType::Core:
            e = createCoreBoss(x, y);
            break;

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
    return e;
}

ECS::EntityID GameState::createRemotePlayer(const std::string &name, uint32_t serverId, rtype::common::components::VesselType vesselType) {
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
    
    // Sprite - Use correct row for vessel type
    const int frameWidth = 33;
    const int frameHeight = 17;
    int vesselRow = static_cast<int>(vesselType);  // 0=Crimson, 1=Azure, 2=Emerald, 3=Solar
    int startY = vesselRow * frameHeight;
    m_world.AddComponent<rtype::client::components::Sprite>(
        e, 
        rtype::client::assets::player::PLAYER_SPRITE, 
        sf::Vector2f(static_cast<float>(frameWidth), static_cast<float>(frameHeight)), 
        true, 
        sf::IntRect(0, startY, frameWidth, frameHeight), 
        3.0f);
    
    m_world.AddComponent<rtype::common::components::Player>(e, name, serverId);
    
    // Health - HP based on vessel type
    int baseHp = 3; // Default
    switch (vesselType) {
        case rtype::common::components::VesselType::CrimsonStriker:
            baseHp = 3;
            break;
        case rtype::common::components::VesselType::AzurePhantom:
            baseHp = 2; // Agile (low defense, only 2 hearts)
            break;
        case rtype::common::components::VesselType::EmeraldTitan:
            baseHp = 4;
            break;
        case rtype::common::components::VesselType::SolarGuardian:
            baseHp = 5;
            break;
    }
    m_world.AddComponent<rtype::common::components::Health>(e, baseHp);
    m_world.AddComponent<rtype::common::components::VesselClass>(e, vesselType);
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
    
    // Sprite selection mapping to use requested assets 1..4
    const char* tex = rtype::client::assets::projectiles::PROJECTILE_1;
    sf::IntRect rect(185, 0, 81, 17);
    float scale = 0.5f;
    if (isCharged || piercing) {
        tex = rtype::client::assets::projectiles::PROJECTILE_4;
        rect = sf::IntRect(185, 17, 81, 17);
        scale = 0.6f;
    } else if (std::fabs(vy) > 1e-3f) {
        // Angled shots (e.g., Azure bouncy split) -> PROJECTILE_2
        tex = rtype::client::assets::projectiles::PROJECTILE_2;
        rect = sf::IntRect(185, 0, 81, 17);
        scale = 0.5f;
    } else if (damage >= 2) {
        // Higher damage straight shots (e.g., Dual laser) -> PROJECTILE_3
        tex = rtype::client::assets::projectiles::PROJECTILE_3;
        rect = sf::IntRect(185, 0, 81, 17);
        scale = 0.55f;
    }

    // Lazy load missing textures (2 and 3)
    rtype::client::gui::TextureCache::getInstance().loadTexture(tex);
    m_world.AddComponent<rtype::client::components::Sprite>(
        entity,
        tex,
        sf::Vector2f(81.0f, 17.0f),
        true,
        rect,
        scale);
    std::cout << "[GameState]   Using projectile sprite: " << tex << std::endl;
    
    // Team - Player team (projectiles are always player team for now)
    m_world.AddComponent<rtype::common::components::Team>(
        entity, rtype::common::components::TeamType::Player);
    
    // Projectile data - mark as server-owned for prediction
    m_world.AddComponent<rtype::common::components::Projectile>(entity, damage, piercing, true /* serverOwned */);
    
    // Map server ID to local entity
    m_serverEntityMap[serverId] = entity;
    
    std::cout << "[GameState] ✓ Created projectile entity: clientId=" << entity << " serverId=" << serverId << " damage=" << damage << " piercing=" << piercing << " serverOwned=TRUE" << std::endl;
    return entity;
}

void GameState::updateEntityStateFromServer(uint32_t serverId, float x, float y, uint16_t hp, bool invulnerable, uint16_t maxHp) {
    // For local player: only update HP/maxHp/invulnerability (position is client-controlled)
    if (serverId == m_localPlayerServerId) {
        auto* health = m_world.GetComponent<rtype::common::components::Health>(m_playerEntity);
        if (health) {
            health->currentHp = hp;
            health->maxHp = maxHp;
            health->invulnerable = invulnerable;
        }
        return;
    }

    // For remote players: update everything
    auto it = m_serverEntityMap.find(serverId);
    if (it == m_serverEntityMap.end()) {
        std::cout << "[GameState] Cannot update entity with serverId=" << serverId << " - not found in map" << std::endl;
        return;
    }
    
    ECS::EntityID e = it->second;
    auto* pos = m_world.GetComponent<rtype::common::components::Position>(e);
    if (pos) { pos->x = x; pos->y = y; }
    auto* health = m_world.GetComponent<rtype::common::components::Health>(e);
    if (health) {
        health->currentHp = hp;
        health->maxHp = maxHp;
        health->invulnerable = invulnerable;
    }
}

void GameState::updateShieldStateFromServer(uint32_t serverId, bool isActive, float duration) {
    // Find the entity by server ID
    auto it = m_serverEntityMap.find(serverId);
    if (it == m_serverEntityMap.end()) {
        return; // Entity not found
    }
    
    ECS::EntityID e = it->second;
    
    // Update or add ShieldComponent
    auto* shield = m_world.GetComponent<rtype::common::components::ShieldComponent>(e);
    if (shield) {
        shield->isActive = isActive;
        // The new ShieldComponent doesn't have a duration field, but we keep the function signature
        // for network compatibility. The shield type should be managed server-side.
    } else if (isActive) {
        // Add ShieldComponent if it doesn't exist and shield is active
        m_world.AddComponent<rtype::common::components::ShieldComponent>(
            e, 
            rtype::common::components::ShieldType::Blue, // Default type
            isActive
        );
    }
}

void GameState::setLocalPlayerServerId(uint32_t serverId) {
    m_localPlayerServerId = serverId;
    std::cout << "[GameState] Local player server ID set to: " << serverId << std::endl;
}

void GameState::setIsAdmin(bool isAdmin) {
    m_isAdmin = isAdmin;
    std::cout << "[GameState] Player admin status set to: " << (isAdmin ? "ADMIN" : "PLAYER") << std::endl;
}

void GameState::setLocalVesselType(rtype::common::components::VesselType vesselType) {
    m_localVesselType = vesselType;
    std::cout << "[GameState] Local vessel type set to: " << static_cast<int>(vesselType) << std::endl;
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
    // Setup HUD score text/defaults
    m_score = 0;
    m_scoreSaved = false;
    m_scoreText.setFont(GUIHelper::getFont());
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(GUIHelper::Colors::TEXT);
    // set global pointer so network handlers can access the active GameState
    g_gameState = this;
}

GameState::~GameState() {
    if (g_gameState == this) g_gameState = nullptr;
}

int GameState::getLevelIndex() const {
    return m_levelIndex;
}

void GameState::setLevelIndex(int index) {
    m_levelIndex = std::max(0, index);
    std::cout << "[GameState] Level index set to: " << m_levelIndex << std::endl;
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

    // Ensure parallax theme matches the starting level immediately
    m_parallaxSystem.setTheme(ParallaxSystem::themeFromLevel(m_levelIndex), true);

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
    // Clear any victory effects
    clearVictoryEffects();
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
    m_isVictory = false;
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
        // Persist highscore once when game over triggers
        if (!m_scoreSaved) {
            HighscoreManager mgr;
            mgr.load();
            HighscoreEntry e{::g_username, 1, m_score, 0};
            mgr.add(e);
            mgr.save();
            m_scoreSaved = true;
        }
    } else {
        // Pause background music while paused
        m_musicManager.setMuted(true);
    }
}

void GameState::showVictoryScreen() {
    m_gameStatus = GameStatus::InGameMenu;
    m_isGameOver = false;
    m_isVictory = true;
    m_selectedMenuOption = 1; 

    // Set title and center
    m_gameOverTitleText.setString("YOU WON");
    GUIHelper::centerText(m_gameOverTitleText, SCREEN_WIDTH / 2.0f, 150.0f);

    // Set Quit label
    m_menuText.setString("Quit");

    // Stop any current music and play victory track (looping)
    m_musicManager.stop();
    const std::string victory = AudioFactory::getMusicPath(AudioFactory::MusicId::Victory);
    if (!victory.empty()) {
        if (m_musicManager.loadFromFile(victory)) {
            m_musicManager.setVolume(40.0f);
            m_musicManager.play(true);
        } else {
            std::cerr << "GameState: could not load victory music: " << victory << std::endl;
        }
    }

    // Reset input states to prevent stuck keys
    m_keyUp = m_keyDown = m_keyLeft = m_keyRight = m_keyFire = false;

    // Spawn a burst of confetti for celebration
    spawnVictoryConfetti(180);
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
    
    // Create player entity with selected vessel type
    m_playerEntity = createPlayer(m_localVesselType);
    
    // Reset flags
    m_isGameOver = false;
    m_gameStatus = GameStatus::Playing;
    m_score = 0;
    m_scoreSaved = false;

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

int GameState::getPlayerMaxLives() const {
    if (m_playerEntity == 0) return 3;  // Default to 3 if no player
    
    // Cast away const to access component (ECS::World doesn't have const GetComponent)
    auto* health = const_cast<ECS::World&>(m_world).GetComponent<rtype::common::components::Health>(m_playerEntity);
    if (!health) return 3;  // Default to 3 if no health component
    
    return health->maxHp;
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
        if (!enemyTypePtr) continue;
        const auto t = enemyTypePtr->type;
        // Consider all boss types across levels
        if (t == rtype::common::components::EnemyType::TankDestroyer ||
            t == rtype::common::components::EnemyType::Serpent ||
            t == rtype::common::components::EnemyType::Fortress ||
            t == rtype::common::components::EnemyType::Core) {
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

    // Always update victory visuals (confetti) even while in menu
    updateVictoryEffects(deltaTime);

    if (m_gameStatus == GameStatus::InGameMenu) {
        return; // Don't update game logic when in menu
    }
    
    // Update parallax background
    m_parallaxSystem.update(deltaTime);
    
    // Run ECS systems in order
    updateInputSystem(deltaTime);
    updateFireRateSystem(deltaTime);
    updateEnemyAISystem(deltaTime);
    updateChargedShotSystem(deltaTime);
    updateInvulnerabilitySystem(deltaTime);
    updateAnimationSystem(deltaTime);
    rtype::common::systems::MovementSystem::update(m_world, deltaTime); // shared movement system
    rtype::common::systems::FortressShieldSystem::update(m_world, deltaTime); // shield sync system
    updateCollisionSystem();
    updateCleanupSystem(deltaTime);
}

void GameState::updateBossMusicState() {
    bool bossAlive = isBossActive();
    if (bossAlive && !m_bossMusicActive) {
        // Choose boss track by current level index
        AudioFactory::MusicId bossTrack = AudioFactory::MusicId::BossFight1; // level 0
        if (m_levelIndex == 1)      bossTrack = AudioFactory::MusicId::BossFight2; // Serpent
        else if (m_levelIndex == 2) bossTrack = AudioFactory::MusicId::BossFight3; // Fortress
        else if (m_levelIndex >= 3) bossTrack = AudioFactory::MusicId::BossFight4; // Core

        const std::string bossMusic = AudioFactory::getMusicPath(bossTrack);
        if (m_musicManager.loadFromFile(bossMusic)) {
            m_musicManager.setVolume(35.0f);
            m_musicManager.play(true);
            m_bossMusicActive = true;
        } else {
            std::cerr << "GameState: could not load boss music: " << bossMusic << std::endl;
        }
    } else if (!bossAlive && m_bossMusicActive) {
        // Boss defeated: stop boss music and play boss death SFX
        m_musicManager.stop();
        if (m_soundManager.has(AudioFactory::SfxId::BossDeath)) {
            m_soundManager.play(AudioFactory::SfxId::BossDeath);
        }
        m_bossMusicActive = false;

        // If this was the level 4 boss (index 3), end the game with victory
        if (m_levelIndex >= 3) {
            showVictoryScreen();
        } else {
            // Otherwise proceed to next level
            advanceLevel();
        }
    }
}

void GameState::advanceLevel() {
    m_levelIndex += 1;
    std::cout << "[GameState] Advancing to level index: " << m_levelIndex << std::endl;

    // If we've exceeded the final level index (3), return to main menu
    if (m_levelIndex >= 4) {
        std::cout << "[GameState] Final level cleared. Returning to main menu." << std::endl;
        m_musicManager.stop();
        // Persist last level index for menu parallax
        m_stateManager.setLastLevelIndex(m_levelIndex);
        m_stateManager.changeState(std::make_unique<MainMenuState>(m_stateManager));
        return;
    }

    // Load music for the new level and transition parallax accordingly
    loadLevelMusic();
    m_parallaxSystem.transitionToTheme(ParallaxSystem::themeFromLevel(m_levelIndex), 1.0f);
}

void GameState::loadLevelMusic() {
    // Choose music based on current level index so resets keep the correct track
    AudioFactory::MusicId id = AudioFactory::MusicId::Level1;
    if (m_levelIndex == 1) id = AudioFactory::MusicId::Level2;
    else if (m_levelIndex == 2) id = AudioFactory::MusicId::Level3;
    else if (m_levelIndex == 3) id = AudioFactory::MusicId::Level4;

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

void GameState::setScoreFromServer(int newScore) {
    m_score = newScore;
    // m_scoreText string is updated during renderHUD, but we can pre-update for immediate reads
    m_scoreText.setString("score " + std::to_string(m_score));
}

// === Victory Confetti Implementation ===
void GameState::spawnVictoryConfetti(std::size_t initialBurst) {
    m_confetti.clear();
    m_confetti.reserve(std::min<std::size_t>(m_confettiMax, initialBurst + 64));
    m_confettiActive = true;
    m_confettiSpawnAccum = 0.f;

    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> distX(0.f, SCREEN_WIDTH);
    std::uniform_real_distribution<float> distVX(-60.f, 60.f);
    std::uniform_real_distribution<float> distVY(80.f, 180.f);
    std::uniform_real_distribution<float> distSize(4.f, 10.f);
    std::uniform_real_distribution<float> distAngle(-180.f, 180.f);
    std::uniform_real_distribution<float> distSpin(-180.f, 180.f);
    std::uniform_real_distribution<float> distLife(3.2f, 5.0f);

    auto randColor = [&]() -> sf::Color {
        static const sf::Color palette[] = {
            sf::Color(255, 99, 132),
            sf::Color(54, 162, 235),
            sf::Color(255, 206, 86),
            sf::Color(75, 192, 192),
            sf::Color(153, 102, 255),
            sf::Color(255, 159, 64)
        };
        std::uniform_int_distribution<int> pick(0, (int)(sizeof(palette)/sizeof(palette[0])) - 1);
        sf::Color c = palette[pick(rng)];
        c.a = 230;
        return c;
    };

    auto spawnOne = [&](float x) {
        ConfettiParticle p;
        p.pos = {x, -10.f};
        p.vel = {distVX(rng), distVY(rng)};
        p.rotation = distAngle(rng);
        p.angular = distSpin(rng);
        p.color = randColor();
        p.size = distSize(rng);
        p.life = distLife(rng);
        p.age = 0.f;
        m_confetti.push_back(p);
    };

    for (std::size_t i = 0; i < initialBurst && m_confetti.size() < m_confettiMax; ++i) {
        spawnOne(distX(rng));
    }
}

void GameState::updateVictoryEffects(float deltaTime) {
    if (!m_confettiActive || !m_isVictory) return;

    // Continuous spawn while in victory screen
    if (m_confetti.size() < m_confettiMax) {
        m_confettiSpawnAccum += deltaTime * m_confettiSpawnRate;
        std::size_t toSpawn = static_cast<std::size_t>(m_confettiSpawnAccum);
        if (toSpawn > 0) {
            m_confettiSpawnAccum -= static_cast<float>(toSpawn);
            // We reuse spawnVictoryConfetti to append by temporarily storing current and merging
            // Simpler: spawn directly in place
            static std::mt19937 rng{std::random_device{}()};
            std::uniform_real_distribution<float> distX(0.f, SCREEN_WIDTH);
            std::uniform_real_distribution<float> distVX(-60.f, 60.f);
            std::uniform_real_distribution<float> distVY(80.f, 180.f);
            std::uniform_real_distribution<float> distSize(4.f, 10.f);
            std::uniform_real_distribution<float> distAngle(-180.f, 180.f);
            std::uniform_real_distribution<float> distSpin(-180.f, 180.f);
            std::uniform_real_distribution<float> distLife(3.2f, 5.0f);
            auto randColor = [&]() -> sf::Color {
                static const sf::Color palette[] = {
                    sf::Color(255, 99, 132), sf::Color(54, 162, 235),
                    sf::Color(255, 206, 86), sf::Color(75, 192, 192),
                    sf::Color(153, 102, 255), sf::Color(255, 159, 64)
                };
                std::uniform_int_distribution<int> pick(0, (int)(sizeof(palette)/sizeof(palette[0])) - 1);
                sf::Color c = palette[pick(rng)];
                c.a = 230; return c;
            };
            std::size_t canSpawn = std::min<std::size_t>(toSpawn, m_confettiMax - m_confetti.size());
            for (std::size_t i = 0; i < canSpawn; ++i) {
                ConfettiParticle p;
                p.pos = {distX(rng), -10.f};
                p.vel = {distVX(rng), distVY(rng)};
                p.rotation = distAngle(rng);
                p.angular = distSpin(rng);
                p.color = randColor();
                p.size = distSize(rng);
                p.life = distLife(rng);
                p.age = 0.f;
                m_confetti.push_back(p);
            }
        }
    }

    const sf::Vector2f gravity(0.f, 220.f);
    const float angularDrag = 0.98f;
    const float velDrag = 0.995f;

    for (auto &p : m_confetti) {
        p.vel += gravity * deltaTime;
        p.vel.x *= velDrag;
        p.pos += p.vel * deltaTime;
        p.rotation += p.angular * deltaTime;
        p.angular *= angularDrag;
        p.age += deltaTime;
    }

    m_confetti.erase(
        std::remove_if(m_confetti.begin(), m_confetti.end(), [&](const ConfettiParticle& p){
            return p.age > p.life || p.pos.y > SCREEN_HEIGHT + 20.f;
        }),
        m_confetti.end()
    );
}

void GameState::renderVictoryEffects(sf::RenderWindow& window) {
    if (!m_confettiActive || !m_isVictory) return;
    sf::RectangleShape rect;
    for (const auto &p : m_confetti) {
        rect.setSize(sf::Vector2f(p.size, p.size * 0.6f));
        rect.setOrigin(rect.getSize().x * 0.5f, rect.getSize().y * 0.5f);
        rect.setPosition(p.pos);
        rect.setRotation(p.rotation);
        rect.setFillColor(p.color);
        window.draw(rect);
    }
}

void GameState::clearVictoryEffects() {
    m_confetti.clear();
    m_confettiActive = false;
    m_confettiSpawnAccum = 0.f;
}

} // namespace rtype::client::gui
