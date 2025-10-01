#include <iostream>
#include <Graphics.hpp>
#include <Input.hpp>
#include <chrono>
#include <thread>
#include <cmath>

using namespace rtype::client::graphics;
using namespace rtype::client::input;

int main() {
    std::cout << "=== R-Type Client - Player Movement Test ===" << std::endl;
    
    // Initialize graphics system
    Graphics graphics;
    if (!graphics.Initialize(800, 600, "R-Type - Player Movement Test")) {
        std::cout << "Failed to initialize graphics!" << std::endl;
        return -1;
    }
    
    // Initialize input system
    Input input;
    if (!input.Initialize()) {
        std::cout << "Failed to initialize input!" << std::endl;
        return -1;
    }
    
    // Connect input to graphics
    graphics.SetInputManager(&input.GetInputManager());
    
    std::cout << "Graphics and Input initialized successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  LEFT/RIGHT arrows - Move player ship horizontally" << std::endl;
    std::cout << "  UP/DOWN arrows - Move player ship vertically" << std::endl;
    std::cout << "  WASD keys also work for movement" << std::endl;
    std::cout << "  SPACE - Fire (test)" << std::endl;
    std::cout << "  ESC - Close window" << std::endl;
    
    // Player ship variables
    float playerX = 400.0f;  // Center of screen
    float playerY = 500.0f;  // Near bottom
    const float playerSpeed = 300.0f; // pixels per second
    const float screenWidth = 800.0f;
    const float screenHeight = 600.0f;
    const float playerWidth = 32.0f;  // Assumed player width
    const float playerHeight = 32.0f; // Assumed player height
    
    // Test variables for animation
    float time = 0.0f;
    
    // Main game loop
    while (graphics.IsRunning()) {
        // Handle events and update input
        graphics.PollEvents();
        input.Update();
        
        // Check for pause
        if (input.IsPausePressed()) {
            std::cout << "Pause pressed, exiting..." << std::endl;
            break;
        }
        
        // Update
        float deltaTime = 0.016f; // ~60 FPS
        time += deltaTime;
        
        // Player movement
        float horizontalMovement = input.GetPlayerMovement();
        float verticalMovement = input.GetPlayerVerticalMovement();
        
        if (horizontalMovement != 0.0f || verticalMovement != 0.0f) {
            // Update position
            playerX += horizontalMovement * playerSpeed * deltaTime;
            playerY += verticalMovement * playerSpeed * deltaTime;
            
            // Keep player on screen - horizontal bounds
            if (playerX < playerWidth * 0.5f) {
                playerX = playerWidth * 0.5f;
            } else if (playerX > screenWidth - playerWidth * 0.5f) {
                playerX = screenWidth - playerWidth * 0.5f;
            }
            
            // Keep player on screen - vertical bounds
            if (playerY < playerHeight * 0.5f) {
                playerY = playerHeight * 0.5f;
            } else if (playerY > screenHeight - playerHeight * 0.5f) {
                playerY = screenHeight - playerHeight * 0.5f;
            }
            
            std::cout << "Player position: (" << playerX << ", " << playerY << ")" << std::endl;
        }
        
        // Fire test
        if (input.IsFirePressed()) {
            std::cout << "FIRE! Pew pew pew!" << std::endl;
        }
        
        // Render
        graphics.BeginFrame();
        
        // Draw background grid
        for (int x = 0; x < 800; x += 100) {
            graphics.DrawRectangle(x, 0, 2, 600, 0x444444FF);
        }
        for (int y = 0; y < 600; y += 100) {
            graphics.DrawRectangle(0, y, 800, 2, 0x444444FF);
        }
        
        // Draw player ship (simple rectangle for now)
        graphics.DrawRectangle(playerX - playerWidth * 0.5f, playerY - playerHeight * 0.5f, playerWidth, playerHeight, 0x00FF00FF); // Green player
        
        // Draw some enemy placeholders
        float enemyY = 100 + std::sin(time) * 50;
        graphics.DrawRectangle(200, enemyY, 24, 24, 0xFF0000FF); // Red enemy
        graphics.DrawRectangle(400, enemyY + 30, 24, 24, 0xFF0000FF); // Red enemy
        graphics.DrawRectangle(600, enemyY, 24, 24, 0xFF0000FF); // Red enemy
        
        graphics.EndFrame();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "Player movement test finished!" << std::endl;
    return 0;
}