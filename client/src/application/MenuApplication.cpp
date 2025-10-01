#include "application/MenuApplication.hpp"
#include <iostream>

namespace rtype::client {

MenuApplication::MenuApplication() : 
    graphics(),
    input(),
    stateManager(nullptr),
    window(nullptr),
    isRunning(false) {
}

MenuApplication::~MenuApplication() {
    Shutdown();
}

bool MenuApplication::Initialize(int width, int height, const std::string& title) {
    std::cout << "=== R-Type Client - Menu Application ===" << std::endl;
    
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
    
    // Get SFML window from graphics system for GUI
    window = graphics.GetSFMLWindow();
    if (!window) {
        std::cout << "Failed to get SFML window from graphics system!" << std::endl;
        return false;
    }
    
    // Set framerate limit
    window->setFramerateLimit(60);
    
    // Initialize state manager with SFML window
    stateManager = std::make_unique<rtype::client::gui::StateManager>(*window);
    
    // Create and push initial main menu state  
    auto mainMenuState = std::make_unique<rtype::client::gui::MainMenuState>(*stateManager);
    stateManager->pushState(std::move(mainMenuState));
    
    isRunning = true;
    std::cout << "Menu application initialized successfully!" << std::endl;
    
    return true;
}

void MenuApplication::Run() {
    if (!isRunning) {
        std::cout << "Application not initialized!" << std::endl;
        return;
    }
    
    sf::Clock clock;
    
    while (isRunning && graphics.IsRunning() && !stateManager->isEmpty()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Handle events
        HandleEvents();
        
        // Update
        Update(deltaTime);
        
        // Render
        Render();
    }
}

void MenuApplication::HandleEvents() {
    sf::Event event;
    while (window && window->pollEvent(event)) {
        // Handle window close
        if (event.type == sf::Event::Closed) {
            isRunning = false;
            return;
        }
        
        // Pass event to state manager
        if (stateManager && !stateManager->isEmpty()) {
            stateManager->handleEvent(event);
        }
    }
    
    // Update input system
    input.Update();
}

void MenuApplication::Update(float deltaTime) {
    if (stateManager && !stateManager->isEmpty()) {
        stateManager->update(deltaTime);
    }
}

void MenuApplication::Render() {
    // Clear with dark background
    window->clear(sf::Color(20, 20, 30));
    
    // Render current state
    if (stateManager && !stateManager->isEmpty()) {
        stateManager->render();
    }
    
    // Display
    window->display();
}

void MenuApplication::Shutdown() {
    if (stateManager) {
        stateManager.reset();
    }
    
    input.Shutdown();
    
    isRunning = false;
    std::cout << "Menu application shut down." << std::endl;
}

} // namespace rtype::client
