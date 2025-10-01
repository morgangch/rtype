#include "TestMode.hpp"
#include "../lib/input/include/SFMLKeyConverter.hpp"
#include <algorithm>
#include <iomanip>

namespace rtype::client {

TestMode::TestMode() : 
    window(nullptr),
    isRunning(false),
    averageFPS(0.0f),
    minFPS(999.0f),
    maxFPS(0.0f),
    frameCount(0),
    lastFPSUpdate(std::chrono::steady_clock::now()) {
}

TestMode::~TestMode() {
    Shutdown();
}

bool TestMode::Initialize(int width, int height) {
    std::cout << "=== R-Type Test Mode ===" << std::endl;
    std::cout << "Diagnostic des performances et des contrôles" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Initialize graphics
    if (!graphics.Initialize(width, height, "R-Type Test Mode")) {
        std::cout << "❌ Failed to initialize graphics!" << std::endl;
        return false;
    }
    std::cout << "✅ Graphics initialized" << std::endl;
    
    // Initialize input
    if (!input.Initialize()) {
        std::cout << "❌ Failed to initialize input!" << std::endl;
        return false;
    }
    std::cout << "✅ Input initialized" << std::endl;
    
    // Get window
    window = graphics.GetSFMLWindow();
    if (!window) {
        std::cout << "❌ Failed to get SFML window!" << std::endl;
        return false;
    }
    
    window->setFramerateLimit(60);
    std::cout << "✅ Window configured (60 FPS limit)" << std::endl;
    
    isRunning = true;
    
    std::cout << std::endl;
    std::cout << "🎮 Test Controls:" << std::endl;
    std::cout << "  ZQSD/Arrows - Move test square" << std::endl;
    std::cout << "  SPACE - Fire test (visual feedback)" << std::endl;
    std::cout << "  ESC - Exit test" << std::endl;
    std::cout << "  F1 - Toggle performance info" << std::endl;
    std::cout << std::endl;
    
    return true;
}

void TestMode::Run() {
    if (!isRunning) {
        std::cout << "❌ Test mode not initialized!" << std::endl;
        return;
    }
    
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    bool showPerformanceInfo = true;
    
    while (isRunning && graphics.IsRunning()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time
        deltaTime = std::min(deltaTime, 0.033f); // Max 30ms
        
        HandleEvents();
        Update(deltaTime);
        Render();
        
        // Performance testing
        TestPerformance(deltaTime);
        
        // Print FPS info every second
        auto now = std::chrono::steady_clock::now();
        if (showPerformanceInfo && std::chrono::duration<float>(now - lastFPSUpdate).count() >= 1.0f) {
            std::cout << "📊 FPS: " << std::fixed << std::setprecision(1) 
                      << "Current=" << (1.0f / deltaTime)
                      << " | Avg=" << averageFPS
                      << " | Min=" << minFPS 
                      << " | Max=" << maxFPS << std::endl;
            lastFPSUpdate = now;
        }
    }
    
    std::cout << std::endl;
    std::cout << "📈 Final Performance Report:" << std::endl;
    std::cout << "  Average FPS: " << std::fixed << std::setprecision(2) << averageFPS << std::endl;
    std::cout << "  Minimum FPS: " << minFPS << std::endl;
    std::cout << "  Maximum FPS: " << maxFPS << std::endl;
    std::cout << "  Total frames: " << frameCount << std::endl;
}

void TestMode::HandleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            isRunning = false;
            return;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                std::cout << "🚪 ESC pressed - Exiting test mode" << std::endl;
                isRunning = false;
                return;
            }
            
            // Convert and pass to input system
            auto key = rtype::client::input::SFMLKeyConverter::SFMLToKey(event.key.code);
            if (key != rtype::client::input::Key::Count) {
                input.GetInputManager().HandleKeyPressed(key);
                std::cout << "⌨️  Key pressed: " << static_cast<int>(key) << std::endl;
            }
        }
        
        if (event.type == sf::Event::KeyReleased) {
            auto key = rtype::client::input::SFMLKeyConverter::SFMLToKey(event.key.code);
            if (key != rtype::client::input::Key::Count) {
                input.GetInputManager().HandleKeyReleased(key);
                std::cout << "⌨️  Key released: " << static_cast<int>(key) << std::endl;
            }
        }
    }
    
    // Update input system
    input.Update();
}

void TestMode::Update(float deltaTime) {
    TestInput();
    TestGraphics();
    
    // Update test player
    float horizontal = input.GetPlayerMovement();
    float vertical = input.GetPlayerVerticalMovement();
    
    if (horizontal != 0.0f || vertical != 0.0f) {
        testPlayer.x += horizontal * testPlayer.speed * deltaTime;
        testPlayer.y += vertical * testPlayer.speed * deltaTime;
        
        // Keep on screen
        testPlayer.x = std::max(25.0f, std::min(testPlayer.x, 800.0f - 25.0f));
        testPlayer.y = std::max(25.0f, std::min(testPlayer.y, 600.0f - 25.0f));
        
        std::cout << "🎮 Movement: H=" << horizontal << " V=" << vertical 
                  << " Pos=(" << static_cast<int>(testPlayer.x) << "," << static_cast<int>(testPlayer.y) << ")" << std::endl;
    }
    
    testPlayer.firing = input.IsFirePressed();
    if (testPlayer.firing) {
        std::cout << "🔫 FIRE! Test shot!" << std::endl;
    }
}

void TestMode::Render() {
    graphics.BeginFrame();
    
    // Clear with dark blue
    window->clear(sf::Color(10, 10, 50));
    
    // Draw test grid
    for (int x = 0; x < 800; x += 100) {
        graphics.DrawRectangle(static_cast<float>(x), 0, 1, 600, 0x333333FF);
    }
    for (int y = 0; y < 600; y += 100) {
        graphics.DrawRectangle(0, static_cast<float>(y), 800, 1, 0x333333FF);
    }
    
    // Draw test player (changes color when firing)
    uint32_t playerColor = testPlayer.firing ? 0xFF0000FF : 0x00FF00FF; // Red if firing, green otherwise
    graphics.DrawRectangle(testPlayer.x - 25, testPlayer.y - 25, 50, 50, playerColor);
    
    // Draw rotating element for graphics test
    float centerX = 700;
    float centerY = 100;
    graphics.DrawRectangle(centerX - 10, centerY - 10, 20, 20, 0xFFFF00FF);
    
    // Draw performance indicator
    uint32_t fpsColor = 0x00FF00FF; // Green
    if (averageFPS < 45) fpsColor = 0xFF8800FF; // Orange
    if (averageFPS < 30) fpsColor = 0xFF0000FF; // Red
    
    graphics.DrawRectangle(10, 10, 200, 30, 0x000000AA);
    
    // Draw controls reminder
    graphics.DrawRectangle(10, 550, 300, 40, 0x000000AA);
    
    graphics.EndFrame();
}

void TestMode::TestInput() {
    // Test all key states
    static int testCounter = 0;
    testCounter++;
    
    if (testCounter % 60 == 0) { // Every second at 60fps
        bool anyKeyDown = false;
        
        // Test ZQSD keys specifically
        if (input.IsKeyDown(rtype::client::input::Key::Z)) {
            std::cout << "🔍 Z key is DOWN" << std::endl;
            anyKeyDown = true;
        }
        if (input.IsKeyDown(rtype::client::input::Key::Q)) {
            std::cout << "🔍 Q key is DOWN" << std::endl;
            anyKeyDown = true;
        }
        if (input.IsKeyDown(rtype::client::input::Key::S)) {
            std::cout << "🔍 S key is DOWN" << std::endl;
            anyKeyDown = true;
        }
        if (input.IsKeyDown(rtype::client::input::Key::D)) {
            std::cout << "🔍 D key is DOWN" << std::endl;
            anyKeyDown = true;
        }
        
        if (!anyKeyDown) {
            std::cout << "🔍 No keys detected (input system check)" << std::endl;
        }
    }
}

void TestMode::TestPerformance(float deltaTime) {
    frameCount++;
    
    float currentFPS = 1.0f / deltaTime;
    
    // Update FPS statistics
    minFPS = std::min(minFPS, currentFPS);
    maxFPS = std::max(maxFPS, currentFPS);
    
    // Running average
    averageFPS = (averageFPS * (frameCount - 1) + currentFPS) / frameCount;
    
    // Warning for performance issues
    if (currentFPS < 30) {
        std::cout << "⚠️  Performance warning: FPS dropped to " << std::fixed << std::setprecision(1) << currentFPS << std::endl;
    }
}

void TestMode::TestGraphics() {
    rotation += 1.0f;
    if (rotation > 360.0f) rotation = 0.0f;
}

void TestMode::Shutdown() {
    input.Shutdown();
    isRunning = false;
    std::cout << "✅ Test mode shutdown complete" << std::endl;
}

} // namespace rtype::client