/**
 * @file EntityManager.hpp
 * @brief Entity management system for the ECS architecture
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the EntityManager class which handles creation, destruction,
 * and lifecycle management of entities within the ECS world. It provides efficient
 * entity ID allocation and deallocation with reuse of freed IDs.
 */

#ifndef ECS_ENTITYMANAGER_HPP
#define ECS_ENTITYMANAGER_HPP

#include "Types.h"
#include <vector>
#include <queue>

namespace ECS {
    /**
     * @brief Manages entity creation, destruction, and lifecycle
     *
     * The EntityManager is responsible for:
     * - Creating new entities with unique IDs
     * - Destroying entities and marking them as inactive
     * - Recycling entity IDs for efficient memory usage
     * - Tracking which entities are alive/active
     *
     * Entity IDs are reused when entities are destroyed to prevent
     * ID exhaustion and maintain performance.
     */
    class EntityManager {
    private:
        /**
         * @brief Tracks which entities are currently alive
         *
         * Uses a vector<bool> for memory-efficient storage of entity states.
         * Index corresponds to EntityID, value indicates if entity is alive.
         */
        std::vector<bool> m_aliveEntities;

        /**
         * @brief Queue of entity IDs available for reuse
         *
         * When entities are destroyed, their IDs are added to this queue
         * for reuse when creating new entities.
         */
        std::queue<EntityID> m_freeEntities;

        /**
         * @brief Next available entity ID for new entities
         *
         * Used when no free entity IDs are available for reuse.
         * Incremented each time a new ID is needed.
         */
        EntityID m_nextEntityID;

    public:
        /**
         * @brief Constructs a new EntityManager
         *
         * Initializes the entity manager with default values.
         * Sets the next entity ID to 1 (since 0 is reserved for INVALID_ENTITY).
         */
        EntityManager();

        /**
         * @brief Default destructor
         */
        ~EntityManager() = default;

        /**
         * @brief Creates a new entity and returns its ID
         *
         * Creates a new entity by either:
         * 1. Reusing a previously freed entity ID, or
         * 2. Assigning the next available ID
         *
         * @return EntityID The unique identifier for the newly created entity
         */
        EntityID CreateEntity();

        /**
         * @brief Destroys an entity and marks it as inactive
         *
         * Marks the entity as destroyed and adds its ID to the free queue
         * for potential reuse. The entity's components should be cleaned up
         * separately by the ComponentManager.
         *
         * @param entity The EntityID of the entity to destroy
         */
        void DestroyEntity(EntityID entity);

        /**
         * @brief Checks if an entity is currently alive/active
         *
         * @param entity The EntityID to check
         * @return true if the entity exists and is alive, false otherwise
         */
        bool IsEntityAlive(EntityID entity) const;

        /**
         * @brief Clears all entities and resets the manager
         *
         * Destroys all entities, clears internal data structures,
         * and resets the manager to its initial state.
         */
        void Clear();

        /**
         * @brief Gets the number of currently alive entities
         *
         * @return size_t The count of entities that are currently active
         */
        size_t GetAliveEntityCount() const;

        /**
         * @brief Get a vector of all currently alive entities.
         * @return Vector of EntityID representing all alive entities.
         */
        std::vector<std::uint32_t> GetAllEntities() const;
    };
}

#endif // ECS_ENTITYMANAGER_HPP
