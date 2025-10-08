cmake_minimum_required(VERSION 3.20)

# MapParser Demo Executable
add_executable(mapparser_demo main.cpp)

# Link mapparser library
target_link_libraries(mapparser_demo PRIVATE mapparser)

# Include directories
target_include_directories(mapparser_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/mapparser/include
)

# Set C++ standard
target_compile_features(mapparser_demo PUBLIC cxx_std_17)

# Set output directory
set_target_properties(mapparser_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
