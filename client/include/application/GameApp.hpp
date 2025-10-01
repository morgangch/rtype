#ifndef RTYPE_CLIENT_GAME_APP_HPP
#define RTYPE_CLIENT_GAME_APP_HPP

#include <Graphics.hpp>
#include <Input.hpp>
#include "gui/StateManager.hpp"
#include "gui/states/MainMenuState.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

namespace rtype::client {

/**
 * @brief Main Game Application that handles Menu and InGame modes
 */
class GameApp {
public:
    enum class Mode {
        Menu,
        InGame
    };
    
    GameApp();
    ~GameApp();
    
    // Core lifecycle
    bool Initialize(int width = 1280, int height = 720, const std::string& title = "R-TYPE");
    void Run();
    void Shutdown();
    
    // Mode switching
    void SwitchToMenu();
    void SwitchToInGame();
    
private:
    void HandleEvents();
    void Update(float deltaTime);
    void Render();
    
    // Menu mode methods
    void UpdateMenu(float deltaTime);
    void RenderMenu();
    
    // InGame mode methods
    void UpdateInGame(float deltaTime);
    void RenderInGame();
    void UpdatePlayer(float deltaTime);
    void UpdateEnemies(float deltaTime);
    void HandleCollisions();
    
    // Shared systems
    rtype::client::graphics::Graphics graphics;
    rtype::client::input::Input input;
    
    // Current mode
    Mode currentMode;
    bool isRunning;
    
    // Menu system
    std::unique_ptr<rtype::client::gui::StateManager> stateManager;
    sf::RenderWindow* window;
    
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

#endif // RTYPE_CLIENT_GAME_APP_HPP