cmake_minimum_required(VERSION 3.20)

# Add client libraries subdirectory first
add_subdirectory(lib)

# Client executable - only include main.cpp and specific client sources (not library sources)
add_executable(client 
    main.cpp
    # Add any other client-specific source files here (not in lib/)
)

# Include directories for client headers
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # Parent directory to find client/components/
    ${CMAKE_CURRENT_SOURCE_DIR}     # Current directory
)

# Link with ECS, common libraries, and client libraries
target_link_libraries(client PRIVATE 
    ecs 
    common 
    packethandler 
    packetmanager
    client_libs  # This provides client_graphics, client_input, etc.
)

target_include_directories(client PRIVATE include)

target_link_libraries(client PRIVATE sfml-graphics sfml-window sfml-system)


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