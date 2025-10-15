cmake_minimum_required(VERSION 3.20)

# ECS Systems Example
add_executable(ecs_systems_example ecs_systems_example.cpp)

# Link with the ECS library
target_link_libraries(ecs_systems_example ecs)

# Include ECS headers
target_include_directories(ecs_systems_example PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/ecs/include
)

# Set C++ standard
target_compile_features(ecs_systems_example PRIVATE cxx_std_17)

# Set output directory
set_target_properties(ecs_systems_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
