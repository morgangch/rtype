#pragma once
#include <common/utils/Config.hpp>

namespace Client::Config {
    constexpr int WINDOW_WIDTH = 1920;
    constexpr int WINDOW_HEIGHT = 1080;
    constexpr bool FULLSCREEN = false;
    constexpr bool VSYNC = true;
    
    constexpr int MAX_FPS = 60;
    constexpr float RENDER_SCALE = 1.0f;
    
    // Graphical settings
    constexpr bool ENABLE_PARTICLES = true;
    constexpr bool ENABLE_SHADOWS = true;
    constexpr int TEXTURE_QUALITY = 2; // 0=Low, 1=Medium, 2=High
}
