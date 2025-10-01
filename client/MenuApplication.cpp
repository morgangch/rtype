#include <SFML/Graphics.hpp>
#include "components/gui/StateManager.hpp"
#include "components/gui/MainMenuState.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Initialize random seed
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
