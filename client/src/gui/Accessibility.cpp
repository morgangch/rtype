/**
 * @file Accessibility.cpp
 * @brief Implementation of the global accessibility manager (daltonism filters)
 *
 * Provides runtime configuration and lazy loading of the GLSL shader used to
 * simulate color vision deficiencies across the entire frame. See
 * Accessibility.h for the public API and usage details.
 */
#include "gui/Accessibility.h"
#include <iostream>

namespace rtype::client::gui {

/**
 * @brief Get the singleton Accessibility instance.
 * @return Reference to the global Accessibility manager
 */
Accessibility& Accessibility::instance() {
    static Accessibility inst;
    return inst;
}

/**
 * @brief Set the current daltonism (colorblindness) simulation mode.
 * @param mode Index in [0..4]: 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia
 *
 * Values are clamped to the valid range. When enabling a non-zero mode, the
 * fragment shader is lazily loaded on first use.
 */
void Accessibility::setMode(int mode) {
    if (mode < 0) mode = 0;
    if (mode > 4) mode = 4;
    m_mode = mode;
    if (m_mode > 0 && !m_shaderLoaded) {
        ensureShaderLoaded();
    }
}

/**
 * @brief Ensure the colorblind simulation shader is loaded into memory.
 * @return true if the shader is available, false otherwise (logs on failure)
 */
bool Accessibility::ensureShaderLoaded() {
    if (m_shaderLoaded) return true;
    // Load fragment shader for colorblind simulation
    if (!m_shader.loadFromFile("assets/shaders/colorblind.frag", sf::Shader::Fragment)) {
        std::cerr << "Accessibility: failed to load shader assets/shaders/colorblind.frag" << std::endl;
        m_shaderLoaded = false;
        return false;
    }
    m_shaderLoaded = true;
    return true;
}

/**
 * @brief Retrieve the configured SFML shader for the active mode.
 * @return Pointer to the shader or nullptr if disabled/unavailable
 *
 * Sets the following uniforms when returning a non-null shader pointer:
 * - int mode       (0..4)   selected daltonism mode
 * - float strength (0..1)   blend factor between original and simulated color
 */
sf::Shader* Accessibility::getShader() {
    if (m_mode <= 0) return nullptr;
    if (!ensureShaderLoaded()) return nullptr;
    // Setup uniforms that don't change per draw here if needed
    m_shader.setUniform("mode", m_mode);
    m_shader.setUniform("strength", m_strength);
    return &m_shader;
}

} // namespace rtype::client::gui
