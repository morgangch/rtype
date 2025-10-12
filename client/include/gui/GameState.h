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
#include <ECS/ECS.h>
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

namespace rtype::client::gui {
    
    /**
     * @class GameState
     * @brief The in-game state for Space Invaders gameplay using pure ECS
     * 
     * GameState handles the core game loop using Entity Component System architecture:
     * - All entities (player, enemies, projectiles) are ECS entities
     * - Components define entity properties (Position, Velocity, Health, etc.)
     * - Systems update entity behavior each frame
     * 
     * ECS Components Used:
     * - Position: Entity location in 2D space
     * - Velocity: Movement speed and direction
     * - Health: Hit points, alive state, and invulnerability (common)
     * - Team: Distinguishes player/enemy entities (common)
     * - Player: Marks player-controlled entities (common)
     * - FireRate: Shooting cooldown timer (common)
     * - Projectile: Projectile-specific data (common)
     * - Sprite: Visual representation for rendering (client)
     * 
     * Controls:
     * - Z/Up: Move up
     * - S/Down: Move down
     * - Q/Left: Move left
     * - D/Right: Move right
     * - Space: Fire projectiles
     * - ESC: Return to menu
     */
    class GameState : public State {
    public:
        /**
         * @enum GameStatus
         * @brief Represents the current state of the game
         */
        enum class GameStatus {
            Playing,     ///< Game is active
            InGameMenu   ///< Paused or game over, showing in-game menu
        };
        
        /**
         * @brief Construct a new GameState
         * @param stateManager Reference to the state manager for transitions
         */
        GameState(StateManager& stateManager);
        
        /**
         * @brief Handle input events (keyboard, window)
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event) override;
        
        /**
         * @brief Handle key press events
         * @param key The SFML key code that was pressed
         */
        void handleKeyPressed(sf::Keyboard::Key key);

        /**
         * @brief Handle key release events
         * @param key The SFML key code that was released
         */
        void handleKeyReleased(sf::Keyboard::Key key);
        
        /**
         * @brief Handle input events while in menu state
         * @param event The SFML event to process
         */
        void handleMenuInput(const sf::Event& event);

        /**
         * @brief Update game logic (player, enemies, collisions)
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override;
        
        /**
         * @brief Render the game to the screen
         * @param window The render window to draw to
         */
        void render(sf::RenderWindow& window) override;
        
        /**
         * @brief Called when entering this state
         */
        void onEnter() override;
        
        /**
         * @brief Called when exiting this state
         */
        void onExit() override;
        
    private:
        
        /**
         * @brief ECS World instance for entity management
         * 
         * The world manages all entities and their components.
         * All game objects (player, enemies, projectiles) are entities in this world.
         */
        ECS::World m_world;
        
        /**
         * @brief Entity ID for the player
         * 
         * The player is a single entity with components:
         * - Position, Velocity, Health, Sprite, Controllable, FireRate, Invulnerability
         */
        ECS::EntityID m_playerEntity{0};

        /**
         * @brief Create the player entity
         * @note Wrapper around rtype::client::factories::createPlayer()
         */
        ECS::EntityID createPlayer();
        
        /**
         * @brief Create a basic enemy entity
         * @note Wrapper around rtype::client::factories::createEnemy()
         */
        ECS::EntityID createEnemy(float x, float y);
        
        /**
         * @brief Create a shooter enemy entity
         * @note Wrapper around rtype::client::factories::createShooterEnemy()
         */
        ECS::EntityID createShooterEnemy(float x, float y);
        
        /**
         * @brief Create a boss enemy entity
         * @note Wrapper around rtype::client::factories::createBoss()
         */
        ECS::EntityID createBoss(float x, float y);
        
        /**
         * @brief Create a player projectile
         * @note Wrapper around rtype::client::factories::createPlayerProjectile()
         */
        ECS::EntityID createPlayerProjectile(float x, float y);
        
        /**
         * @brief Create a charged player projectile
         * @note Wrapper around rtype::client::factories::createChargedProjectile()
         */
        ECS::EntityID createChargedProjectile(float x, float y);
        
        /**
         * @brief Create an enemy projectile
         * @note Wrapper around rtype::client::factories::createEnemyProjectile()
         */
        ECS::EntityID createEnemyProjectile(float x, float y, float vx = -300.0f, float vy = 0.0f);

        /**
         * @brief Movement System - Updates positions based on velocity
         * @note Wrapper around rtype::client::systems::updateMovementSystem()
         */
        void updateMovementSystem(float deltaTime);
        
        /**
         * @brief Input System - Process player input
         * @note Wrapper around rtype::client::systems::updateInputSystem()
         */
        void updateInputSystem(float deltaTime);
        
        /**
         * @brief Fire Rate System - Update shooting cooldowns
         * @note Wrapper around rtype::client::systems::updateFireRateSystem()
         */
        void updateFireRateSystem(float deltaTime);
        
        /**
         * @brief Charged Shot System - Update charge accumulation
         * @note Wrapper around rtype::client::systems::updateChargedShotSystem()
         */
        void updateChargedShotSystem(float deltaTime);
        
        /**
         * @brief Invulnerability System - Update invulnerability timers
         * @note Wrapper around rtype::client::systems::updateInvulnerabilitySystem()
         */
        void updateInvulnerabilitySystem(float deltaTime);
        
        /**
         * @brief Animation System - Update sprite animations (frame cycling)
         */
        void updateAnimationSystem(float deltaTime);
        
        /**
         * @brief Handle player animation based on movement input
         * @param entity The entity to update animation for
         * @param animation The animation component
         * @param sprite The sprite component
         * @param isMovingUp Whether the UP key is pressed
         */
        void updatePlayerAnimation(ECS::EntityID entity, 
                                   rtype::client::components::Animation* animation,
                                   rtype::client::components::Sprite* sprite,
                                   bool isMovingUp);
        
        /**
         * @brief Enemy Spawning System - Spawn enemies periodically
         * @note Wrapper around rtype::client::systems::updateEnemySpawnSystem()
         */
        void updateEnemySpawnSystem(float deltaTime);
        
        /**
         * @brief Enemy AI System - Handle enemy shooting
         * @note Wrapper around rtype::client::systems::updateEnemyAISystem()
         */
        void updateEnemyAISystem(float deltaTime);
        
        /**
         * @brief Cleanup System - Remove off-screen entities
         * @note Wrapper around rtype::client::systems::updateCleanupSystem()
         */
        void updateCleanupSystem(float deltaTime);
        
        /**
         * @brief Collision System - Detect and handle all collisions
         * @note Wrapper around rtype::client::collision::updateCollisionSystem()
         */
        void updateCollisionSystem();
        
        /**
         * @brief Check player vs enemies collision
         * @note Wrapper around rtype::client::collision::checkPlayerVsEnemiesCollision()
         */
        void checkPlayerVsEnemiesCollision(
            ECS::ComponentArray<rtype::common::components::Position>& positions,
            const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds);
        
        /**
         * @brief Check player projectiles vs enemies collision
         * @note Wrapper around rtype::client::collision::checkPlayerProjectilesVsEnemiesCollision()
         */
        void checkPlayerProjectilesVsEnemiesCollision(
            ECS::ComponentArray<rtype::common::components::Position>& positions,
            const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
            std::vector<ECS::EntityID>& toDestroy);
        
        /**
         * @brief Check enemy projectiles vs player collision
         * @note Wrapper around rtype::client::collision::checkEnemyProjectilesVsPlayerCollision()
         */
        void checkEnemyProjectilesVsPlayerCollision(
            ECS::ComponentArray<rtype::common::components::Position>& positions,
            const std::function<sf::FloatRect(ECS::EntityID, const rtype::common::components::Position&)>& getBounds,
            std::vector<ECS::EntityID>& toDestroy);
        
        /**
         * @brief Handle player firing a projectile
         * 
         * Called when space bar is pressed and fire cooldown allows.
         * Creates a player projectile entity at player's position.
         */
        void handlePlayerFire();
        
        /**
         * @brief Handle player taking damage
         * @param damage Amount of damage to apply
         * 
         * Reduces player health, grants invulnerability, checks for game over.
         * Only applies damage if player is not currently invulnerable.
         */
        void damagePlayer(int damage = 1);
        
        /**
         * @brief Get the player's current number of lives
         * @return Number of hit points remaining (0 if player is dead/doesn't exist)
         * 
         * Helper method to access player health for HUD rendering.
         */
        int getPlayerLives() const;
        
        /**
         * @brief Check if a boss is currently active in the game
         * @return True if a boss entity exists, false otherwise
         * 
         * Pure ECS approach: queries the world for entities with Boss type.
         * Replaces the need for a m_bossActive flag.
         */
        bool isBossActive();
        
        /**
         * @brief Reset the game to initial state
         * 
         * Resets player position, lives to 3, and clears all enemies and projectiles.
         * Called when starting a new game or when player loses all lives.
         */
        void resetGame();
        
        /**
         * @brief Render System - Draw all entities with Sprite components
         * @param window The render window to draw to
         * 
         * Iterates all entities with Position + Sprite components.
         * Draws each entity as a rectangle with appropriate color.
         * Handles invulnerability blinking for entities with Invulnerability component.
         */
        void renderEntities(sf::RenderWindow& window);
        
        /**
         * @brief Render the player's lives/HUD
         * @param window The render window to draw to
         * 
         * Draws the player's remaining lives count in the top-left corner.
         */
        void renderHUD(sf::RenderWindow& window);
        
        /**
         * @brief Render the game over menu
         * @param window The render window to draw to
         * 
         * Draws the game over screen with options to restart or return to menu.
         */
        void renderGameOverMenu(sf::RenderWindow& window);
        
        /**
         * @brief Initialize the In-Game Menu UI elements (text, buttons)
         * 
         * Sets up the sf::Text objects for the in-game menu using the
         * centralized GUIHelper utilities for consistent styling.
         * Called once in the constructor.
         */
        void setupGameOverUI();
        
        /**
         * @brief Show the in-game menu (pause or game over)
         * 
         * Transitions to in-game menu state and displays menu options.
         * Resets all input states to prevent stuck keys.
         * 
         * @param isGameOver True if player died (game over), false if paused
         */
        void showInGameMenu(bool isGameOver = false);
        
        /**
         * @brief Resume the game from in-game menu
         * 
         * Returns to playing state and resets all input states to ensure
         * no keys remain in "pressed" state from the menu.
         */
        void resumeGame();
        
        /**
         * @brief Reference to the state manager for state transitions
         */
        StateManager& m_stateManager;
        
        /**
         * @brief Current game status (Playing or InGameMenu)
         */
        GameStatus m_gameStatus{GameStatus::Playing};
        
        /**
         * @brief Flag indicating if menu is shown due to game over (true) or pause (false)
         */
        bool m_isGameOver{false};
        
        /**
         * @brief Selected menu option in in-game menu (0 = Restart/Resume, 1 = Menu)
         */
        int m_selectedMenuOption{0};
        
        /**
         * @brief Text object for "GAME OVER" title displayed in game over menu
         */
        sf::Text m_gameOverTitleText;
        
        /**
         * @brief Text object for "Restart" button in game over menu
         */
        sf::Text m_restartText;
        
        /**
         * @brief Text object for "Return to Menu" button in game over menu
         */
        sf::Text m_menuText;
        
        /**
         * @brief Texture for heart sprites (HUD lives display)
         */
        sf::Texture m_heartTexture;
        
        /**
         * @brief Sprite for full heart (alive)
         */
        sf::Sprite m_fullHeartSprite;
        
        /**
         * @brief Sprite for empty heart (lost life)
         */
        sf::Sprite m_emptyHeartSprite;
        
        /**
         * @brief Parallax background system for space environment
         */
        ParallaxSystem m_parallaxSystem;
        
        /**
         * @brief Timer for enemy spawning
         * 
         * Accumulates delta time. When it exceeds ENEMY_SPAWN_INTERVAL,
         * a new enemy is spawned and the timer resets.
         */
        float m_enemySpawnTimer{0.0f};
        
        /**
         * @brief Timer for boss spawning
         * 
         * Accumulates delta time. When it exceeds BOSS_SPAWN_INTERVAL (180s = 3 min),
         * a boss is spawned and the timer resets.
         */
        float m_bossSpawnTimer{0.0f};
        
        /**
         * @brief Interval between enemy spawns in seconds
         */
        static constexpr float ENEMY_SPAWN_INTERVAL{2.0f};
        
        /**
         * @brief Interval between boss spawns in seconds (3 minutes)
         */
        static constexpr float BOSS_SPAWN_INTERVAL{180.0f};
        
        /**
         * @brief Interval between enemy shots in seconds
         */
        static constexpr float ENEMY_FIRE_INTERVAL{2.5f};
        
        /**
         * @brief Duration of player invulnerability after taking damage (seconds)
         */
        static constexpr float INVULNERABILITY_DURATION{2.0f};
        
        /**
         * @brief Minimum time between shots in seconds
         */
        static constexpr float FIRE_COOLDOWN{0.2f};
        
        /**
         * @brief Maximum number of simultaneous enemies
         * 
         * Prevents excessive enemy spawning that could impact performance.
         */
        static constexpr size_t MAX_ENEMIES{10};
        
        /**
         * @brief Input state: Up key (Z or Arrow Up) is pressed
         */
        bool m_keyUp{false};
        
        /**
         * @brief Input state: Down key (S or Arrow Down) is pressed
         */
        bool m_keyDown{false};
        
        /**
         * @brief Input state: Left key (Q or Arrow Left) is pressed
         */
        bool m_keyLeft{false};
        
        /**
         * @brief Input state: Right key (D or Arrow Right) is pressed
         */
        bool m_keyRight{false};
        
        /**
         * @brief Input state: Fire key (Space) is pressed
         * 
         * Used to trigger projectile firing with rate limiting via FIRE_COOLDOWN.
         */
        bool m_keyFire{false};
        
        /**
         * @brief Screen width in pixels
         */
        static constexpr float SCREEN_WIDTH{1280.0f};
        
        /**
         * @brief Screen height in pixels
         */
        static constexpr float SCREEN_HEIGHT{720.0f};
    };
    
} // namespace rtype::client::gui

#endif // CLIENT_GAME_STATE_HPP
