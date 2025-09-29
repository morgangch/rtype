cmake_minimum_required(VERSION 3.20)

# Client Input Library - Input management system
project(client_input)

add_library(client_input STATIC
    src/InputManager.cpp
    src/KeyboardInput.cpp
    src/MouseInput.cpp
    src/GamepadInput.cpp
    include/input/InputManager.hpp
    include/input/KeyboardInput.hpp
    include/input/MouseInput.hpp
    include/input/GamepadInput.hpp
    include/input/Input.hpp
    include/input/Keys.hpp
)

# Include directories
target_include_directories(client_input PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link with common components
target_include_directories(client_input PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)

# Dependencies
target_link_libraries(client_input PUBLIC ecs)

# Compiler features
target_compile_features(client_input PUBLIC cxx_std_17)