/**
 * @file GameState.h
 * @brief Space Invaders game state implementation for R-TYPE using ECS
 *
 * This file contains the GameState class which implements the actual gameplay
 * for a Space Invaders style game using a pure ECS architecture.
 * All game entities (player, enemies, projectiles) are managed through the ECS.
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_GAME_STATE_HPP
#define CLIENT_GAME_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "ParallaxSystem.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ECS/ECS.h>
#include <unordered_map>
#include <common/components/Position.h>
#include <common/components/Velocity.h>
#include <common/components/Health.h>
#include <common/components/Team.h>
#include <common/components/Projectile.h>
#include <common/components/Player.h>
#include <common/components/FireRate.h>
#include <common/components/EnemyType.h>
#include <common/systems/ChargedShot.h>
#include <client/components/Sprite.h>
#include <client/components/Animation.h>
#include <vector>
#include <functional>
#include "MusicManager.h"
#include "SoundManager.h"

namespace rtype::client::gui {

    class GameState : public State {
    public:
        enum class GameStatus { Playing, InGameMenu };

        GameState(StateManager& stateManager);
        ~GameState();

        void handleEvent(const sf::Event& event) override;
        void handleKeyPressed(sf::Keyboard::Key key);
        void handleKeyReleased(sf::Keyboard::Key key);
        void handleMenuInput(const sf::Event& event);
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;
        void onEnter() override;
        void onExit() override;

        // Network helpers used by packet handlers
        ECS::EntityID createEnemyFromServer(uint32_t serverId, float x, float y, uint16_t hp, uint16_t enemyType);
        ECS::EntityID createRemotePlayer(const std::string &name, uint32_t serverId);
        ECS::EntityID createProjectileFromServer(uint32_t serverId, uint32_t ownerId, float x, float y, float vx, float vy, uint16_t damage, bool piercing, bool isCharged);
        void updateEntityStateFromServer(uint32_t serverId, float x, float y, uint16_t hp);
        void destroyEntityByServerId(uint32_t serverId);
        void setLocalPlayerServerId(uint32_t serverId); // Set when receiving initial state from server
        void setIsAdmin(bool isAdmin); // Set whether the local player is room admin

        void setMusicMuted(bool muted);
        bool isMusicMuted() const;

    private:
        ECS::World m_world;
        ECS::EntityID m_playerEntity{0};
        // Map between server entity id and local ECS entity id
        std::unordered_map<uint32_t, ECS::EntityID> m_serverEntityMap;
        // Track local player's server ID to filter out own state updates
        uint32_t m_localPlayerServerId{0};
        // Track if local player is room admin (for boss spawning)
        bool m_isAdmin{false};

        ECS::EntityID createPlayer();
        ECS::EntityID createEnemy(float x, float y);
        ECS::EntityID createShooterEnemy(float x, float y);
        ECS::EntityID createBoss(float x, float y);
        ECS::EntityID createPlayerProjectile(float x, float y);
        ECS::EntityID createChargedProjectile(float x, float y);
        ECS::EntityID createEnemyProjectile(float x, float y, float vx = -300.0f, float vy = 0.0f);

        void updateMovementSystem(float deltaTime);
        void updateInputSystem(float deltaTime);
        void updateFireRateSystem(float deltaTime);
        void updateChargedShotSystem(float deltaTime);
        void updateInvulnerabilitySystem(float deltaTime);
        void updateAnimationSystem(float deltaTime);
        void updatePlayerAnimation(ECS::EntityID entity, rtype::client::components::Animation* animation, rtype::client::components::Sprite* sprite, bool isMovingUp);
        void updateEnemyAISystem(float deltaTime);
        void updateCleanupSystem(float deltaTime);
        void updateCollisionSystem();
        void checkPlayerVsEnemiesCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds);
        void checkPlayerProjectilesVsEnemiesCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds, std::vector<ECS::EntityID>& toDestroy);
        void checkEnemyProjectilesVsPlayerCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds, std::vector<ECS::EntityID>& toDestroy);
        void handlePlayerFire();
        void damagePlayer(int damage = 1);
        int getPlayerLives() const;
        bool isBossActive();
        void resetGame();
        void renderEntities(sf::RenderWindow& window);
        void renderHUD(sf::RenderWindow& window);
        void renderGameOverMenu(sf::RenderWindow& window);
        void setupGameOverUI();
        void loadHUDTextures();
        void showInGameMenu(bool isGameOver = false);
        void resumeGame();
        void updateBossMusicState();
        void loadLevelMusic();

        StateManager& m_stateManager;
        MusicManager m_musicManager;
        bool m_bossMusicActive{false};
        SoundManager m_soundManager;
        bool loadGameSounds();
        GameStatus m_gameStatus{GameStatus::Playing};
        bool m_isGameOver{false};
        int m_selectedMenuOption{0};
        sf::Text m_gameOverTitleText;
        sf::Text m_restartText;
        sf::Text m_menuText;
        sf::Texture m_heartTexture;
        bool m_texturesLoaded{false};
        sf::Sprite m_fullHeartSprite;
        sf::Sprite m_emptyHeartSprite;
        ParallaxSystem m_parallaxSystem;
        static constexpr float ENEMY_FIRE_INTERVAL{2.5f};
        static constexpr float INVULNERABILITY_DURATION{2.0f};
        static constexpr float FIRE_COOLDOWN{0.2f};
        static constexpr size_t MAX_ENEMIES{10};
        bool m_keyUp{false};
        bool m_keyDown{false};
        bool m_keyLeft{false};
        bool m_keyRight{false};
        bool m_keyFire{false};
        static constexpr float SCREEN_WIDTH{1280.0f};
        static constexpr float SCREEN_HEIGHT{720.0f};
    };

    // Global pointer to the active GameState (set when GameState constructed)
    extern GameState* g_gameState;

} // namespace rtype::client::gui

#endif // CLIENT_GAME_STATE_HPP
