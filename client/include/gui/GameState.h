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
#include <common/systems/ChargedShotSystem.h>
#include <client/components/Components.h>
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
        /* === ECS and Entity Management === */
        /// Core ECS world containing all game entities and their components
        ECS::World m_world;
        /// Entity ID of the local player
        ECS::EntityID m_playerEntity{0};
        /// Map between server entity id and local ECS entity id
        std::unordered_map<uint32_t, ECS::EntityID> m_serverEntityMap;
        /// Track local player's server ID to filter out own state updates
        uint32_t m_localPlayerServerId{0};
        /// Track if local player is room admin (for boss spawning)
        bool m_isAdmin{false};

        /* === Entity Factory Methods === */
        /**
         * @brief Create the local player entity
         * @return Entity ID of the created player
         *
         * Creates the main player entity with Position, Velocity, Sprite, Health,
         * Team, Player, FireRate, and ChargedShot components.
         */
        ECS::EntityID createPlayer();

        /**
         * @brief Create a basic enemy entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created enemy
         *
         * Creates a basic enemy with 1 HP that moves left at constant speed.
         * Includes Position, Velocity, Health, Sprite, Team, EnemyType, and FireRate.
         */
        ECS::EntityID createEnemy(float x, float y);

        /**
         * @brief Create a snake-type enemy entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created snake enemy
         *
         * Creates a snake-type enemy with 1 HP that moves in a sine wave pattern.
         */
        ECS::EntityID createSnakeEnemy(float x, float y);

        /**
         * @brief Create a shooter enemy entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created shooter enemy
         *
         * Creates a shooter-type enemy with 3 HP that fires projectiles at regular intervals.
         */
        ECS::EntityID createShooterEnemy(float x, float y);

        /**
         * @brief Create a boss enemy entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created boss
         *
         * Creates a large boss entity with high HP (30) and alternating movement pattern.
         */
        ECS::EntityID createTankDestroyer(float x, float y);

        /**
         * @brief Create a player projectile entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created projectile
         *
         * Creates a standard player projectile moving right at high speed.
         */
        ECS::EntityID createPlayerProjectile(float x, float y);

        /**
         * @brief Create a charged player projectile entity
         * @param x X position
         * @param y Y position
         * @return Entity ID of the created charged projectile
         *
         * Creates a charged projectile with higher damage and piercing capability.
         */
        ECS::EntityID createChargedProjectile(float x, float y);

        /**
         * @brief Create an enemy projectile entity
         * @param x X position
         * @param y Y position
         * @param vx Velocity X (default: -300.0f)
         * @param vy Velocity Y (default: 0.0f)
         * @return Entity ID of the created projectile
         *
         * Creates an enemy projectile with configurable velocity.
         */
        ECS::EntityID createEnemyProjectile(float x, float y, float vx = -300.0f, float vy = 0.0f);

        /**
         * @brief Update input system for player control
         * @param deltaTime Time elapsed since last frame
         *
         * Processes keyboard input state and updates player velocity.
         */
        void updateInputSystem(float deltaTime);

        /**
         * @brief Update fire rate cooldowns for all entities with FireRate component
         * @param deltaTime Time elapsed since last frame
         *
         * Decrements cooldown timers for firing mechanics.
         */
        void updateFireRateSystem(float deltaTime);

        /**
         * @brief Update charged shot system for player
         * @param deltaTime Time elapsed since last frame
         *
         * Manages charged shot accumulation and release for the local player.
         */
        void updateChargedShotSystem(float deltaTime);

        /**
         * @brief Update invulnerability timers for entities
         * @param deltaTime Time elapsed since last frame
         *
         * Decrements invulnerability duration for recently damaged entities.
         */
        void updateInvulnerabilitySystem(float deltaTime);

        /**
         * @brief Update animation system for all animated sprites
         * @param deltaTime Time elapsed since last frame
         *
         * Advances animation frames for entities with Animation component.
         */
        void updateAnimationSystem(float deltaTime);

        /**
         * @brief Update player-specific animation state
         * @param entity Entity ID of the player
         * @param animation Animation component pointer
         * @param sprite Sprite component pointer
         * @param isMovingUp Whether the player is moving upward
         *
         * Adjusts player sprite animation based on movement direction.
         */
        void updatePlayerAnimation(ECS::EntityID entity, rtype::client::components::Animation* animation, rtype::client::components::Sprite* sprite, bool isMovingUp);

        /**
         * @brief Update cleanup system for out-of-bounds entities
         * @param deltaTime Time elapsed since last frame
         *
         * Destroys entities that have left the playable area.
         */
        void updateCleanupSystem(float deltaTime);

        /**
         * @brief Update collision detection and resolution
         *
         * Orchestrates all collision checks between different entity types.
         */
        void updateCollisionSystem();

        /* === Collision Detection Helpers === */
        /**
         * @brief Check collisions between player and enemies
         * @param positions Component array of all positions
         * @param getBounds Function to get bounding box from entity and position
         *
         * Damages player on enemy collision and handles invulnerability.
         */
        void checkPlayerVsEnemiesCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds);

        /**
         * @brief Check collisions between player projectiles and enemies
         * @param positions Component array of all positions
         * @param getBounds Function to get bounding box from entity and position
         * @param toDestroy Vector to accumulate entities to destroy
         *
         * Damages enemies hit by player projectiles and destroys non-piercing projectiles.
         */
        void checkPlayerProjectilesVsEnemiesCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds, std::vector<ECS::EntityID>& toDestroy);

        /**
         * @brief Check collisions between enemy projectiles and player
         * @param positions Component array of all positions
         * @param getBounds Function to get bounding box from entity and position
         * @param toDestroy Vector to accumulate entities to destroy
         *
         * Damages player hit by enemy projectiles and destroys the projectiles.
         */
        void checkEnemyProjectilesVsPlayerCollision(ECS::ComponentArray<rtype::common::components::Position>& positions, const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds, std::vector<ECS::EntityID>& toDestroy);

        /* === Gameplay Logic === */
        /**
         * @brief Handle player firing action
         *
         * Creates projectile if fire cooldown allows, either normal or charged shot.
         */
        void handlePlayerFire();

        /**
         * @brief Apply damage to the player
         * @param damage Amount of damage (default: 1)
         *
         * Reduces player HP, grants invulnerability period, triggers game over if HP reaches 0.
         */
        void damagePlayer(int damage = 1);

        /**
         * @brief Get current player lives/health
         * @return Current HP of the player entity
         */
        int getPlayerLives() const;

        /**
         * @brief Check if a boss entity is currently active
         * @return true if a boss enemy exists in the world
         */
        bool isBossActive();

        /**
         * @brief Reset the game state to initial conditions
         *
         * Clears all entities and reinitializes the player.
         */
        void resetGame();

        /* === Rendering === */
        /**
         * @brief Render all game entities to the window
         * @param window SFML RenderWindow to draw into
         *
         * Draws all entities with Sprite components in the correct order.
         */
        void renderEntities(sf::RenderWindow& window);

        /**
         * @brief Render heads-up display (health, score, etc.)
         * @param window SFML RenderWindow to draw into
         */
        void renderHUD(sf::RenderWindow& window);

        /**
         * @brief Render game over menu overlay
         * @param window SFML RenderWindow to draw into
         */
        void renderGameOverMenu(sf::RenderWindow& window);

        /* === UI and Resource Management === */
        /**
         * @brief Set up game over UI text elements
         *
         * Initializes fonts, positions, and styles for game over screen.
         */
        void setupGameOverUI();

        /**
         * @brief Load HUD textures (hearts, etc.)
         *
         * Preloads sprites used in the heads-up display.
         */
        void loadHUDTextures();

        /**
         * @brief Show the in-game menu overlay
         * @param isGameOver Whether this is the game over menu (default: false)
         *
         * Pauses gameplay and displays menu options.
         */
        void showInGameMenu(bool isGameOver = false);

        /**
         * @brief Resume gameplay from paused/menu state
         *
         * Hides menu and resumes normal game update loop.
         */
        void resumeGame();

        /* === Audio Management === */
        /**
         * @brief Update boss music state based on boss presence
         *
         * Switches between normal and boss music tracks as needed.
         */
        void updateBossMusicState();

        /**
         * @brief Load level background music
         *
         * Initializes and starts playing the level music track.
         */
        void loadLevelMusic();

        /* === State Members === */
        /// Reference to the owning StateManager for state transitions
        StateManager& m_stateManager;
        /// Music manager for background music playback
        MusicManager m_musicManager;
        /// Flag indicating whether boss music is currently playing
        bool m_bossMusicActive{false};
        /// Sound manager for sound effects playback
        SoundManager m_soundManager;

        /**
         * @brief Load game sound effects
         * @return true if sounds loaded successfully
         *
         * Preloads all sound effects used during gameplay.
         */
        bool loadGameSounds();

        /* === UI State === */
        /// Current gameplay/UI status (Playing or InGameMenu)
        GameStatus m_gameStatus{GameStatus::Playing};
        /// Flag indicating game over condition
        bool m_isGameOver{false};
        /// Currently selected menu option index
        int m_selectedMenuOption{0};
        /// SFML text element for game over title
        sf::Text m_gameOverTitleText;
        /// SFML text element for restart option
        sf::Text m_restartText;
        /// SFML text element for menu option
        sf::Text m_menuText;

        /* === HUD Resources === */
        /// Texture for heart sprite (health display)
        sf::Texture m_heartTexture;
        /// Flag indicating whether HUD textures have been loaded
        bool m_texturesLoaded{false};
        /// Sprite for full health heart
        sf::Sprite m_fullHeartSprite;
        /// Sprite for empty/lost health heart
        sf::Sprite m_emptyHeartSprite;

        /* === Rendering === */
        /// Parallax background system for scrolling layers
        ParallaxSystem m_parallaxSystem;

        /* === Game Constants === */
        /// Interval in seconds between enemy projectile shots
        static constexpr float ENEMY_FIRE_INTERVAL{2.5f};
        /// Duration in seconds of player invulnerability after taking damage
        static constexpr float INVULNERABILITY_DURATION{2.0f};
        /// Cooldown in seconds between player shots
        static constexpr float FIRE_COOLDOWN{0.2f};
        /// Maximum number of simultaneous enemies allowed
        static constexpr size_t MAX_ENEMIES{10};

        /* === Input State === */
        /// Flag for up arrow key state
        bool m_keyUp{false};
        /// Flag for down arrow key state
        bool m_keyDown{false};
        /// Flag for left arrow key state
        bool m_keyLeft{false};
        /// Flag for right arrow key state
        bool m_keyRight{false};
        /// Flag for fire key state
        bool m_keyFire{false};

        /* === Screen Dimensions === */
        /// Screen width in pixels
        static constexpr float SCREEN_WIDTH{1280.0f};
        /// Screen height in pixels
        static constexpr float SCREEN_HEIGHT{720.0f};
    };

    // Global pointer to the active GameState (set when GameState constructed)
    extern GameState* g_gameState;

} // namespace rtype::client::gui

#endif // CLIENT_GAME_STATE_HPP
