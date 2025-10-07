/**
 * @file ComponentManager.h
 * @brief Component management system for the ECS architecture
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the ComponentManager class and related component storage classes
 * that handle component lifecycle, storage, and retrieval within the ECS world.
 * It provides type-safe component management with efficient storage and lookup.
 */

#ifndef ECS_COMPONENTMANAGER_HPP
#define ECS_COMPONENTMANAGER_HPP

#include "Types.h"
#include "Component.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeinfo>
#include <algorithm>

namespace ECS {
    /**
     * @brief Abstract base class for component storage arrays
     *
     * Provides a common interface for component arrays of different types,
     * enabling polymorphic operations on component storage.
     */
    class IComponentArray {
    public:
        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~IComponentArray() = default;

        /**
         * @brief Removes a component from the specified entity
         *
         * @param entity The EntityID from which to remove the component
         */
        virtual void RemoveComponent(EntityID entity) = 0;

        /**
         * @brief Clears all components from this array
         */
        virtual void Clear() = 0;
    };

    /**
     * @brief Template class for storing components of a specific type
     *
     * ComponentArray manages components of type T for all entities.
     * It provides efficient storage using unique_ptr for memory management
     * and unordered_map for fast entity-to-component lookups.
     *
     * @tparam T The component type to store
     */
    template<typename T>
    class ComponentArray : public IComponentArray {
    private:
        /**
         * @brief Storage for components mapped by entity ID
         *
         * Uses unique_ptr for automatic memory management and to support
         * polymorphic component types.
         */
        std::unordered_map<EntityID, std::unique_ptr<T>> m_components;

    public:
        /**
         * @brief Adds a component to the specified entity
         *
         * @param entity The EntityID to attach the component to
         * @param component Unique pointer to the component instance
         */
        void AddComponent(EntityID entity, std::unique_ptr<T> component) {
            m_components[entity] = std::move(component);
        }

        /**
         * @brief Retrieves a component for the specified entity
         *
         * @param entity The EntityID to get the component from
         * @return T* Pointer to the component, or nullptr if not found
         */
        T* GetComponent(EntityID entity) {
            auto it = m_components.find(entity);
            return (it != m_components.end()) ? it->second.get() : nullptr;
        }

        /**
         * @brief Removes a component from the specified entity
         *
         * @param entity The EntityID from which to remove the component
         */
        void RemoveComponent(EntityID entity) override {
            m_components.erase(entity);
        }

        /**
         * @brief Checks if an entity has this component type
         *
         * @param entity The EntityID to check
         * @return true if the entity has this component, false otherwise
         */
        bool HasComponent(EntityID entity) const {
            return m_components.find(entity) != m_components.end();
        }

        /**
         * @brief Clears all components from this array
         */
        void Clear() override {
            m_components.clear();
        }

        /**
         * @brief Returns iterator to the beginning of components
         * @return Iterator to the first component
         */
        auto begin() { return m_components.begin(); }

        /**
         * @brief Returns iterator to the end of components
         * @return Iterator past the last component
         */
        auto end() { return m_components.end(); }

        /**
         * @brief Returns const iterator to the beginning of components
         * @return Const iterator to the first component
         */
        auto begin() const { return m_components.begin(); }

        /**
         * @brief Returns const iterator to the end of components
         * @return Const iterator past the last component
         */
        auto end() const { return m_components.end(); }
    };

    /**
     * @brief Manages all component types and their associations with entities
     *
     * The ComponentManager is the central hub for component operations in the ECS.
     * It handles:
     * - Creating and storing components of any type
     * - Retrieving components by entity and type
     * - Removing components from entities
     * - Tracking which components belong to which entities
     * - Providing efficient access to component collections
     */
    class ComponentManager {
    private:
        /**
         * @brief Storage for all component arrays, indexed by component type ID
         */
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentArray>> m_componentArrays;

        /**
         * @brief Mapping of entities to their component type IDs
         *
         * Used for efficient cleanup and querying of entity components.
         */
        std::unordered_map<EntityID, std::vector<ComponentTypeID>> m_entityComponents;

        /**
         * @brief Gets or creates a component array for the specified type
         *
         * @tparam T The component type
         * @return ComponentArray<T>* Pointer to the component array for type T
         */
        template<typename T>
        ComponentArray<T>* GetComponentArray() {
            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            auto it = m_componentArrays.find(typeID);

            if (it == m_componentArrays.end()) {
                m_componentArrays[typeID] = std::make_unique<ComponentArray<T>>();
            }

            return static_cast<ComponentArray<T>*>(m_componentArrays[typeID].get());
        }

    public:
        /**
         * @brief Default constructor
         */
        ComponentManager() = default;

        /**
         * @brief Default destructor
         */
        ~ComponentManager() = default;

        /**
         * @brief Creates and adds a component to an entity
         *
         * Creates a new component of type T with the provided arguments
         * and attaches it to the specified entity.
         *
         * @tparam T The component type to create
         * @tparam Args The argument types for the component constructor
         * @param entity The EntityID to attach the component to
         * @param args Arguments to forward to the component constructor
         * @return T* Pointer to the newly created component
         */
        template<typename T, typename... Args>
        T* AddComponent(EntityID entity, Args&&... args) {
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* componentPtr = component.get();

            GetComponentArray<T>()->AddComponent(entity, std::move(component));

            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            m_entityComponents[entity].push_back(typeID);

            return componentPtr;
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
            return GetComponentArray<T>()->GetComponent(entity);
        }

        /**
         * @brief Removes a component from an entity
         *
         * @tparam T The component type to remove
         * @param entity The EntityID to remove the component from
         */
        template<typename T>
        void RemoveComponent(EntityID entity) {
            GetComponentArray<T>()->RemoveComponent(entity);

            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            auto& components = m_entityComponents[entity];
            components.erase(std::remove(components.begin(), components.end(), typeID), components.end());
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
            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            auto it = m_componentArrays.find(typeID);
            if (it == m_componentArrays.end()) {
                return false;
            }
            return static_cast<const ComponentArray<T>*>(it->second.get())->HasComponent(entity);
        }

        /**
         * @brief Removes all components from the specified entity
         *
         * @param entity The EntityID to remove all components from
         */
        void RemoveAllComponents(EntityID entity);

        /**
         * @brief Clears all components from all entities
         */
        void Clear();

        /**
         * @brief Gets the component array for a specific component type
         *
         * Provides direct access to the component array for iteration
         * and batch operations.
         *
         * @tparam T The component type
         * @return ComponentArray<T>* Pointer to the component array
         */
        template<typename T>
        ComponentArray<T>* GetAllComponents() {
            return GetComponentArray<T>();
        }
    };
}

#endif // ECS_COMPONENTMANAGER_HPP
