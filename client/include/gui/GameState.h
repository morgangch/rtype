/**
 * @file GameState.h
 * @brief Space Invaders game state implementation for R-TYPE
 * 
 * This file contains the GameState class which implements the actual gameplay
 * for a Space Invaders style game. The player controls a ship and must avoid
 * enemies scrolling from right to left.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_GAME_STATE_HPP
#define CLIENT_GAME_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace rtype::client::gui {
    
    /**
     * @class GameState
     * @brief The in-game state for Space Invaders gameplay
     * 
     * GameState handles the core game loop including:
     * - Player ship movement (ZQSD/Arrow keys)
     * - Projectile firing (Space bar)
     * - Enemy spawning and movement
     * - Collision detection (player-enemy and projectile-enemy)
     * - Game rendering
     * 
     * Features:
     * - Smooth player movement in all directions
     * - Projectile system with fire rate cooldown
     * - Automatic enemy spawning from the right
     * - Simple AABB collision detection
     * - Enemy health system with damage
     * - Space-themed visuals with starfield background
     * - ESC to return to main menu
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
         * @struct Player
         * @brief Represents the player's ship in the game
         * 
         * The Player structure contains all data related to the player's ship including
         * position, size, speed, health/lives, and collision bounds calculation.
         */
        struct Player {
            /** @brief Current position of the player (center point) */
            sf::Vector2f position{100.0f, 360.0f};  // Centered vertically
            
            /** @brief Size of the player ship (width, height) */
            sf::Vector2f size{32.0f, 32.0f};
            
            /** @brief Movement speed in pixels per second */
            float speed{300.0f};
            
            /** @brief Current number of lives remaining */
            int lives{3};
            
            /** @brief Invulnerability timer after taking damage (seconds) */
            float invulnerabilityTimer{0.0f};
            
            /**
             * @brief Check if player is currently invulnerable
             * @return True if player has temporary invulnerability
             */
            bool isInvulnerable() const {
                return invulnerabilityTimer > 0.0f;
            }
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the player's collision box
             * 
             * Calculates the bounding rectangle centered on the player's position.
             * Used for AABB collision detection with enemies and enemy projectiles.
             */
            sf::FloatRect getBounds() const {
                return sf::FloatRect(
                    position.x - size.x * 0.5f,
                    position.y - size.y * 0.5f,
                    size.x,
                    size.y
                );
            }
        };
        
        /**
         * @struct Enemy
         * @brief Represents an enemy ship in the game
         * 
         * The Enemy structure contains all data for enemy ships that move from
         * right to left across the screen and can fire projectiles.
         */
        struct Enemy {
            /** @brief Current position of the enemy (center point) */
            sf::Vector2f position;
            
            /** @brief Size of the enemy ship (width, height) */
            sf::Vector2f size{24.0f, 24.0f};
            
            /** @brief Movement speed in pixels per second (leftward) */
            float speed{100.0f};
            
            /** @brief Health points of the enemy */
            int health{1};
            
            /** @brief Timer for enemy fire rate */
            float fireTimer{0.0f};
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the enemy's collision box
             * 
             * Calculates the bounding rectangle centered on the enemy's position.
             * Used for AABB collision detection with the player and projectiles.
             */
            sf::FloatRect getBounds() const {
                return sf::FloatRect(
                    position.x - size.x * 0.5f,
                    position.y - size.y * 0.5f,
                    size.x,
                    size.y
                );
            }
        };
        
        /**
         * @struct Projectile
         * @brief Represents a projectile fired by the player
         * 
         * The Projectile structure contains data for player projectiles that
         * move from left to right across the screen to destroy enemies.
         */
        struct Projectile {
            /** @brief Current position of the projectile (center point) */
            sf::Vector2f position;
            
            /** @brief Size of the projectile (width, height) */
            sf::Vector2f size{12.0f, 4.0f};
            
            /** @brief Movement speed in pixels per second (rightward) */
            float speed{500.0f};
            
            /** @brief Damage dealt to enemies on hit */
            int damage{1};
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the projectile's collision box
             * 
             * Calculates the bounding rectangle centered on the projectile's position.
             * Used for AABB collision detection with enemies.
             */
            sf::FloatRect getBounds() const {
                return sf::FloatRect(
                    position.x - size.x * 0.5f,
                    position.y - size.y * 0.5f,
                    size.x,
                    size.y
                );
            }
        };
        
        /**
         * @struct EnemyProjectile
         * @brief Represents a projectile fired by enemies
         * 
         * The EnemyProjectile structure contains data for enemy projectiles that
         * move from right to left across the screen to damage the player.
         */
        struct EnemyProjectile {
            /** @brief Current position of the enemy projectile (center point) */
            sf::Vector2f position;
            
            /** @brief Size of the projectile (width, height) */
            sf::Vector2f size{10.0f, 4.0f};
            
            /** @brief Movement speed in pixels per second (leftward, slower than player shots) */
            float speed{300.0f};
            
            /** @brief Damage dealt to player on hit */
            int damage{1};
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the enemy projectile's collision box
             * 
             * Calculates the bounding rectangle centered on the projectile's position.
             * Used for AABB collision detection with the player.
             */
            sf::FloatRect getBounds() const {
                return sf::FloatRect(
                    position.x - size.x * 0.5f,
                    position.y - size.y * 0.5f,
                    size.x,
                    size.y
                );
            }
        };
        
        /**
         * @brief Update the player's position based on input
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Moves the player ship based on current input state (ZQSD/Arrow keys).
         * Handles diagonal movement normalization and screen boundary clamping.
         * Player can move freely across the entire screen.
         * Also updates invulnerability timer.
         */
        void updatePlayer(float deltaTime);
        
        /**
         * @brief Update all enemies' positions and remove off-screen ones
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Moves all enemies from right to left. Removes enemies that have
         * moved completely off the left side of the screen.
         * Updates enemy fire timers and triggers enemy shots.
         */
        void updateEnemies(float deltaTime);
        
        /**
         * @brief Update all projectiles' positions and remove off-screen ones
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Moves all projectiles from left to right. Removes projectiles that have
         * moved completely off the right side of the screen.
         */
        void updateProjectiles(float deltaTime);
        
        /**
         * @brief Update all enemy projectiles' positions and remove off-screen ones
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Moves all enemy projectiles from right to left. Removes projectiles that have
         * moved completely off the left side of the screen.
         */
        void updateEnemyProjectiles(float deltaTime);
        
        /**
         * @brief Check for collisions between player and enemies
         * 
         * Uses AABB (Axis-Aligned Bounding Box) collision detection.
         * If a collision is detected and player is not invulnerable,
         * the player loses one life.
         */
        void checkCollisions();
        
        /**
         * @brief Check for collisions between projectiles and enemies
         * 
         * Uses AABB collision detection. When a projectile hits an enemy:
         * - The enemy takes damage
         * - The projectile is destroyed
         * - If enemy health reaches 0, the enemy is destroyed
         */
        void checkProjectileCollisions();
        
        /**
         * @brief Check for collisions between enemy projectiles and player
         * 
         * Uses AABB collision detection. When an enemy projectile hits the player:
         * - The player loses one life if not invulnerable
         * - The projectile is destroyed
         * - Player gains temporary invulnerability
         * - If player lives reach 0, game over
         */
        void checkEnemyProjectileCollisions();
        
        /**
         * @brief Spawn a new enemy at a random vertical position
         * 
         * Creates a new enemy on the right edge of the screen at a random
         * Y position. Only spawns if the current enemy count is below MAX_ENEMIES.
         */
        void spawnEnemy();
        
        /**
         * @brief Fire a projectile from the player's position
         * 
         * Creates a new projectile at the player's current position,
         * shooting to the right. Respects the fire rate cooldown.
         */
        void fireProjectile();
        
        /**
         * @brief Fire an enemy projectile from an enemy's position
         * @param enemyPosition Position of the enemy firing the projectile
         * 
         * Creates a new enemy projectile at the specified position,
         * shooting to the left towards the player.
         */
        void fireEnemyProjectile(const sf::Vector2f& enemyPosition);
        
        /**
         * @brief Handle player taking damage
         * 
         * Reduces player lives by 1, grants invulnerability,
         * and checks for game over condition.
         */
        void damagePlayer();
        
        /**
         * @brief Reset the game to initial state
         * 
         * Resets player position, lives to 3, and clears all enemies and projectiles.
         * Called when starting a new game or when player loses all lives.
         */
        void resetGame();
        
        /**
         * @brief Render the animated starfield background
         * @param window The render window to draw to
         * 
         * Draws a space-themed background with moving stars to create
         * a parallax scrolling effect.
         */
        void renderStarfield(sf::RenderWindow& window);
        
        /**
         * @brief Render the player ship
         * @param window The render window to draw to
         * 
         * Draws the player's ship as a green rectangle.
         * Flashes the ship during invulnerability period.
         */
        void renderPlayer(sf::RenderWindow& window);
        
        /**
         * @brief Render all enemy ships
         * @param window The render window to draw to
         * 
         * Draws all active enemies as red triangles pointing left.
         */
        void renderEnemies(sf::RenderWindow& window);
        
        /**
         * @brief Render all projectiles
         * @param window The render window to draw to
         * 
         * Draws all active projectiles as yellow/white rectangles moving right.
         */
        void renderProjectiles(sf::RenderWindow& window);
        
        /**
         * @brief Render all enemy projectiles
         * @param window The render window to draw to
         * 
         * Draws all active enemy projectiles as red rectangles moving left.
         */
        void renderEnemyProjectiles(sf::RenderWindow& window);
        
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
         * 
         * @param isGameOver True if player died (game over), false if paused
         */
        void showInGameMenu(bool isGameOver = false);
        
        /**
         * @brief Get the current movement vector from input state
         * @return sf::Vector2f Normalized movement direction vector
         * 
         * Combines keyboard input (ZQSD/Arrow keys) into a movement vector.
         * The vector is normalized for diagonal movement to prevent faster diagonal speed.
         */
        sf::Vector2f getMovementInput() const;
        
        /**
         * @brief Reference to the state manager for state transitions
         * 
         * Used to switch between game states (e.g., return to main menu).
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
         * @brief The player's ship data
         * 
         * Contains position, size, speed, and collision information for the player.
         */
        Player m_player;
        
        /**
         * @brief Active enemy ships in the game
         * 
         * Vector of all currently active enemies. Enemies are added via spawnEnemy()
         * and removed when they move off-screen or on collision.
         */
        std::vector<Enemy> m_enemies;
        
        /**
         * @brief Active projectiles fired by the player
         * 
         * Vector of all currently active projectiles. Projectiles are added via fireProjectile()
         * and removed when they move off-screen or hit an enemy.
         */
        std::vector<Projectile> m_projectiles;
        
        /**
         * @brief Active projectiles fired by enemies
         * 
         * Vector of all currently active enemy projectiles. Projectiles are added when enemies fire
         * and removed when they move off-screen or hit the player.
         */
        std::vector<EnemyProjectile> m_enemyProjectiles;
        
        /**
         * @brief Timer for enemy spawning
         * 
         * Accumulates delta time. When it exceeds ENEMY_SPAWN_INTERVAL,
         * a new enemy is spawned and the timer resets.
         */
        float m_enemySpawnTimer{0.0f};
        
        /**
         * @brief Timer for fire rate cooldown
         * 
         * Tracks time since last shot. When it reaches FIRE_COOLDOWN,
         * the player can fire again.
         */
        float m_fireCooldown{0.0f};
        
        /**
         * @brief Interval between enemy spawns in seconds
         */
        static constexpr float ENEMY_SPAWN_INTERVAL{2.0f};
        
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
         * @note Not yet implemented in gameplay
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
