/**
 * @file Position.hpp
 * @brief Position component for 2D spatial positioning
 * 
 * This file defines the Position component which represents an entity's
 * location and rotation in 2D space. This is one of the most fundamental
 * components used by nearly all game entities.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_POSITION_HPP
#define COMMON_POSITION_HPP

#include <ECS/ECS.h>

namespace rtype::common::components {
    /**
     * @class Position
     * @brief Component representing 2D position and rotation
     * 
     * The Position component stores an entity's location in 2D world space
     * and its rotation angle. This is a core component used by:
     * - All visible entities (players, enemies, projectiles)
     * - Collision systems for spatial queries
     * - Camera systems for viewport positioning
     * - Movement systems for updating locations
     * 
     * Coordinates:
     * - X increases to the right
     * - Y increases downwards (standard screen coordinates)
     * - Rotation is in degrees (0° = right, 90° = down, 180° = left, 270° = up)
     * 
     * Example usage:
     * @code
     * // Create entity at center of screen, facing right
     * auto& pos = entity.addComponent<Position>(640.0f, 360.0f, 0.0f);
     * 
     * // Move entity
     * pos.x += velocity.vx * deltaTime;
     * pos.y += velocity.vy * deltaTime;
     * 
     * // Rotate entity 90 degrees clockwise
     * pos.rotation += 90.0f;
     * @endcode
     */
    class Position : public ECS::Component<Position> {
    public:
        /** @brief X coordinate in world space (pixels from left) */
        float x;
        
        /** @brief Y coordinate in world space (pixels from top) */
        float y;
        
        /** @brief Rotation angle in degrees (0° = right, clockwise positive) */
        float rotation;

        /**
         * @brief Construct a new Position component
         * @param x Initial X coordinate (default: 0.0)
         * @param y Initial Y coordinate (default: 0.0)
         * @param rotation Initial rotation in degrees (default: 0.0)
         */
        Position(float x = 0.0f, float y = 0.0f, float rotation = 0.0f) 
            : x(x), y(y), rotation(rotation) {}
    };
}

#endif // COMMON_POSITION_HPP

