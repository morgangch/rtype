cmake_minimum_required(VERSION 3.20)

# Common Components Library - Auto-discover all .cpp files in components directory
file(GLOB_RECURSE COMMON_SOURCES "components/*.cpp")

add_library(common STATIC ${COMMON_SOURCES})

# Include directories
target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..  # Parent directory to find common/components/
    ${CMAKE_CURRENT_SOURCE_DIR}     # Current directory
)

# Link with ECS library (common components depend on ECS)
target_link_libraries(common PUBLIC ecs)

# Set C++ standard
target_compile_features(common PUBLIC cxx_std_17)

# Set output directory for library
set_target_properties(common PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
