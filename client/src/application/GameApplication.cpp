#include "application/GameApplication.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace rtype::client {

GameApplication::GameApplication() : 
    graphics(),
    input(),
    isRunning(false) {
}

GameApplication::~GameApplication() {
    Shutdown();
}

bool GameApplication::Initialize(int width, int height, const std::string& title) {
    std::cout << "=== R-Type Game - Starting ===" << std::endl;
    
    screenWidth = static_cast<float>(width);
    screenHeight = static_cast<float>(height);
    
    // Initialize graphics system
    if (!graphics.Initialize(width, height, title)) {
        std::cout << "Failed to initialize graphics!" << std::endl;
        return false;
    }
    
    // Initialize input system
    if (!input.Initialize()) {
        std::cout << "Failed to initialize input!" << std::endl;
        return false;
    }
    
    // Connect input to graphics
    graphics.SetInputManager(&input.GetInputManager());
    
    // Initialize player position
    player.x = 50.0f;
    player.y = screenHeight * 0.5f;
    
    isRunning = true;
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD/Arrows - Move player ship" << std::endl;
    std::cout << "  SPACE - Fire" << std::endl;
    std::cout << "  ESC - Return to menu" << std::endl;
    
    return true;
}

void GameApplication::Run() {
    if (!isRunning) {
        std::cout << "Game not initialized!" << std::endl;
        return;
    }
    
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    
    while (isRunning && graphics.IsRunning()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time to prevent large jumps
        deltaTime = std::min(deltaTime, 0.016f);
        
        // Handle events
        HandleEvents();
        
        // Update
        Update(deltaTime);
        
        // Render
        Render();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

void GameApplication::HandleEvents() {
    // Handle graphics events (window close, etc.)
    graphics.PollEvents();
    
    // Update input
    input.Update();
    
    // Check for exit
    if (input.IsPausePressed()) {
        std::cout << "ESC pressed, returning to menu..." << std::endl;
        isRunning = false;
    }
}

void GameApplication::Update(float deltaTime) {
    UpdatePlayer(deltaTime);
    UpdateEnemies(deltaTime);
    HandleCollisions();
}

void GameApplication::UpdatePlayer(float deltaTime) {
    // Player movement
    float horizontalMovement = input.GetPlayerMovement();
    float verticalMovement = input.GetPlayerVerticalMovement();
    
    if (horizontalMovement != 0.0f || verticalMovement != 0.0f) {
        // Update position
        player.x += horizontalMovement * player.speed * deltaTime;
        player.y += verticalMovement * player.speed * deltaTime;
        
        // Keep player on screen
        player.x = std::max(player.width * 0.5f, std::min(player.x, screenWidth * 0.3f)); // Left third of screen
        player.y = std::max(player.height * 0.5f, std::min(player.y, screenHeight - player.height * 0.5f));
    }
    
    // Fire detection (for future projectiles)
    if (input.IsFirePressed()) {
        std::cout << "FIRE! Pew pew!" << std::endl;
    }
}

void GameApplication::UpdateEnemies(float deltaTime) {
    // Spawn enemies
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        enemySpawnTimer = 0.0f;
        
        // Add new enemy from the right
        Enemy newEnemy;
        newEnemy.x = screenWidth + newEnemy.width;
        newEnemy.y = static_cast<float>(rand() % static_cast<int>(screenHeight - 100)) + 50;
        newEnemy.active = true;
        
        enemies.push_back(newEnemy);
        
        // Limit number of enemies
        if (enemies.size() > 10) {
            enemies.erase(enemies.begin());
        }
    }
    
    // Update enemy positions
    for (auto& enemy : enemies) {
        if (enemy.active) {
            enemy.x -= enemy.speed * deltaTime;
            
            // Remove enemies that are off-screen
            if (enemy.x < -enemy.width) {
                enemy.active = false;
            }
        }
    }
    
    // Remove inactive enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.active; }),
        enemies.end()
    );
}

void GameApplication::HandleCollisions() {
    // Simple collision detection between player and enemies
    for (auto& enemy : enemies) {
        if (enemy.active) {
            // Simple AABB collision
            if (player.x < enemy.x + enemy.width &&
                player.x + player.width > enemy.x &&
                player.y < enemy.y + enemy.height &&
                player.y + player.height > enemy.y) {
                
                std::cout << "Collision detected! Game Over!" << std::endl;
                enemy.active = false;
                // Could add game over logic here
            }
        }
    }
}

void GameApplication::Render() {
    graphics.BeginFrame();
    
    // Draw starfield background
    for (int x = 0; x < static_cast<int>(screenWidth); x += 100) {
        for (int y = 0; y < static_cast<int>(screenHeight); y += 100) {
            graphics.DrawCircle(static_cast<float>(x + (rand() % 50)), 
                              static_cast<float>(y + (rand() % 50)), 1, 0xFFFFFF80);
        }
    }
    
    // Draw player ship (green)
    graphics.DrawRectangle(player.x - player.width * 0.5f, 
                          player.y - player.height * 0.5f, 
                          player.width, player.height, 0x00FF00FF);
    
    // Draw player "engine" effect
    graphics.DrawRectangle(player.x - player.width * 0.5f - 8, 
                          player.y - 4, 8, 8, 0xFF8800FF);
    
    // Draw enemies (red)
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            graphics.DrawRectangle(enemy.x - enemy.width * 0.5f, 
                                  enemy.y - enemy.height * 0.5f, 
                                  enemy.width, enemy.height, 0xFF0000FF);
        }
    }
    
    // Draw UI
    graphics.DrawRectangle(10, 10, 200, 30, 0x000000AA); // Background for text area
    
    graphics.EndFrame();
}

void GameApplication::Shutdown() {
    input.Shutdown();
    isRunning = false;
    std::cout << "Game shut down." << std::endl;
}

} // namespace rtype::client