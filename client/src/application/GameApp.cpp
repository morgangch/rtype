#include "application/GameApp.hpp"
#include "../../lib/input/include/SFMLKeyConverter.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace rtype::client {

GameApp::GameApp() : 
    graphics(),
    input(),
    currentMode(Mode::InGame),
    isRunning(false),
    window(nullptr) {
}

GameApp::~GameApp() {
    Shutdown();
}

bool GameApp::Initialize(int width, int height, const std::string& title) {
    std::cout << "=== R-Type Game Application ===" << std::endl;
    
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
    
    // Get SFML window
    window = graphics.GetSFMLWindow();
    if (!window) {
        std::cout << "Failed to get SFML window from graphics system!" << std::endl;
        return false;
    }
    
    window->setFramerateLimit(60);
    
    // Initialize game objects
    player.x = 100.0f;
    player.y = screenHeight * 0.5f;
    
    isRunning = true;
    
    std::cout << "Game application initialized successfully!" << std::endl;
    
    return true;
}

void GameApp::Run() {
    if (!isRunning) {
        std::cout << "Application not initialized!" << std::endl;
        return;
    }
    
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    
    while (isRunning && graphics.IsRunning()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time to prevent large jumps
        deltaTime = std::min(deltaTime, 0.016f);
        
        // Handle events (same for both modes)
        HandleEvents();
        
        // Update based on current mode
        Update(deltaTime);
        
        // Render based on current mode
        Render();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

void GameApp::HandleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        // Handle window close
        if (event.type == sf::Event::Closed) {
            isRunning = false;
            return;
        }
        
        // Handle ESC key to exit
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            std::cout << "ESC pressed, exiting game..." << std::endl;
            isRunning = false;
            return;
        }
        
        // Pass key events to input system
        if (event.type == sf::Event::KeyPressed) {
            // Convert SFML key to our Key enum
            auto key = rtype::client::input::SFMLKeyConverter::SFMLToKey(event.key.code);
            if (key != rtype::client::input::Key::Count) {
                input.GetInputManager().HandleKeyPressed(key);
            }
        } else if (event.type == sf::Event::KeyReleased) {
            // Convert SFML key to our Key enum
            auto key = rtype::client::input::SFMLKeyConverter::SFMLToKey(event.key.code);
            if (key != rtype::client::input::Key::Count) {
                input.GetInputManager().HandleKeyReleased(key);
            }
        }
    }
    
    // Update input system continuously
    input.Update();
}

void GameApp::Update(float deltaTime) {
    UpdateInGame(deltaTime);
}

void GameApp::Render() {
    graphics.BeginFrame();
    RenderInGame();
    graphics.EndFrame();
}

void GameApp::UpdateInGame(float deltaTime) {
    UpdatePlayer(deltaTime);
    UpdateEnemies(deltaTime);
    HandleCollisions();
}

void GameApp::RenderInGame() {
    // Clear with space background for game
    window->clear(sf::Color(5, 5, 15));
    
    // Draw starfield background
    for (int x = 0; x < static_cast<int>(screenWidth); x += 150) {
        for (int y = 0; y < static_cast<int>(screenHeight); y += 150) {
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
    
    // Draw UI - Game mode indicator
    graphics.DrawRectangle(10, 10, 250, 30, 0x000000AA);
    
    // Display
    window->display();
}

void GameApp::UpdatePlayer(float deltaTime) {
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
    
    // Fire detection
    if (input.IsFirePressed()) {
        std::cout << "FIRE! Pew pew!" << std::endl;
    }
}

void GameApp::UpdateEnemies(float deltaTime) {
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

void GameApp::HandleCollisions() {
    // Simple collision detection between player and enemies
    for (auto& enemy : enemies) {
        if (enemy.active) {
            // Simple AABB collision
            if (player.x < enemy.x + enemy.width &&
                player.x + player.width > enemy.x &&
                player.y < enemy.y + enemy.height &&
                player.y + player.height > enemy.y) {
                
                std::cout << "Collision detected! Restarting..." << std::endl;
                enemy.active = false;
                
                // Reset player position
                player.x = 100.0f;
                player.y = screenHeight * 0.5f;
                
                // Clear enemies
                enemies.clear();
            }
        }
    }
}

void GameApp::SwitchToInGame() {
    std::cout << "Switching to InGame mode" << std::endl;
    currentMode = Mode::InGame;
    
    // Reset game state
    enemies.clear();
    enemySpawnTimer = 0.0f;
    player.x = 100.0f;
    player.y = screenHeight * 0.5f;
    
    std::cout << "Game mode active! Controls:" << std::endl;
    std::cout << "  ZQSD/Arrows - Move player ship" << std::endl;
    std::cout << "  SPACE - Fire" << std::endl;
    std::cout << "  ESC - Exit game" << std::endl;
}

void GameApp::Shutdown() {
    input.Shutdown();
    isRunning = false;
    std::cout << "Game application shut down." << std::endl;
}

} // namespace rtype::client