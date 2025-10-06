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
         * @brief Player ship structure
         */
        struct Player {
            sf::Vector2f position{100.0f, 360.0f};  // Centered vertically
            sf::Vector2f size{32.0f, 32.0f};
            float speed{300.0f};
            
            // Get bounding box for collision
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
         * @brief Enemy structure
         */
        struct Enemy {
            sf::Vector2f position;
            sf::Vector2f size{24.0f, 24.0f};
            float speed{100.0f};
            
            // Get bounding box for collision
            sf::FloatRect getBounds() const {
                return sf::FloatRect(
                    position.x - size.x * 0.5f,
                    position.y - size.y * 0.5f,
                    size.x,
                    size.y
                );
            }
        };
        
        // Game logic methods
        void updatePlayer(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();
        void spawnEnemy();
        void resetGame();
        
        // Rendering methods
        void renderStarfield(sf::RenderWindow& window);
        void renderPlayer(sf::RenderWindow& window);
        void renderEnemies(sf::RenderWindow& window);
        
        // Input handling
        sf::Vector2f getMovementInput() const;
        
        // State reference
        StateManager& m_stateManager;
        
        // Game objects
        Player m_player;
        std::vector<Enemy> m_enemies;
        
        // Spawn timing
        float m_enemySpawnTimer{0.0f};
        static constexpr float ENEMY_SPAWN_INTERVAL{2.0f};
        static constexpr size_t MAX_ENEMIES{10};
        
        // Input state tracking
        bool m_keyUp{false};
        bool m_keyDown{false};
        bool m_keyLeft{false};
        bool m_keyRight{false};
        bool m_keyFire{false};
        
        // Screen bounds
        static constexpr float SCREEN_WIDTH{1280.0f};
        static constexpr float SCREEN_HEIGHT{720.0f};
        static constexpr float PLAYER_BOUNDS_RIGHT{SCREEN_WIDTH * 0.3f}; // Left third only
    };
    
} // namespace rtype::client::gui

#endif // CLIENT_GAME_STATE_HPP
