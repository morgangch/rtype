#ifndef RTYPE_CLIENT_TEST_MODE_HPP
#define RTYPE_CLIENT_TEST_MODE_HPP

#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "../lib/input/include/Input.hpp"
#include "../lib/graphics/include/Graphics.hpp"

namespace rtype::client {

/**
 * @brief Mode de test pour diagnostiquer les problèmes de performance et d'input
 */
class TestMode {
public:
    TestMode();
    ~TestMode();
    
    bool Initialize(int width = 800, int height = 600);
    void Run();
    void Shutdown();
    
private:
    void HandleEvents();
    void Update(float deltaTime);
    void Render();
    
    void TestInput();
    void TestPerformance(float deltaTime);
    void TestGraphics();
    
    // Systems
    rtype::client::graphics::Graphics graphics;
    rtype::client::input::Input input;
    sf::RenderWindow* window;
    
    // Test state
    bool isRunning;
    
    // Performance metrics
    float averageFPS;
    float minFPS;
    float maxFPS;
    int frameCount;
    std::chrono::steady_clock::time_point lastFPSUpdate;
    
    // Input test state
    struct TestPlayer {
        float x = 400;
        float y = 300;
        float speed = 200.0f;
        bool firing = false;
    } testPlayer;
    
    // Graphics test
    float rotation = 0.0f;
};

} // namespace rtype::client

#endif // RTYPE_CLIENT_TEST_MODE_HPP