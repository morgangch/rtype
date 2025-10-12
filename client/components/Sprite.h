/**
 * @file Sprite.h
 * @brief Visual rendering component for entities
 * 
 * This component stores all visual properties needed to render an entity
 * in SFML, including size, color, and shape type.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_COMPONENTS_SPRITE_H
#define CLIENT_COMPONENTS_SPRITE_H

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>

namespace rtype::client::components {
    /**
     * @class Sprite
     * @brief Component for visual representation of entities
     * 
     * The Sprite component contains all data needed to render an entity
     * visually using SFML. It supports basic shapes with customizable
     * size and color.
     * 
     * Example usage:
     * @code
     * // Create a green player ship
     * world.AddComponent<Sprite>(playerEntity, 
     *     sf::Vector2f(32.0f, 32.0f),  // size
     *     sf::Color::Green);            // color
     * 
     * // Create a red enemy
     * world.AddComponent<Sprite>(enemyEntity, 
     *     sf::Vector2f(24.0f, 24.0f),
     *     sf::Color::Red);
     * @endcode
     */
    class Sprite : public ECS::Component<Sprite> {
    public:
        /** @brief Size of the sprite (width, height) in pixels */
        sf::Vector2f size;
        
        /** @brief Color of the sprite */
        sf::Color color;
        
        /** @brief Visibility flag - if false, sprite won't be rendered */
        bool visible;
        
        /**
         * @brief Constructor with custom size and color
         * @param size Size of the sprite (width, height)
         * @param color Color of the sprite
         * @param visible Initial visibility state
         */
        Sprite(const sf::Vector2f& size = sf::Vector2f(32.0f, 32.0f),
               const sf::Color& color = sf::Color::White,
               bool visible = true)
            : size(size), color(color), visible(visible) {}
    };
}

#endif // CLIENT_COMPONENTS_SPRITE_H
