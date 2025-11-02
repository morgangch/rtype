/**
 * @file SimpleShape.h
 * @brief Simple geometric shape component for debris and procedural entities
 * 
 * This component allows rendering simple colored shapes without textures,
 * useful for debris, particles, and other procedural visual elements.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_COMPONENTS_SIMPLE_SHAPE_H
#define CLIENT_COMPONENTS_SIMPLE_SHAPE_H

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>

namespace rtype::client::components {
    
    /**
     * @enum ShapeType
     * @brief Types of simple geometric shapes
     */
    enum class ShapeType {
        Rectangle,  ///< Rectangular shape
        Circle,     ///< Circular shape
        Triangle    ///< Triangular shape
    };
    
    /**
     * @class SimpleShape
     * @brief Component for rendering simple colored geometric shapes
     * 
     * Used for debris, particles, and other entities that don't need textures.
     * Provides a lightweight alternative to Sprite component.
     */
    class SimpleShape : public ECS::Component<SimpleShape> {
    public:
        ShapeType type;           ///< Type of shape (rectangle, circle, etc.)
        sf::Vector2f size;        ///< Size (width, height) for rectangles
        float radius;             ///< Radius for circles
        sf::Color fillColor;      ///< Fill color of the shape
        sf::Color outlineColor;   ///< Outline color (border)
        float outlineThickness;   ///< Thickness of the outline
        float rotation;           ///< Rotation angle in degrees
        
        /**
         * @brief Constructor for rectangular shape
         */
        SimpleShape(sf::Vector2f size, 
                   sf::Color fillColor = sf::Color(100, 100, 100),
                   sf::Color outlineColor = sf::Color(60, 60, 60),
                   float outlineThickness = 1.0f)
            : type(ShapeType::Rectangle)
            , size(size)
            , radius(0.0f)
            , fillColor(fillColor)
            , outlineColor(outlineColor)
            , outlineThickness(outlineThickness)
            , rotation(0.0f)
        {}
        
        /**
         * @brief Constructor for circular shape
         */
        SimpleShape(float radius,
                   sf::Color fillColor = sf::Color(100, 100, 100),
                   sf::Color outlineColor = sf::Color(60, 60, 60),
                   float outlineThickness = 1.0f)
            : type(ShapeType::Circle)
            , size(sf::Vector2f(0, 0))
            , radius(radius)
            , fillColor(fillColor)
            , outlineColor(outlineColor)
            , outlineThickness(outlineThickness)
            , rotation(0.0f)
        {}
    };
    
} // namespace rtype::client::components

#endif // CLIENT_COMPONENTS_SIMPLE_SHAPE_H
