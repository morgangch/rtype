cmake_minimum_required(VERSION 3.20)
project(rtype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)
