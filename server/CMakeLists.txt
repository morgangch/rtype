cmake_minimum_required(VERSION 3.20)

# Server executable
add_executable(server main.cpp)

# Set output directory
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
