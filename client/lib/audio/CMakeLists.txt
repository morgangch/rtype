cmake_minimum_required(VERSION 3.20)

# Client Audio Library - SFML-based audio system
project(client_audio)

# Find SFML Audio
find_package(sfml-audio QUIET)

if(sfml-audio_FOUND)
    message(STATUS "SFML Audio found, enabling audio library")
    set(RTYPE_USE_SFML_AUDIO ON)
    
    add_library(client_audio STATIC
        src/SFMLAudioManager.cpp
        src/Sound.cpp
        src/Music.cpp
        include/audio/AudioManager.hpp
        include/audio/Sound.hpp
        include/audio/Music.hpp
        include/audio/Audio.hpp
    )
    
    target_link_libraries(client_audio PRIVATE sfml-audio)
    target_compile_definitions(client_audio PUBLIC RTYPE_USE_SFML_AUDIO)
else()
    message(WARNING "SFML Audio not found, audio library will use stub implementation")
    
    add_library(client_audio STATIC
        src/StubAudioManager.cpp
        src/Sound.cpp
        src/Music.cpp
        include/audio/AudioManager.hpp
        include/audio/Sound.hpp
        include/audio/Music.hpp
        include/audio/Audio.hpp
    )
endif()

# Include directories
target_include_directories(client_audio PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler features
target_compile_features(client_audio PUBLIC cxx_std_17)