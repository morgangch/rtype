/**
 * @file camera.hpp
 * @brief Camera component for viewport management in the R-TYPE client
 * 
 * This file defines the Camera component which controls the game's viewport.
 * It supports zoom control and entity following for smooth camera tracking.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_CAMERA_HPP
#define CLIENT_CAMERA_HPP

#include <ECS/ECS.hpp>
#include <common/components/Position.hpp>

namespace rtype::client::components {
    /**
     * @class Camera
     * @brief Component representing a camera/viewport in the game
     * 
     * The Camera component defines a viewport for rendering the game world.
     * It provides functionality for:
     * - Zoom control to scale the viewport
     * - Entity following for smooth camera tracking
     * 
     * Features:
     * - Adjustable zoom level (1.0 = normal, <1.0 = zoomed out, >1.0 = zoomed in)
     * - Optional entity following for dynamic camera movement
     * - Can be attached to any entity for camera-as-entity behavior
     * 
     * Example usage:
     * @code
     * // Create a camera entity
     * auto cameraEntity = ecs.createEntity();
     * auto& camera = cameraEntity.addComponent<Camera>(1.5f);  // 1.5x zoom
     * 
     * // Make camera follow the player
     * camera.followTarget = playerEntityID;
     * 
     * // For Space Invaders style (fixed camera), use zoom 1.0 and no follow target
     * auto& fixedCamera = cameraEntity.addComponent<Camera>(1.0f);
     * fixedCamera.followTarget = 0;  // No follow
     * @endcode
     */
    class Camera : public ECS::Component<Camera> {
    public:
        /**
         * @brief Zoom level of the camera
         * 
         * - 1.0 = normal view
         * - < 1.0 = zoomed out (see more of the world)
         * - > 1.0 = zoomed in (see less of the world, larger objects)
         */
        float zoom;
        
        /**
         * @brief ID of the entity to follow
         * 
         * If set to a valid entity ID, the camera will track that entity's position.
         * Set to 0 for a static camera that doesn't follow anything.
         */
        ECS::EntityID followTarget;

        /**
         * @brief Construct a new Camera component
         * @param zoom Initial zoom level (default: 1.0 for normal view)
         * 
         * Creates a camera with the specified zoom level and no follow target.
         * To make the camera follow an entity, set followTarget after construction.
         */
        Camera(float zoom = 1.0f) : zoom(zoom), followTarget(0) {}
    };
}

#endif // CLIENT_CAMERA_HPP

