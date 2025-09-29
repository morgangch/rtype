#include <iostream>
#include <graphics/Graphics.hpp>
#include <chrono>
#include <thread>
#include <cmath>

using namespace rtype::client::graphics;

int main() {
    std::cout << "=== R-Type Client - Graphics Test ===" << std::endl;
    
    // Initialize graphics system
    Graphics graphics;
    if (!graphics.Initialize(800, 600, "R-Type - Graphics Test")) {
        std::cout << "Failed to initialize graphics!" << std::endl;
        return -1;
    }
    
    std::cout << "Graphics initialized successfully!" << std::endl;
    std::cout << "Controls: ESC to close window" << std::endl;
    
    // Create some test sprites
    Sprite testSprite;
    testSprite.SetPosition(400, 300); // Center of screen
    testSprite.SetScale(2.0f);
    
    // Test variables for animation
    float time = 0.0f;
    const float speed = 100.0f;
    
    // Main game loop
    while (graphics.IsRunning()) {
        // Handle events
        graphics.PollEvents();
        
        // Update
        time += 0.016f; // ~60 FPS
        
        // Move test objects
        float circleX = 400 + std::sin(time) * 200;
        float circleY = 300 + std::cos(time) * 100;
        
        // Render
        graphics.BeginFrame();
        
        // Draw some test primitives
        graphics.DrawRectangle(50, 50, 100, 50, 0xFF0000FF);  // Red rectangle
        graphics.DrawCircle(circleX, circleY, 30, 0x00FF00FF); // Green moving circle
        graphics.DrawRectangle(650, 500, 100, 50, 0x0000FFFF); // Blue rectangle
        
        // Draw grid
        for (int x = 0; x < 800; x += 100) {
            graphics.DrawRectangle(x, 0, 2, 600, 0x444444FF);
        }
        for (int y = 0; y < 600; y += 100) {
            graphics.DrawRectangle(0, y, 800, 2, 0x444444FF);
        }
        
        graphics.EndFrame();
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "Graphics test finished!" << std::endl;
    return 0;
}
