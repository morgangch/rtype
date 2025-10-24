/**
 * @file RenderSystem.h
 * @brief Client-side rendering system
 * 
 * This file defines the RenderSystem class which handles rendering of all visual
 * entities in the game, including layer-based rendering and depth sorting.
 * 
 * Part of the ECS architecture for visual output.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_RENDER_SYSTEM_HPP
#define CLIENT_RENDER_SYSTEM_HPP

#include <common/core/System.h>

namespace rtype::client::systems {
    /**
     * @brief ECS system for rendering game entities
     * 
     * @details Manages the rendering pipeline with support for:
     * - Layer-based rendering (background, entities, UI, etc.)
     * - Depth sorting within layers for proper visual ordering
     * - Sprite/texture rendering via SFML
     * 
     * The rendering order is:
     * 1. Sort entities by layer and depth
     * 2. Render each layer sequentially
     * 3. Within each layer, render entities in depth order (back to front)
     * 
     * Usage: Call update() once per frame after all game logic has been processed.
     * 
     * Thread safety: Not thread-safe. Must be called from the rendering thread.
     */
    class RenderSystem : public System {
    public:
        /**
         * @brief Update and render all visual entities
         * @param cm Component manager containing all entity components
         * @param deltaTime Time elapsed since last frame in seconds (unused in rendering)
         * 
         * @details Processes all entities with renderable components and draws them
         * to the screen in the correct order. Handles:
         * - Sprite rendering
         * - Texture application
         * - Position/rotation/scale transformations
         * - Layer and depth-based ordering
         * 
         * Should be called once per frame after game logic updates.
         */
        void update(ComponentManager& cm, float deltaTime) override;

    private:
        /**
         * @brief Render all entities in a specific layer
         * @param layer Layer index to render
         * 
         * @details Renders all entities belonging to the specified layer in depth order.
         * Lower layer numbers are rendered first (background), higher numbers last (foreground).
         * 
         * Common layer organization:
         * - Layer 0: Background/parallax
         * - Layer 1: Game entities (enemies, player, projectiles)
         * - Layer 2: Effects/particles
         * - Layer 3: UI/HUD
         */
        void renderLayer(int layer);
        
        /**
         * @brief Sort entities by depth for correct rendering order
         * 
         * @details Sorts entities within each layer by their depth/z-coordinate to ensure
         * proper visual ordering (back-to-front rendering). Uses stable sort to maintain
         * relative ordering of entities with the same depth.
         * 
         * Called automatically before rendering each layer.
         */
        void sortByDepth();
    };
}

#endif // CLIENT_RENDER_SYSTEM_HPP
