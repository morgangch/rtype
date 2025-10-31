/**
 * @file Accessibility.h
 * @brief Global accessibility manager for colorblindness (Daltonism) filters
 *
 * This header declares the Accessibility singleton used to enable and
 * configure post-process color filters that simulate common types of
 * color vision deficiencies (Protanopia, Deuteranopia, Tritanopia) as
 * well as Achromatopsia (grayscale). The implementation relies on an
 * SFML fragment shader located at assets/shaders/colorblind.frag and is
 * applied as a full-screen post-process pass by the rendering states.
 *
 * Features:
 * - Simple global API: set/get current daltonism mode (0..4)
 * - Optional intensity control (blend strength 0..1)
 * - Lazy loading of the GLSL fragment shader on first use
 * - Designed to be applied after the frame is rendered to the window
 *
 * Typical usage:
 * 1) On startup or when loading settings: Accessibility::instance().setMode(index);
 * 2) During rendering (last step):
 *    - Copy the window content to a texture
 *    - Draw a full-screen sprite with the Accessibility shader
 *
 * See also:
 * - client/src/gui/SettingsState.cpp (selection UI and preview)
 * - client/src/gui/GameState.cpp (in-game application of the filter)
 */

#ifndef CLIENT_GUI_ACCESSIBILITY_H
#define CLIENT_GUI_ACCESSIBILITY_H

#include <SFML/Graphics.hpp>

namespace rtype::client::gui {

/**
 * @class Accessibility
 * @brief Singleton that manages colorblindness simulation state and shader
 *
 * The Accessibility manager stores the currently selected daltonism mode and
 * exposes a shader configured with the appropriate uniforms to recolor the
 * whole frame. Call getShader() each frame you want to apply the effect; it
 * will return nullptr if no mode is active or if the shader failed to load.
 */
class Accessibility {
public:
    /**
     * @brief Access the global Accessibility instance
     * @return Reference to the singleton
     */
    static Accessibility& instance();

    /**
     * @brief Set current daltonism mode
     * @param mode Index in [0..4]: 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia
     *
     * Values outside the range are clamped. When set to a non-zero value,
     * the shader is lazily loaded on first use.
     */
    void setMode(int mode);

    /**
     * @brief Get current daltonism mode index
     * @return Current mode in [0..4]
     */
    int getMode() const { return m_mode; }

    /**
     * @brief Check whether a colorblind mode is active
     * @return true if mode > 0
     */
    bool isEnabled() const { return m_mode > 0; }

    /**
     * @brief Retrieve the configured shader for the active mode
     * @return Pointer to SFML shader or nullptr if disabled/unavailable
     *
     * The following GLSL uniforms are set when returning a non-null pointer:
     * - int mode       (0..4)   selected daltonism mode
     * - float strength (0..1)   blend factor between original and simulated color
     */
    sf::Shader* getShader();

    /**
     * @brief Set blend strength for the effect
     * @param s Blend factor in [0..1] (0 = original colors, 1 = full simulation)
     */
    void setStrength(float s) { m_strength = s; }

    /**
     * @brief Get current blend strength
     * @return Blend factor in [0..1]
     */
    float getStrength() const { return m_strength; }

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    Accessibility() = default;
    Accessibility(const Accessibility&) = delete;
    Accessibility& operator=(const Accessibility&) = delete;

    /**
     * @brief Load the fragment shader if not already loaded
     * @return true on success, false if loading failed
     */
    bool ensureShaderLoaded();

    int m_mode{0};             ///< Active mode: 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia
    float m_strength{1.0f};    ///< Blend strength for simulation [0..1]
    sf::Shader m_shader;       ///< Fragment shader instance used for recoloring
    bool m_shaderLoaded{false};///< True if m_shader has been loaded successfully
};

} // namespace rtype::client::gui

#endif // CLIENT_GUI_ACCESSIBILITY_H
