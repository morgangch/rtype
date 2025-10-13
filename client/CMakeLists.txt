cmake_minimum_required(VERSION 3.20)

# Client executable - Auto-discover all .cpp files in current directory and subdirectories
file(GLOB_RECURSE CLIENT_SOURCES "*.cpp")

add_executable(client ${CLIENT_SOURCES})

# Include directories for client headers
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # Parent directory to find client/components/
    ${CMAKE_CURRENT_SOURCE_DIR}     # Current directory
)

find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)

# If SFML not found, provide helpful error message
if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML 2.5 or later is required but not found. Please install SFML:\n"
                        "  Ubuntu/Debian: sudo apt-get install libsfml-dev\n"
                        "  macOS: brew install sfml@2\n"
                        "  Windows: vcpkg install sfml")
endif()

# Debug: Print SFML variables
message(STATUS "SFML_FOUND: ${SFML_FOUND}")
message(STATUS "SFML_VERSION: ${SFML_VERSION}")
message(STATUS "SFML_INCLUDE_DIR: ${SFML_INCLUDE_DIR}")
message(STATUS "SFML_LIBRARIES: ${SFML_LIBRARIES}")

# Link with ECS and common libraries
target_link_libraries(client PRIVATE ecs common packethandler packetmanager)

target_include_directories(client PRIVATE include)

# Link SFML libraries - try both modern targets and legacy libraries
if(TARGET sfml-graphics)
    # Modern CMake targets (SFML 2.5+)
    target_link_libraries(client PRIVATE 
        sfml-graphics 
        sfml-window 
        sfml-system 
        sfml-audio
        sfml-network
    )
else()
    # Legacy library names
    target_link_libraries(client PRIVATE 
        ${SFML_LIBRARIES}
        ${SFML_DEPENDENCIES}
    )
endif()


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