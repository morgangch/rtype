#include <iostream>
#include <cstdlib>
#include <ctime>
#include "application/GameApp.hpp"

using namespace rtype::client;

int main() {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "=== R-Type Game ===" << std::endl;
    std::cout << "Starting game directly (no menu)..." << std::endl;

    // Create game application
    GameApp app;

    // Initialize and go directly to game
    if (!app.Initialize()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return -1;
    }

    // Start directly in game mode
    app.SwitchToInGame();

    // Run the application
    app.Run();

    std::cout << "Game exited successfully." << std::endl;
    return 0;
}