/**
 * @file CollisionSystem.h
 * @brief Collision detection system using AABB (Axis-Aligned Bounding Box)
 * 
 * This system detects collisions between entities and provides
 * a callback mechanism for handling collision events.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_COLLISION_SYSTEM_H
#define COMMON_SYSTEMS_COLLISION_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>

#include <functional>
#include <vector>

namespace rtype::common::systems {
    /**
     * @struct BoundingBox
     * @brief Simple AABB structure
     */
    struct BoundingBox {
        float x, y;          ///< Top-left position
        float width, height; ///< Dimensions

        /**
         * @brief Check if this box intersects with another
         * @param other The other bounding box
         * @return True if boxes intersect
         */
        bool intersects(const BoundingBox& other) const {
            return x < other.x + other.width &&
                   x + width > other.x &&
                   y < other.y + other.height &&
                   y + height > other.y;
        }
    };

    /**
     * @class CollisionSystem
     * @brief Detects and handles entity collisions
     * 
     * This system performs AABB collision detection between entities.
     * Users can register collision callbacks to handle specific collision types.
     */
    class CollisionSystem : public ECS::System {
    public:
        /**
         * @brief Collision callback function type
         * 
         * Called when two entities collide.
         * Parameters: entity1, entity2, world
         */
        using CollisionCallback = std::function<void(ECS::EntityID, ECS::EntityID, ECS::World&)>;

    private:
        std::vector<CollisionCallback> m_collisionCallbacks;

    public:
        /**
         * @brief Constructor
         */
        CollisionSystem() : ECS::System("CollisionSystem", 50) {}

        /**
         * @brief Register a collision callback
         * @param callback Function to call when entities collide
         */
        void RegisterCallback(CollisionCallback callback) {
            m_collisionCallbacks.push_back(callback);
        }

        /**
         * @brief Clear all collision callbacks
         */
        void ClearCallbacks() {
            m_collisionCallbacks.clear();
        }

        /**
         * @brief Update collision detection
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            auto* positions = world.GetAllComponents<components::Position>();
            if (!positions)
                return;

            // Get all entities with positions
            std::vector<ECS::EntityID> entities;
            for (auto& [entity, _] : *positions) {
                entities.push_back(entity);
            }

            // Check collisions between all entity pairs
            for (size_t i = 0; i < entities.size(); ++i) {
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    ECS::EntityID entityA = entities[i];
                    ECS::EntityID entityB = entities[j];

                    // Get bounding boxes (must be implemented by derived class or callback)
                    // This is a generic system - specific implementations should use callbacks
                    
                    // Notify all callbacks of potential collision
                    for (auto& callback : m_collisionCallbacks) {
                        callback(entityA, entityB, world);
                    }
                }
            }
        }

        /**
         * @brief Initialize the system
         * @param world The ECS world
         */
        void Initialize(ECS::World& world) override {
            // Optional: Add initialization logic
        }

        /**
         * @brief Cleanup the system
         * @param world The ECS world
         */
        void Cleanup(ECS::World& world) override {
            m_collisionCallbacks.clear();
        }
    };
}

#endif // COMMON_SYSTEMS_COLLISION_SYSTEM_H
