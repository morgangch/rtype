cmake_minimum_required(VERSION 3.10)

# PacketHandler Library - Auto-discover all .cpp files in src directory
file(GLOB_RECURSE PACKETHANDLER_SOURCES "src/*.cpp")

add_library(packethandler STATIC ${PACKETHANDLER_SOURCES})

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
