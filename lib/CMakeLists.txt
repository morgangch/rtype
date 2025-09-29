cmake_minimum_required(VERSION 3.20)

# Libraries root CMakeLists.txt
# This file manages all libraries in the lib directory

add_subdirectory(ecs)
add_subdirectory(packetmanager)
add_subdirectory(mapparser)
add_subdirectory(packethandler)

# Future libraries can be added here, for example:
# add_subdirectory(networking)
# add_subdirectory(physics)
# add_subdirectory(audio)
