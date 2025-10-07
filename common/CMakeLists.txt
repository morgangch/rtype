cmake_minimum_required(VERSION 3.20)

# Common Components Library - Header-only library for ECS components
# Since all components are header-only, we create an INTERFACE library

add_library(common INTERFACE)

# Include directories
target_include_directories(common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # Parent directory to find common/components/
    ${CMAKE_CURRENT_SOURCE_DIR}     # Current directory
)

find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)

# Link with ECS library (common components depend on ECS)
target_link_libraries(common INTERFACE ecs)

# Set C++ standard
target_compile_features(common INTERFACE cxx_std_17)
