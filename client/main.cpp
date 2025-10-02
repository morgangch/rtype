/**
 * @file main.cpp
 * @brief Entry point for the R-TYPE client application
 * 
 * This file contains the main function that initializes the R-TYPE client
 * application. It sets up the SFML window, initializes the state management
 * system, and runs the main game loop. The application starts with the
 * MainMenuState and handles state transitions through the StateManager.
 * 
 * The main loop follows the standard game loop pattern:
 * 1. Handle events (input, window events)
 * 2. Update game state and animations
 * 3. Render the current state to the screen
 * 4. Display the frame
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include <SFML/Graphics.hpp>
#include "gui/StateManager.hpp"
#include "gui/MainMenuState.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Initialize random seed for username generation
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Create window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "R-TYPE - Main Menu", sf::Style::Default);
    window.setFramerateLimit(60);
    
    // Create state manager
    rtype::client::gui::StateManager stateManager(window);
    
    // Push initial state (main menu)
    stateManager.pushState(std::make_unique<rtype::client::gui::MainMenuState>(stateManager));
    
    sf::Clock clock;
    
    // Main loop
    while (window.isOpen() && !stateManager.isEmpty()) {
        float deltaTime = clock.restart().asSeconds();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            stateManager.handleEvent(event);
        }
        
        stateManager.update(deltaTime);
        
        // Clear and render
        window.clear(sf::Color::Black);
        stateManager.render();
        window.display();
    }
    
    return 0;
}
