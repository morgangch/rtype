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

    /**
     * @class GameState
     * @brief Main gameplay state using ECS for entities and systems
     *
     * GameState contains the in-game simulation for the R-TYPE client. It
     * orchestrates ECS systems (movement, AI, collisions, rendering), manages
     * creation of entities via factory methods, and exposes a small API used
     * by network packet handlers to create/update/destroy server-owned entities.
     *
     * Responsibilities:
     * - Host the ECS::World instance used for all game entities
     * - Provide network-aware factory methods (createEnemyFromServer, createProjectileFromServer)
     * - Run the per-frame update pipeline in the correct order for deterministic
     *   and responsive gameplay (input → simulation → collision → render)
     * - Play music and sound effects and manage HUD/UI for the in-game menu
     *
     * Notes on network model:
     * - The server is authoritative: the client receives spawn/update/destroy packets
     *   and maps server entity IDs to local ECS entity IDs via m_serverEntityMap.
     * - The client performs optimistic prediction for low-latency feedback but
     *   respects server confirmation (ENTITY_DESTROY, authoritative HP, positions).
     */
    class GameState : public State {
    public:
        /**
         * @brief Overall UI/gameplay status for the state
         */
        enum class GameStatus { Playing, InGameMenu };

        /**
         * @brief Construct a GameState bound to a StateManager
         * @param stateManager Reference to the owning StateManager
         */
        GameState(StateManager& stateManager);

        /**
         * @brief Destroy the GameState
         *
         * Ensures global pointers and resources are cleaned up.
         */
        ~GameState();

        /**
         * @brief Handle generic SFML events
         * @param event The SFML event to handle
         */
        void handleEvent(const sf::Event& event) override;

        /**
         * @brief Handle a key pressed event
         * @param key SFML key code
         */
        void handleKeyPressed(sf::Keyboard::Key key);

        /**
         * @brief Handle a key released event
         * @param key SFML key code
         */
        void handleKeyReleased(sf::Keyboard::Key key);

        /**
         * @brief Handle input specific to in-game menus
         * @param event The SFML event to handle
         */
        void handleMenuInput(const sf::Event& event);

        /**
         * @brief Per-frame update called by the StateManager
         * @param deltaTime Time elapsed since last frame (seconds)
         */
        void update(float deltaTime) override;

        /**
         * @brief Render current game state to the provided window
         * @param window SFML RenderWindow to draw into
         */
        void render(sf::RenderWindow& window) override;

        /**
         * @brief Called when this state becomes active
         */
        void onEnter() override;

        /**
         * @brief Called when this state is exited
         */
        void onExit() override;

        /* === Network-aware helpers (used by packet handlers) === */
        /**
         * @brief Create or update an enemy entity based on server spawn packet
         * @param serverId Server-assigned entity id
         * @param x X position
         * @param y Y position
         * @param hp Health value (server authoritative)
         * @param enemyType Numeric enemy type (maps to EnemyType enum)
         * @return Local ECS::EntityID of the created/updated entity
         *
         * This function maps a server entity id to a local entity and ensures
         * the appropriate factory (createBoss/createShooter/createEnemy) is used.
         */
        ECS::EntityID createEnemyFromServer(uint32_t serverId, float x, float y, uint16_t hp, uint16_t enemyType);

        /**
         * @brief Create a remote player representation for another client
         * @param name Player display name
         * @param serverId Server entity id
         * @return Local ECS::EntityID for the remote player entity
         */
        ECS::EntityID createRemotePlayer(const std::string &name, uint32_t serverId);

        /**
         * @brief Create a projectile spawned by the server
         * @param serverId Server-assigned projectile id
         * @param ownerId Server id of projectile owner
         * @param x X position
         * @param y Y position
         * @param vx Velocity X
         * @param vy Velocity Y
         * @param damage Damage value
         * @param piercing Whether projectile pierces
         * @param isCharged Whether projectile is charged (visual/style)
         * @return Local ECS::EntityID of the projectile
         */
        ECS::EntityID createProjectileFromServer(uint32_t serverId, uint32_t ownerId, float x, float y, float vx, float vy, uint16_t damage, bool piercing, bool isCharged);

        /**
         * @brief Update an entity position and HP based on authoritative server snapshot
         * @param serverId Server entity id
         * @param x X position
         * @param y Y position
         * @param hp Health value
         */
        void updateEntityStateFromServer(uint32_t serverId, float x, float y, uint16_t hp);

        /**
         * @brief Destroy a local entity corresponding to a server entity id
         * @param serverId Server entity id to destroy
         */
        void destroyEntityByServerId(uint32_t serverId);

        /**
         * @brief Set the server ID of the local player
         * @param serverId Local player's server id (used to ignore server echoes)
         */
        void setLocalPlayerServerId(uint32_t serverId); // Set when receiving initial state from server

        /**
         * @brief Mark the local player as room admin (enables admin-only input such as boss spawn)
         * @param isAdmin True to mark as admin
         */
        void setIsAdmin(bool isAdmin); // Set whether the local player is room admin

        /**
         * @brief Mute/unmute music
         */
        void setMusicMuted(bool muted);

        /**
         * @brief Query whether music is muted
         * @return true if muted
         */
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
