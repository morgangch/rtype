/**
 * @file World.h
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

#include "Types.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

namespace ECS {
    /**
     * @brief Central manager for the ECS world
     *
     * The World class is the primary interface for ECS operations. It coordinates
     * between EntityManager, ComponentManager, and SystemManager to provide a unified,
     * easy-to-use API for creating entities, adding components, managing systems,
     * and running the ECS world.
     *
     * The World acts as a facade that simplifies ECS usage by hiding the complexity
     * of managing separate entity, component, and system managers.
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

        /**
         * @brief Manages system registration, execution, and lifecycle
         */
        SystemManager m_systemManager;

    public:
        /**
         * @brief Default constructor
         *
         * Initializes a new ECS world with empty entity, component, and system managers.
         */
        World() = default;

        /**
         * @brief Default destructor
         */
        ~World() = default;

        // Entity Management

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
         * @brief Gets the number of currently alive entities
         *
         * @return size_t The count of entities that are currently active
         */
        size_t GetAliveEntityCount() const;

        /**
         * @brief Get a vector of all currently alive entities.
         * @return Vector of EntityID representing all alive entities.
         */
        std::vector<EntityID> GetAllEntities() const {
            return m_entityManager.GetAllEntities();
        }

        // Component Management

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

        // System Management

        /**
         * @brief Register a new system to the world
         *
         * Creates and registers a system of type T with the provided arguments.
         * The system will be initialized and added to the execution queue.
         *
         * @tparam T The system type to register (must inherit from System)
         * @tparam Args Constructor argument types for the system
         * @param args Arguments to forward to the system constructor
         * @return T* Pointer to the registered system
         */
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args) {
            return m_systemManager.RegisterSystem<T>(*this, std::forward<Args>(args)...);
        }

        /**
         * @brief Remove a system by name
         *
         * @param systemName Name of the system to remove
         * @return true if system was found and removed, false otherwise
         */
        bool RemoveSystem(const std::string& systemName) {
            return m_systemManager.RemoveSystem(*this, systemName);
        }

        /**
         * @brief Remove a system by pointer
         *
         * @param system Pointer to the system to remove
         * @return true if system was found and removed, false otherwise
         */
        bool RemoveSystem(System* system) {
            return m_systemManager.RemoveSystem(*this, system);
        }

        /**
         * @brief Get a system by name
         *
         * @param systemName Name of the system to retrieve
         * @return System* Pointer to the system, or nullptr if not found
         */
        System* GetSystem(const std::string& systemName) {
            return m_systemManager.GetSystem(systemName);
        }

        /**
         * @brief Get a system by type
         *
         * @tparam T The system type to retrieve
         * @return T* Pointer to the system, or nullptr if not found
         */
        template<typename T>
        T* GetSystem() {
            return m_systemManager.GetSystem<T>();
        }

        /**
         * @brief Enable a system by name
         *
         * @param systemName Name of the system to enable
         * @return true if system was found and enabled, false otherwise
         */
        bool EnableSystem(const std::string& systemName) {
            return m_systemManager.EnableSystem(systemName);
        }

        /**
         * @brief Disable a system by name
         *
         * @param systemName Name of the system to disable
         * @return true if system was found and disabled, false otherwise
         */
        bool DisableSystem(const std::string& systemName) {
            return m_systemManager.DisableSystem(systemName);
        }

        /**
         * @brief Check if a system is enabled
         *
         * @param systemName Name of the system to check
         * @return true if system exists and is enabled, false otherwise
         */
        bool IsSystemEnabled(const std::string& systemName) const {
            return m_systemManager.IsSystemEnabled(systemName);
        }

        /**
         * @brief Update all enabled systems
         *
         * Calls the Update method on all enabled systems in priority order.
         * This should be called once per frame in your game loop.
         *
         * @param deltaTime Time elapsed since last update in seconds
         */
        void UpdateSystems(float deltaTime) {
            m_systemManager.UpdateSystems(*this, deltaTime);
        }

        /**
         * @brief Set the priority of a system
         *
         * @param systemName Name of the system
         * @param priority New priority value (lower = earlier execution)
         * @return true if system was found and priority was set, false otherwise
         */
        bool SetSystemPriority(const std::string& systemName, int priority) {
            return m_systemManager.SetSystemPriority(systemName, priority);
        }

        /**
         * @brief Get the number of registered systems
         *
         * @return size_t Number of systems currently registered
         */
        size_t GetSystemCount() const {
            return m_systemManager.GetSystemCount();
        }

        /**
         * @brief Get names of all registered systems
         *
         * @return std::vector<std::string> Vector containing all system names
         */
        std::vector<std::string> GetSystemNames() const {
            return m_systemManager.GetSystemNames();
        }

        /**
         * @brief Clears all entities and components from the world
         *
         * Resets the world to its initial empty state.
         */
        void Clear();
    };
}

#endif // ECS_WORLD_HPP
