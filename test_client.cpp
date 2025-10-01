#include <iostream>
#include <cstdlib>
#include <ctime>
#include "TestMode.hpp"

using namespace rtype::client;

int main() {
    std::cout << "🧪 R-Type Input & Performance Test Utility" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    TestMode testMode;
    
    if (!testMode.Initialize()) {
        std::cout << "❌ Failed to initialize test mode!" << std::endl;
        return -1;
    }
    
    testMode.Run();
    
    std::cout << "✅ Test completed successfully." << std::endl;
    return 0;
}