/**
 * @file Velocity.hpp
 * @brief Velocity component for 2D movement
 * 
 * This file defines the Velocity component which represents an entity's
 * movement speed and direction in 2D space. Used by movement systems
 * to update positions over time.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_VELOCITY_HPP
#define COMMON_COMPONENTS_VELOCITY_HPP
#include <ECS/ECS.hpp>

namespace rtype::common::components {
    /**
     * @class Velocity
     * @brief Component representing 2D velocity with speed limiting
     * 
     * The Velocity component stores an entity's movement vector and maximum
     * speed. It is used by physics/movement systems to update Position components.
     * 
     * Features:
     * - 2D velocity vector (vx, vy)
     * - Maximum speed clamping to prevent excessive velocities
     * - Works with Position component for automatic movement
     * 
     * Velocity is in pixels per second:
     * - vx > 0: moving right
     * - vx < 0: moving left
     * - vy > 0: moving down
     * - vy < 0: moving up
     * 
     * Example usage:
     * @code
     * // Create fast-moving enemy
     * auto& vel = enemy.addComponent<Velocity>(200.0f, 0.0f, 250.0f);
     * // Enemy moves right at 200 px/s, max speed 250 px/s
     * 
     * // In physics system update:
     * position.x += velocity.vx * deltaTime;
     * position.y += velocity.vy * deltaTime;
     * 
     * // Clamp to max speed
     * float speed = std::sqrt(velocity.vx * velocity.vx + velocity.vy * velocity.vy);
     * if (speed > velocity.maxSpeed) {
     *     velocity.vx = (velocity.vx / speed) * velocity.maxSpeed;
     *     velocity.vy = (velocity.vy / speed) * velocity.maxSpeed;
     * }
     * @endcode
     */
    class Velocity : public ECS::Component<Velocity> {
    public:
        /** @brief Velocity along X axis (pixels per second) */
        float vx;
        
        /** @brief Velocity along Y axis (pixels per second) */
        float vy;
        
        /** @brief Maximum allowed speed magnitude (pixels per second) */
        float maxSpeed;

        /**
         * @brief Construct a new Velocity component
         * @param vx Initial X velocity in pixels/second (default: 0.0)
         * @param vy Initial Y velocity in pixels/second (default: 0.0)
         * @param maxSpeed Maximum speed magnitude (default: 100.0 px/s)
         */
        Velocity(float vx = 0.0f, float vy = 0.0f, float maxSpeed = 100.0f)
            : vx(vx), vy(vy), maxSpeed(maxSpeed) {}
    };
}
#endif // COMMON_COMPONENTS_VELOCITY_HPP

