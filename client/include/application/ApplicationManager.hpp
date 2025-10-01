#pragma once

#include "MenuApplication.hpp"
#include "GameApplication.hpp"
#include <memory>

namespace rtype::client {

/**
 * @brief Main Application Manager that handles transitions between Menu and Game
 */
class ApplicationManager {
public:
    enum class AppState {
        Menu,
        Game,
        Exiting
    };
    
    ApplicationManager();
    ~ApplicationManager();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
    // State transitions
    void SwitchToMenu();
    void SwitchToGame();
    void RequestExit();
    
private:
    AppState currentState;
    std::unique_ptr<MenuApplication> menuApp;
    std::unique_ptr<GameApplication> gameApp;
    bool isRunning;
};

} // namespace rtype::client