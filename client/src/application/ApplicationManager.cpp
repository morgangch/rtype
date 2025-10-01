#include "application/ApplicationManager.hpp"
#include <iostream>

namespace rtype::client {

ApplicationManager::ApplicationManager() : 
    currentState(AppState::Menu),
    menuApp(nullptr),
    gameApp(nullptr),
    isRunning(false) {
}

ApplicationManager::~ApplicationManager() {
    Shutdown();
}

bool ApplicationManager::Initialize() {
    std::cout << "=== R-Type Application Manager ===" << std::endl;
    
    // Initialize with menu first
    menuApp = std::make_unique<MenuApplication>();
    if (!menuApp->Initialize(1280, 720, "R-TYPE - Main Menu")) {
        std::cout << "Failed to initialize menu application!" << std::endl;
        return false;
    }
    
    isRunning = true;
    currentState = AppState::Menu;
    
    std::cout << "Application Manager initialized successfully!" << std::endl;
    return true;
}

void ApplicationManager::Run() {
    while (isRunning) {
        switch (currentState) {
            case AppState::Menu: {
                if (menuApp && menuApp->IsRunning()) {
                    menuApp->Run();
                    
                    // Check if menu wants to start game
                    if (menuApp->ShouldStartGame()) {
                        std::cout << "Menu requested game start!" << std::endl;
                        menuApp->ResetGameRequest();
                        
                        // IMPORTANT: Properly shutdown menu before starting game
                        menuApp->Shutdown();
                        menuApp.reset();
                        
                        // Small delay to ensure window cleanup
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        currentState = AppState::Game;
                    }
                } else {
                    // Menu exited, check what to do next
                    if (menuApp && menuApp->ShouldStartGame()) {
                        menuApp->ResetGameRequest();
                        currentState = AppState::Game;
                    } else {
                        currentState = AppState::Exiting;
                    }
                }
                break;
            }
            
            case AppState::Game: {
                if (!gameApp) {
                    gameApp = std::make_unique<GameApplication>();
                    // Use same resolution as menu to avoid window conflicts
                    if (!gameApp->Initialize(1280, 720, "R-TYPE - Game")) {
                        std::cout << "Failed to initialize game!" << std::endl;
                        currentState = AppState::Menu;
                        break;
                    }
                }
                
                if (gameApp->IsRunning()) {
                    gameApp->Run();
                } else {
                    // Game exited, return to menu
                    std::cout << "Game exited, returning to menu..." << std::endl;
                    gameApp->Shutdown();
                    gameApp.reset();
                    
                    // Small delay to ensure window cleanup
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                    currentState = AppState::Menu;
                    
                    // Reinitialize menu
                    menuApp = std::make_unique<MenuApplication>();
                    if (!menuApp->Initialize(1280, 720, "R-TYPE - Main Menu")) {
                        std::cout << "Failed to reinitialize menu!" << std::endl;
                        currentState = AppState::Exiting;
                    }
                }
                break;
            }
            
            case AppState::Exiting:
            default:
                isRunning = false;
                break;
        }
    }
}

void ApplicationManager::SwitchToMenu() {
    if (gameApp) {
        gameApp->Shutdown();
        gameApp.reset();
    }
    currentState = AppState::Menu;
}

void ApplicationManager::SwitchToGame() {
    if (menuApp) {
        menuApp->Shutdown();
        menuApp.reset();
    }
    currentState = AppState::Game;
}

void ApplicationManager::RequestExit() {
    currentState = AppState::Exiting;
    isRunning = false;
}

void ApplicationManager::Shutdown() {
    if (menuApp) {
        menuApp->Shutdown();
        menuApp.reset();
    }
    
    if (gameApp) {
        gameApp->Shutdown();
        gameApp.reset();
    }
    
    isRunning = false;
    std::cout << "Application Manager shut down." << std::endl;
}

} // namespace rtype::client