cmake_minimum_required(VERSION 3.20)

# Client Libraries CMakeLists

# Add subdirectories for each client library that exists
add_subdirectory(graphics)
# add_subdirectory(input)
# add_subdirectory(audio)
# add_subdirectory(ui)
# add_subdirectory(animation)
# add_subdirectory(physics)
# add_subdirectory(assets)
# add_subdirectory(scene)
# add_subdirectory(effects)

# Create a meta-target that links available client libraries
add_library(client_libs INTERFACE)

target_link_libraries(client_libs INTERFACE
    client_graphics
    # client_input
    # client_audio
    # client_ui
    # client_animation
    # client_physics
    # client_assets
    # client_scene
    # client_effects
)