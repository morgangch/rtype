/**
 * @file RenderSystem.h
 * @brief System for rendering entities with sprites
 * 
 * This system handles all visual rendering of entities using SFML,
 * combining Position and Sprite components.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_SYSTEMS_RENDER_SYSTEM_H
#define CLIENT_SYSTEMS_RENDER_SYSTEM_H

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>
#include <common/components/Position.h>
#include <client/components/Sprite.h>
#include <client/components/Invulnerability.h>

namespace rtype::client::systems {
    /**
     * @class RenderSystem
     * @brief Renders all entities with visual components
     * 
     * The RenderSystem draws entities to the screen using their Position
     * and Sprite components. It handles:
     * - Basic shape rendering (rectangles)
     * - Color and size customization
     * - Invulnerability blinking effect
     * - Visibility toggling
     * 
     * Required Components:
     * - Position: Entity's screen location
     * - Sprite: Visual properties (size, color)
     * 
     * Optional Components:
     * - Invulnerability: For blinking effect
     */
    class RenderSystem {
    public:
        /**
         * @brief Render all visible entities to the window
         * @param world The ECS world containing entities
         * @param window SFML render window
         */
        void render(ECS::World& world, sf::RenderWindow& window) {
            // Get all positions
            auto* positions = world.GetAllComponents<rtype::common::components::Position>();
            if (!positions) return;
            
            for (auto& [entity, posPtr] : *positions) {
                auto& pos = *posPtr;
                auto* sprite = world.GetComponent<rtype::client::components::Sprite>(entity);
                
                if (!sprite || !sprite->visible) continue;
                
                // Check invulnerability for blinking effect
                auto* invuln = world.GetComponent<rtype::client::components::Invulnerability>(entity);
                if (invuln && invuln->isActive()) {
                    if (!invuln->shouldRender()) {
                        continue; // Skip rendering for blink effect
                    }
                }
                
                // Draw entity as rectangle
                sf::RectangleShape shape(sprite->size);
                shape.setPosition(pos.x - sprite->size.x * 0.5f, 
                                 pos.y - sprite->size.y * 0.5f);
                shape.setFillColor(sprite->color);
                window.draw(shape);
            }
        }
    };
}

#endif // CLIENT_SYSTEMS_RENDER_SYSTEM_H
