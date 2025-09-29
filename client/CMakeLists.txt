cmake_minimum_required(VERSION 3.20)

# Add client libraries subdirectory first
add_subdirectory(lib)

# Client executable
add_executable(client
    main.cpp
)

# Link with libraries
target_link_libraries(client PRIVATE 
    ecs
    client_graphics
)

# Add include directories
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/client)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/lib/ecs/include)

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
