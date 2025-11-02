/**
 * @file config.h
 * @brief Client-side configuration constants
 * 
 * This file defines compile-time configuration constants for the R-TYPE client,
 * including window settings, rendering parameters, and graphical options.
 * 
 * All values are constexpr for compile-time optimization.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_CONFIG_HPP
#define CLIENT_CONFIG_HPP

#include "common/utils/Config.h"

namespace rtype::client::configs {
    /**
     * @brief Window width in pixels
     * @details Default resolution for the game window
     */
    constexpr int WINDOW_WIDTH = 1920;
    
    /**
     * @brief Window height in pixels
     * @details Default resolution for the game window
     */
    constexpr int WINDOW_HEIGHT = 1080;
    
    /**
     * @brief Fullscreen mode toggle
     * @details If true, game runs in fullscreen; if false, runs in windowed mode
     */
    constexpr bool FULLSCREEN = false;
    
    /**
     * @brief Vertical sync toggle
     * @details If true, enables VSync to prevent screen tearing
     */
    constexpr bool VSYNC = true;
    
    /**
     * @brief Maximum frames per second
     * @details Frame rate cap for the client rendering loop
     */
    constexpr int MAX_FPS = 60;
    
    /**
     * @brief Render scaling factor
     * @details Multiplier for rendering resolution (1.0 = native, >1.0 = supersampling, <1.0 = undersampling)
     */
    constexpr float RENDER_SCALE = 1.0f;
    
    /**
     * @brief Particle effects toggle
     * @details If true, enables particle system rendering
     */
    constexpr bool ENABLE_PARTICLES = true;
    
    /**
     * @brief Shadow rendering toggle
     * @details If true, enables shadow rendering for entities
     */
    constexpr bool ENABLE_SHADOWS = true;
    
    /**
     * @brief Texture quality level
     * @details Quality setting for texture loading and filtering
     * - 0: Low (performance mode)
     * - 1: Medium (balanced)
     * - 2: High (quality mode)
     */
    constexpr int TEXTURE_QUALITY = 2;
}

#endif // CLIENT_CONFIG_HPP
