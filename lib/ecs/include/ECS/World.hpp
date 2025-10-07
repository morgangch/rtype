/**
 * @file World.hpp
 * @brief Central ECS world manager that coordinates entities and components
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the World class, which serves as the main interface for
 * ECS operations. It coordinates between EntityManager and ComponentManager
 * to provide a unified API for entity and component management.
 */

#ifndef ECS_WORLD_HPP
#define ECS_WORLD_HPP

#include "Types.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"

namespace ECS {
    /**
     * @brief Central manager for the ECS world
     *
     * The World class is the primary interface for ECS operations. It coordinates
     * between EntityManager and ComponentManager to provide a unified, easy-to-use
     * API for creating entities, adding components, and managing the ECS state.
     *
     * The World acts as a facade that simplifies ECS usage by hiding the complexity
     * of managing separate entity and component systems.
     */
    class World {
    private:
        /**
         * @brief Manages entity creation, destruction, and lifecycle
         */
        EntityManager m_entityManager;

        /**
         * @brief Manages component storage and retrieval
         */
        ComponentManager m_componentManager;

    public:
        /**
         * @brief Default constructor
         *
         * Initializes a new ECS world with empty entity and component managers.
         */
        World() = default;

        /**
         * @brief Default destructor
         */
        ~World() = default;

        /**
         * @brief Creates a new entity in the world
         *
         * @return EntityID The unique identifier for the newly created entity
         */
        EntityID CreateEntity();

        /**
         * @brief Destroys an entity and all its components
         *
         * Removes the entity from the world and cleans up all associated components.
         *
         * @param entity The EntityID of the entity to destroy
         */
        void DestroyEntity(EntityID entity);

        /**
         * @brief Checks if an entity is currently alive in the world
         *
         * @param entity The EntityID to check
         * @return true if the entity exists and is alive, false otherwise
         */
        bool IsEntityAlive(EntityID entity) const;

        /**
         * @brief Adds a component to an entity
         *
         * Creates a new component of type T with the provided arguments
         * and attaches it to the specified entity.
         *
         * @tparam T The component type to add
         * @tparam Args The argument types for the component constructor
         * @param entity The EntityID to add the component to
         * @param args Arguments to forward to the component constructor
         * @return T* Pointer to the newly created component
         */
        template<typename T, typename... Args>
        T* AddComponent(EntityID entity, Args&&... args) {
            return m_componentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
        }

        /**
         * @brief Retrieves a component from an entity
         *
         * @tparam T The component type to retrieve
         * @param entity The EntityID to get the component from
         * @return T* Pointer to the component, or nullptr if not found
         */
        template<typename T>
        T* GetComponent(EntityID entity) {
            return m_componentManager.GetComponent<T>(entity);
        }

        /**
         * @brief Removes a component from an entity
         *
         * @tparam T The component type to remove
         * @param entity The EntityID to remove the component from
         */
        template<typename T>
        void RemoveComponent(EntityID entity) {
            m_componentManager.RemoveComponent<T>(entity);
        }

        /**
         * @brief Checks if an entity has a specific component type
         *
         * @tparam T The component type to check for
         * @param entity The EntityID to check
         * @return true if the entity has the component, false otherwise
         */
        template<typename T>
        bool HasComponent(EntityID entity) const {
            return m_componentManager.HasComponent<T>(entity);
        }

        /**
         * @brief Gets all components of a specific type
         *
         * Provides access to the component array for iteration over all
         * components of type T in the world.
         *
         * @tparam T The component type to get
         * @return ComponentArray<T>* Pointer to the component array
         */
        template<typename T>
        ComponentArray<T>* GetAllComponents() {
            return m_componentManager.GetAllComponents<T>();
        }

        /**
         * @brief Clears all entities and components from the world
         *
         * Resets the world to its initial empty state.
         */
        void Clear();

        /**
         * @brief Gets the number of currently alive entities
         *
         * @return size_t The count of entities that are currently active
         */
        size_t GetAliveEntityCount() const;
    };
}

#endif // ECS_WORLD_HPP
