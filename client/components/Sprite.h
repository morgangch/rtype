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
     * visually using SFML. It supports both textures and basic colored shapes.
     * 
     * Example usage:
     * @code
     * // Create a sprite with texture
     * world.AddComponent<Sprite>(playerEntity, 
     *     "assets/sprites/player.png",
     *     sf::Vector2f(32.0f, 32.0f));
     * 
     * // Create a colored shape (fallback)
     * world.AddComponent<Sprite>(enemyEntity, 
     *     sf::Vector2f(24.0f, 24.0f),
     *     sf::Color::Red);
     * @endcode
     */
    class Sprite : public ECS::Component<Sprite> {
    public:
        /** @brief Path to the texture file */
        std::string texturePath;
        
        /** @brief SFML texture object */
        sf::Texture texture;
        
        /** @brief SFML sprite object for rendering */
        sf::Sprite sprite;
        
        /** @brief Size of the sprite (width, height) in pixels */
        sf::Vector2f size;
        
        /** @brief Color of the sprite (tint or fallback for shapes) */
        sf::Color color;
        
        /** @brief Visibility flag - if false, sprite won't be rendered */
        bool visible;
        
        /** @brief Whether to use texture or colored shape */
        bool useTexture;
        
        /** @brief Rectangle defining which part of the texture to use (for spritesheets) */
        sf::IntRect textureRect;
        
        /** @brief Scale factor for rendering */
        float scale;
        
        /**
         * @brief Constructor with texture path and spritesheet support
         * @param texturePath Path to the texture file
         * @param size Size of the sprite (width, height) for rendering
         * @param visible Initial visibility state
         * @param frameRect Rectangle defining which part of texture to use (for spritesheets)
         * @param scale Scale multiplier (default 2.0 for larger sprites)
         */
        Sprite(const std::string& texturePath, 
               const sf::Vector2f& size = sf::Vector2f(32.0f, 32.0f),
               bool visible = true,
               const sf::IntRect& frameRect = sf::IntRect(0, 0, 0, 0),
               float scale = 2.0f)
            : texturePath(texturePath), 
              size(size), 
              color(sf::Color::White), 
              visible(visible), 
              useTexture(true), 
              textureRect(frameRect), 
              scale(scale) {
            // No logic in constructor - just initialize member variables
        }
        
        /**
         * @brief Constructor with custom size and color (no texture)
         * @param size Size of the sprite (width, height)
         * @param color Color of the sprite
         * @param visible Initial visibility state
         */
        Sprite(const sf::Vector2f& size = sf::Vector2f(32.0f, 32.0f),
               const sf::Color& color = sf::Color::White,
               bool visible = true)
            : texturePath(""), size(size), color(color), visible(visible), 
              useTexture(false), textureRect(0, 0, 0, 0), scale(1.0f) {}
    };
}

#endif // CLIENT_COMPONENTS_SPRITE_H
