cmake_minimum_required(VERSION 3.20)

# Client executable - Auto-discover all .cpp files in current directory and subdirectories
file(GLOB_RECURSE CLIENT_SOURCES "*.cpp")

add_executable(client ${CLIENT_SOURCES})

# Include directories for client headers
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # Parent directory to find client/components/
    ${CMAKE_CURRENT_SOURCE_DIR}     # Current directory
)

# Link with ECS and common libraries
target_link_libraries(client PRIVATE ecs common packethandler packetmanager)

target_include_directories(client PRIVATE include)


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
