cmake_minimum_required(VERSION 3.20)

# PacketManager Library
add_library(packetmanager STATIC
    src/packetManager.cpp
    include/packetmanager.h
)

# Include directories
target_include_directories(packetmanager PUBLIC  include)

# Set C++ standard
target_compile_features(packetmanager PUBLIC cxx_std_17)

# Set output directory for library
set_target_properties(packetmanager PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
