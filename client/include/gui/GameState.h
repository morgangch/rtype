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
#include "ParallaxSystem.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace rtype::client::gui {
    
    /**
     * @class GameState
     * @brief The in-game state for Space Invaders gameplay
     * 
     * GameState handles the core game loop including:
     * - Player ship movement (ZQSD/Arrow keys)
     * - Enemy spawning and movement
     * - Collision detection
     * - Game rendering
     * 
     * Features:
     * - Smooth player movement in all directions
     * - Automatic enemy spawning from the right
     * - Simple AABB collision detection
     * - Space-themed visuals with starfield background
     * - ESC to return to main menu
     * 
     * Controls:
     * - Z/Up: Move up
     * - S/Down: Move down
     * - Q/Left: Move left
     * - D/Right: Move right
     * - Space: Fire (not yet implemented)
     * - ESC: Return to menu
     */
    class GameState : public State {
    public:
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
         * position, size, speed, and collision bounds calculation.
         */
        struct Player {
            /** @brief Current position of the player (center point) */
            sf::Vector2f position{100.0f, 360.0f};  // Centered vertically
            
            /** @brief Size of the player ship (width, height) */
            sf::Vector2f size{32.0f, 32.0f};
            
            /** @brief Movement speed in pixels per second */
            float speed{300.0f};
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the player's collision box
             * 
             * Calculates the bounding rectangle centered on the player's position.
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
         * @struct Enemy
         * @brief Represents an enemy ship in the game
         * 
         * The Enemy structure contains all data for enemy ships that move from
         * right to left across the screen.
         */
        struct Enemy {
            /** @brief Current position of the enemy (center point) */
            sf::Vector2f position;
            
            /** @brief Size of the enemy ship (width, height) */
            sf::Vector2f size{24.0f, 24.0f};
            
            /** @brief Movement speed in pixels per second (leftward) */
            float speed{100.0f};
            
            /**
             * @brief Get the axis-aligned bounding box for collision detection
             * @return sf::FloatRect representing the enemy's collision box
             * 
             * Calculates the bounding rectangle centered on the enemy's position.
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
         */
        void updatePlayer(float deltaTime);
        
        /**
         * @brief Update all enemies' positions and remove off-screen ones
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Moves all enemies from right to left. Removes enemies that have
         * moved completely off the left side of the screen.
         */
        void updateEnemies(float deltaTime);
        
        /**
         * @brief Check for collisions between player and enemies
         * 
         * Uses AABB (Axis-Aligned Bounding Box) collision detection.
         * If a collision is detected, the game is reset.
         */
        void checkCollisions();
        
        /**
         * @brief Spawn a new enemy at a random vertical position
         * 
         * Creates a new enemy on the right edge of the screen at a random
         * Y position. Only spawns if the current enemy count is below MAX_ENEMIES.
         */
        void spawnEnemy();
        
        /**
         * @brief Reset the game to initial state
         * 
         * Resets player position to starting point and clears all enemies.
         * Called when the player collides with an enemy or when starting a new game.
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
         * Draws the player's ship as a green triangle pointing right.
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
         * @brief Interval between enemy spawns in seconds
         */
        static constexpr float ENEMY_SPAWN_INTERVAL{2.0f};
        
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
