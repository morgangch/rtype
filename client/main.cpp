/**
 * @file main.cpp
 * @brief Entry point for the R-TYPE client application
 * 
 * This file contains the main function that initializes the R-TYPE client
 * application. It sets up the SFML window, initializes the state management
 * system with integrated network functionality, and runs the main game loop.
 * 
 * The application starts with the MainMenuState and handles state transitions
 * through the StateManager. Network operations are managed through the
 * NetworkManager which runs in a background thread for non-blocking I/O.
 * 
 * The main loop follows the standard game loop pattern:
 * 1. Handle events (input, window events)
 * 2. Update game state, animations, and network operations
 * 3. Render the current state to the screen
 * 4. Display the frame
 * 
 * Network functionality has been restored and integrated into the GUI system:
 * - Background network thread handles UDP communication
 * - States can connect/disconnect using NetworkManager
 * - Original network code functionality is preserved
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include <SFML/Graphics.hpp>
#include "gui/StateManager.h"
#include "gui/MainMenuState.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "include/network/network.h"

int main() {
    // Initialize random seed for username generation
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create window with fixed size (non-resizable)
    sf::RenderWindow window(sf::VideoMode(1280, 720), "R-TYPE - Main Menu",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Create state manager
    rtype::client::gui::StateManager stateManager(window);
    // Expose the StateManager to other subsystems (network controllers)
    rtype::client::gui::setGlobalStateManager(&stateManager);

    // Push initial state (main menu)
    stateManager.pushState(std::make_unique<rtype::client::gui::MainMenuState>(stateManager));

    sf::Clock clock;

    // Initialize network
    rtype::client::network::register_controllers();

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

        if (rtype::client::network::udp_fd != -1) {
            rtype::client::network::loop_recv();
            rtype::client::network::loop_send();
        }
    }

    return 0;
}
