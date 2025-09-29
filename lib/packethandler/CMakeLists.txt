cmake_minimum_required(VERSION 3.10)

# Add the packethandler library
add_library(packethandler STATIC
    src/packethandler.cpp
)

# Include directories
target_include_directories(packethandler PUBLIC
    include
    ../packetmanager/include
    ../../common/packets
)

# Link dependencies
target_link_libraries(packethandler PUBLIC packetmanager)

# Set C++ standard
set_property(TARGET packethandler PROPERTY CXX_STANDARD 17)
