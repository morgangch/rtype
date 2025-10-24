/**
 * @file CleanupSystem.h
 * @brief System for removing off-screen entities
 * 
 * This system removes entities that have moved outside the game bounds.
 * Useful for cleaning up projectiles and enemies.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_SYSTEMS_CLEANUP_SYSTEM_H
#define COMMON_SYSTEMS_CLEANUP_SYSTEM_H

#include <ECS/ECS.h>
#include <common/components/Position.h>

#include <vector>

namespace rtype::common::systems {
    /**
     * @class CleanupSystem
     * @brief Removes entities that are outside the game bounds
     * 
     * This system checks entity positions and destroys those
     * that have moved outside the configured boundaries.
     */
    class CleanupSystem : public ECS::System {
    private:
        float m_minX, m_maxX;  ///< Horizontal bounds
        float m_minY, m_maxY;  ///< Vertical bounds
        float m_margin;        ///< Extra margin before cleanup

    public:
        /**
         * @brief Constructor
         * @param minX Minimum X boundary
         * @param maxX Maximum X boundary
         * @param minY Minimum Y boundary
         * @param maxY Maximum Y boundary
         * @param margin Extra margin before cleanup (default: 100)
         */
        CleanupSystem(float minX, float maxX, float minY, float maxY, float margin = 100.0f)
            : ECS::System("CleanupSystem", 100),
              m_minX(minX), m_maxX(maxX),
              m_minY(minY), m_maxY(maxY),
              m_margin(margin) {}

        /**
         * @brief Update and clean up off-screen entities
         * @param world The ECS world
         * @param deltaTime Time elapsed since last update
         */
        void Update(ECS::World& world, float deltaTime) override {
            auto* positions = world.GetAllComponents<components::Position>();
            
            if (!positions)
                return;

            std::vector<ECS::EntityID> toDestroy;

            // Find entities outside bounds
            for (auto& [entity, posPtr] : *positions) {
                const auto& pos = *posPtr;
                
                if (pos.x < m_minX - m_margin || pos.x > m_maxX + m_margin ||
                    pos.y < m_minY - m_margin || pos.y > m_maxY + m_margin) {
                    toDestroy.push_back(entity);
                }
            }

            // Destroy out-of-bounds entities
            for (auto entity : toDestroy) {
                world.DestroyEntity(entity);
            }
        }

        /**
         * @brief Set the cleanup boundaries
         * @param minX Minimum X
         * @param maxX Maximum X
         * @param minY Minimum Y
         * @param maxY Maximum Y
         */
        void SetBounds(float minX, float maxX, float minY, float maxY) {
            m_minX = minX;
            m_maxX = maxX;
            m_minY = minY;
            m_maxY = maxY;
        }

        /**
         * @brief Set the cleanup margin
         * @param margin New margin value
         */
        void SetMargin(float margin) {
            m_margin = margin;
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
            // Optional: Add cleanup logic
        }
    };
}

#endif // COMMON_SYSTEMS_CLEANUP_SYSTEM_H
