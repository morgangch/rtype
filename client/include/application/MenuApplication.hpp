#pragma once

#include <Graphics.hpp>
#include <Input.hpp>
#include "gui/StateManager.hpp"
#include "gui/states/MainMenuState.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <thread>

namespace rtype::client {

/**
 * @brief Menu Application class that integrates GUI with modern Graphics/Input system
 * 
 * This class bridges the SFML-based GUI system with the modern Graphics and Input libraries
 */
class MenuApplication {
public:
    MenuApplication();
    ~MenuApplication();
    
    // Core lifecycle
    bool Initialize(int width = 1280, int height = 720, const std::string& title = "R-TYPE - Main Menu");
    void Run();
    void Shutdown();
    
    // State management
    bool IsRunning() const { return isRunning; }
    bool ShouldStartGame() const { return shouldStartGame; }
    void ResetGameRequest() { shouldStartGame = false; }
    
private:
    void HandleEvents();
    void Update(float deltaTime);
    void Render();
    
    // Systems
    rtype::client::graphics::Graphics graphics;
    rtype::client::input::Input input;
    
    // GUI
    std::unique_ptr<rtype::client::gui::StateManager> stateManager;
    sf::RenderWindow* window; // Raw pointer to SFML window from graphics system
    
    bool isRunning;
    bool shouldStartGame = false;
};

} // namespace rtype::client