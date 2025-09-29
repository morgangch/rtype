cmake_minimum_required(VERSION 3.20)

# Client Graphics Library - SFML-based rendering system
project(client_graphics)

# Find SFML
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SFML QUIET sfml-graphics sfml-window sfml-system)
endif()

if(SFML_FOUND)
    message(STATUS "SFML found via pkg-config, enabling graphics library")
    set(RTYPE_USE_SFML ON)
    
    add_library(client_graphics STATIC
        src/SFMLRenderer.cpp
        src/SFMLTexture.cpp
        src/Sprite.cpp
        src/Camera.cpp
        src/Graphics.cpp
        include/graphics/Renderer.hpp
        include/graphics/Texture.hpp
        include/graphics/Sprite.hpp
        include/graphics/Camera.hpp
        include/graphics/Graphics.hpp
    )
    
    target_link_libraries(client_graphics PRIVATE ${SFML_LIBRARIES})
    target_include_directories(client_graphics PRIVATE ${SFML_INCLUDE_DIRS})
    target_compile_definitions(client_graphics PUBLIC RTYPE_USE_SFML)
    
else()
    # Try direct library search
    find_library(SFML_GRAPHICS_LIB sfml-graphics)
    find_library(SFML_WINDOW_LIB sfml-window)
    find_library(SFML_SYSTEM_LIB sfml-system)
    
    if(SFML_GRAPHICS_LIB AND SFML_WINDOW_LIB AND SFML_SYSTEM_LIB)
        message(STATUS "SFML found via direct search, enabling graphics library")
        set(RTYPE_USE_SFML ON)
        
        add_library(client_graphics STATIC
            src/SFMLRenderer.cpp
            src/SFMLTexture.cpp
            src/Sprite.cpp
            src/Camera.cpp
            src/Graphics.cpp
            include/graphics/Renderer.hpp
            include/graphics/Texture.hpp
            include/graphics/Sprite.hpp
            include/graphics/Camera.hpp
            include/graphics/Graphics.hpp
        )
        
        target_link_libraries(client_graphics PRIVATE ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
        target_compile_definitions(client_graphics PUBLIC RTYPE_USE_SFML)
    
    else()
        message(WARNING "SFML not found, graphics library will use stub implementation")
        message(STATUS "Install SFML: sudo apt-get install libsfml-dev")
        
        add_library(client_graphics STATIC
            src/StubRenderer.cpp
            src/StubTexture.cpp
            src/Sprite.cpp
            src/Camera.cpp
            src/Graphics.cpp
            include/graphics/Renderer.hpp
            include/graphics/Texture.hpp
            include/graphics/Sprite.hpp
            include/graphics/Camera.hpp
            include/graphics/Graphics.hpp
        )
    endif()
endif()

# Include directories
target_include_directories(client_graphics PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link with common components
target_include_directories(client_graphics PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)

# Dependencies
target_link_libraries(client_graphics PUBLIC ecs)

# Compiler features
target_compile_features(client_graphics PUBLIC cxx_std_17)
