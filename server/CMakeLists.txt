cmake_minimum_required(VERSION 3.20)

# Server executable - Auto-discover all .cpp files in current directory
file(GLOB_RECURSE SERVER_SOURCES "*.cpp")

add_executable(server ${SERVER_SOURCES})
# Include directories for server headers
target_include_directories(server PRIVATE include)

# Link with ECS library
target_link_libraries(server PRIVATE ecs packetmanager packethandler)

# Set output directory - handle both single and multi-config generators
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    set_target_properties(server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
else()
    # Single config generators (Make, Ninja)
    set_target_properties(server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()
