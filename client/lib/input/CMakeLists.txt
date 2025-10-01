cmake_minimum_required(VERSION 3.20)

# Client Input Library - Input management system
project(client_input)

# Check if SFML is available (should be detected by graphics library)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SFML QUIET sfml-window sfml-system)
endif()

if(SFML_FOUND)
    message(STATUS "SFML found for input library")
endif()

add_library(client_input STATIC
    src/InputManager.cpp
    src/Input.cpp
    src/SFMLKeyConverter.cpp
    include/input/InputManager.hpp
    include/input/Input.hpp
    include/input/Keys.hpp
    include/input/SFMLKeyConverter.hpp
)

if(SFML_FOUND)
    target_compile_definitions(client_input PUBLIC RTYPE_USE_SFML)
endif()

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