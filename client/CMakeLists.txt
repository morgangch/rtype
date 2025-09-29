cmake_minimum_required(VERSION 3.20)

# Client executable
add_executable(client
    main.cpp
    ../common/components/Position.cpp
    ../common/components/Velocity.cpp
    components/render.cpp
    components/camera.cpp
    components/audio.cpp
)

# Link with ECS library
target_link_libraries(client PRIVATE ecs)

target_include_directories(client PRIVATE 
    ${CMAKE_SOURCE_DIR}  # This allows #include <common/components/Position.hpp>
    ${CMAKE_CURRENT_SOURCE_DIR}  # For local includes
)

# Set output directory - handle both single and multi-config generators
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    set_target_properties(client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
else()
    # Single config generators (Make, Ninja)
    set_target_properties(client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()
