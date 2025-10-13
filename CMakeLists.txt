cmake_minimum_required(VERSION 3.20)
project(rtype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(common)  # Add common components library
add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client)

# Skip tests on Windows due to MSVC compatibility issues with test code
if(NOT WIN32)
    add_subdirectory(tests)
endif()

add_subdirectory(examples/mapparser_demo)  # MapParser demo program
