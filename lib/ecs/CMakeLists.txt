 cmake_minimum_required(VERSION 3.20)

# ECS Library
add_library(ecs STATIC
    src/Entity.cpp
    src/ComponentManager.cpp
    src/EntityManager.cpp
    src/World.cpp
)

# Include directories
target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set C++ standard
target_compile_features(ecs PUBLIC cxx_std_17)

# Set output directory for library
set_target_properties(ecs PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
