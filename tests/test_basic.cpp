#include <iostream>
#include <cassert>

// Basic test function to verify that basic functionality works
int test_basic_functionality() {
    // Test 1: Basic arithmetic
    int result = 2 + 2;
    assert(result == 4);

    // Test 2: String operations
    std::string test_str = "Hello World";
    assert(test_str.length() == 11);

    std::cout << "All basic tests passed!" << std::endl;
    return 0;
}

int main() {
    try {
        return test_basic_functionality();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
