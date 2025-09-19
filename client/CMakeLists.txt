cmake_minimum_required(VERSION 3.20)

# Client executable
add_executable(client main.cpp)

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
