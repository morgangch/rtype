cmake_minimum_required(VERSION 3.20)

# Client executable
add_executable(client main.cpp)

# Set output directory
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
