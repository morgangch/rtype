cmake_minimum_required(VERSION 3.20)

# mapparser Library
add_library(mapparser STATIC
    src/mapparser.cpp
    include/mapparser.h
)

# Include directories
target_include_directories(mapparser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard
target_compile_features(mapparser PUBLIC cxx_std_17)

# Set output directory for library
set_target_properties(mapparser PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
