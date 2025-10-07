/**
 * @file render.h
 * @brief Renderable component for visual representation in the R-TYPE client
 * 
 * This file defines the Renderable component which controls how entities are
 * displayed on screen. It manages texture, size, layering, and visual properties
 * for all rendered entities in the game.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_RENDERABLE_HPP
#define CLIENT_RENDERABLE_HPP

#include <ECS/ECS.h>
#include <string>

namespace rtype::client::components {
    /**
     * @enum RenderLayer
     * @brief Defines rendering layers for z-ordering
     * 
     * Lower values are rendered first (background), higher values are rendered
     * on top (foreground). This ensures correct visual layering.
     */
    enum class RenderLayer {
        Background = 0,  ///< Background elements (starfield, nebulas)
        Entities,        ///< Game entities (ships, enemies, projectiles)
        Effects,         ///< Visual effects (explosions, particles)
        UI,              ///< User interface elements
        Debug            ///< Debug visuals (hitboxes, paths)
    };

    /**
     * @class Renderable
     * @brief Component that makes an entity visible on screen
     * 
     * The Renderable component defines how an entity should be rendered.
     * It provides complete control over the visual representation including:
     * - Texture/sprite selection
     * - Size and scaling
     * - Render layer for z-ordering
     * - Visibility toggling
     * - Transparency (alpha channel)
     * 
     * Features:
     * - Multi-layer rendering system for proper z-ordering
     * - Per-entity visibility control
     * - Alpha transparency support (0.0 = fully transparent, 1.0 = opaque)
     * - Configurable size independent of texture resolution
     * 
     * Example usage:
     * @code
     * // Create player ship with sprite
     * auto& sprite = playerEntity.addComponent<Renderable>(
     *     "textures/player_ship.png", 
     *     64.0f,  // width
     *     64.0f,  // height
     *     RenderLayer::Entities
     * );
     * 
     * // Create semi-transparent background
     * auto& bg = bgEntity.addComponent<Renderable>(
     *     "textures/stars.png",
     *     1280.0f, 720.0f,
     *     RenderLayer::Background
     * );
     * bg.alpha = 0.5f;  // 50% transparent
     * 
     * // Hide an entity without removing component
     * sprite.visible = false;
     * @endcode
     */
    class Renderable : public ECS::Component<Renderable> {
    public:
        /** @brief Path to the texture file (relative to assets folder) */
        std::string texturePath;
        
        /** @brief Width of the rendered sprite in pixels */
        float width;
        
        /** @brief Height of the rendered sprite in pixels */
        float height;
        
        /** @brief Rendering layer for z-ordering (lower = behind, higher = in front) */
        RenderLayer layer;
        
        /** @brief Whether the entity is currently visible */
        bool visible;
        
        /** @brief Alpha transparency (0.0 = fully transparent, 1.0 = fully opaque) */
        float alpha;

        /**
         * @brief Construct a new Renderable component
         * @param texturePath Path to the texture file
         * @param width Width of the sprite in pixels (default: 32.0f)
         * @param height Height of the sprite in pixels (default: 32.0f)
         * @param layer Render layer for z-ordering (default: Entities)
         * 
         * Initializes a renderable component with:
         * - Specified texture, size, and layer
         * - Visible by default (visible = true)
         * - Fully opaque (alpha = 1.0)
         */
        Renderable(const std::string& texturePath = "", float width = 32.0f, float height = 32.0f, RenderLayer layer = RenderLayer::Entities)
            : texturePath(texturePath), width(width), height(height), layer(layer), visible(true), alpha(1.0f) {}
    };
}

#endif // CLIENT_RENDERABLE_HPP

