#include <iostream>
#include <cstdlib>
#include <ctime>
#include "application/MenuApplication.hpp"

using namespace rtype::client;

int main() {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Create menu application
    MenuApplication app;
    
    // Initialize application
    if (!app.Initialize(1280, 720, "R-TYPE - Main Menu")) {
        std::cout << "Failed to initialize menu application!" << std::endl;
        return -1;
    }
    
    // Run the application
    app.Run();
    
    std::cout << "Application exited successfully." << std::endl;
    return 0;
}