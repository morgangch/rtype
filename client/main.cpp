#include <iostream>
#include <cstdlib>
#include <ctime>
#include "application/GameApp.hpp"

using namespace rtype::client;

int main() {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Create game application
    GameApp app;
    
    // Initialize application
    if (!app.Initialize()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return -1;
    }
    
    // Run the application
    app.Run();
    
    std::cout << "Application exited successfully." << std::endl;
    return 0;
}