#pragma once

#include <Graphics.hpp>
#include <Input.hpp>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

namespace rtype::client {

/**
 * @brief Game Application class for the actual R-Type gameplay
 * 
 * This class handles the game loop, player movement, enemies, etc.
 */
class GameApplication {
public:
    GameApplication();
    ~GameApplication();
    
    // Core lifecycle
    bool Initialize(int width = 1280, int height = 720, const std::string& title = "R-TYPE - Game");
    void Run();
    void Shutdown();
    
    // State management
    bool IsRunning() const { return isRunning; }
    void RequestExit() { isRunning = false; }
    
private:
    void HandleEvents();
    void Update(float deltaTime);
    void Render();
    
    // Game logic
    void UpdatePlayer(float deltaTime);
    void UpdateEnemies(float deltaTime);
    void HandleCollisions();
    
    // Systems
    rtype::client::graphics::Graphics graphics;
    rtype::client::input::Input input;
    
    // Game state
    bool isRunning;
    
    // Game objects
    struct Player {
        float x = 100.0f;
        float y = 300.0f;
        float width = 32.0f;
        float height = 32.0f;
        float speed = 300.0f;
    } player;
    
    struct Enemy {
        float x, y;
        float width = 24.0f;
        float height = 24.0f;
        float speed = 100.0f;
        bool active = true;
    };
    
    std::vector<Enemy> enemies;
    float enemySpawnTimer = 0.0f;
    const float enemySpawnInterval = 2.0f;
    
    // Screen bounds
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
};

} // namespace rtype::client