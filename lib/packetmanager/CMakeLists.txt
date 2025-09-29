cmake_minimum_required(VERSION 3.20)

# PacketManager Library - Auto-discover all .cpp files in src directory
file(GLOB_RECURSE PACKETMANAGER_SOURCES "src/*.cpp")

add_library(packetmanager STATIC ${PACKETMANAGER_SOURCES})

# Include directories
target_include_directories(packetmanager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard
target_compile_features(packetmanager PUBLIC cxx_std_17)

# Set output directory for library
set_target_properties(packetmanager PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
