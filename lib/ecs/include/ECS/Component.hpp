/**
 * @file Component.hpp
 * @brief Base component classes and type registration system for the ECS
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the foundational component classes and type registration
 * system used throughout the ECS. It provides automatic type ID generation
 * for components and establishes the component inheritance hierarchy.
 */

#ifndef ECS_COMPONENT_HPP
#define ECS_COMPONENT_HPP

#include "Types.hpp"

namespace ECS {
    /**
     * @brief Abstract interface for all components
     *
     * IComponent provides the base interface that all components must implement.
     * It ensures that every component can provide its type ID for runtime
     * type identification and component system operations.
     */
    class IComponent {
    public:
        /**
         * @brief Virtual destructor for proper polymorphic cleanup
         */
        virtual ~IComponent() = default;

        /**
         * @brief Gets the runtime type ID of this component
         *
         * @return ComponentTypeID The unique type identifier for this component
         */
        virtual ComponentTypeID GetTypeID() const = 0;
    };

    /**
     * @brief Registry system for automatic component type ID generation
     *
     * ComponentTypeRegistry provides a centralized system for assigning
     * unique type IDs to component types. It uses template specialization
     * to ensure each component type gets exactly one unique ID that persists
     * throughout the program's lifetime.
     */
    class ComponentTypeRegistry {
    private:
        /**
         * @brief Generates the next available component type ID
         *
         * Uses a static counter to ensure unique IDs are generated.
         * Thread-safe within single-threaded applications.
         *
         * @return ComponentTypeID The next available unique type ID
         */
        static ComponentTypeID GetNextTypeID() {
            static ComponentTypeID nextID = 1;
            return nextID++;
        }

    public:
        /**
         * @brief Gets or creates a unique type ID for component type T
         *
         * Each component type gets exactly one type ID that is consistent
         * across all instances and calls. The ID is generated on first access
         * and cached for subsequent calls.
         *
         * @tparam T The component type to get an ID for
         * @return ComponentTypeID The unique type ID for component type T
         */
        template<typename T>
        static ComponentTypeID GetTypeID() {
            static ComponentTypeID typeID = GetNextTypeID();
            return typeID;
        }
    };

    /**
     * @brief Template base class for all concrete components
     *
     * Component<T> provides the standard implementation of IComponent
     * for concrete component types. It automatically handles type ID
     * generation and provides both instance and static access to the type ID.
     *
     * All game components should inherit from Component<ComponentType>
     * using the Curiously Recurring Template Pattern (CRTP).
     *
     * @tparam T The derived component type (CRTP parameter)
     *
     * @example
     * class PositionComponent : public Component<PositionComponent> {
     *     float x, y;
     * public:
     *     PositionComponent(float x, float y) : x(x), y(y) {}
     * };
     */
    template<typename T>
    class Component : public IComponent {
    public:
        /**
         * @brief Gets the type ID of this component instance
         *
         * @return ComponentTypeID The unique type ID for this component type
         */
        ComponentTypeID GetTypeID() const override {
            return GetStaticTypeID();
        }

        /**
         * @brief Gets the static type ID for component type T
         *
         * Provides static access to the component's type ID without
         * requiring an instance. Useful for component manager operations.
         *
         * @return ComponentTypeID The unique type ID for component type T
         */
        static ComponentTypeID GetStaticTypeID() {
            return ComponentTypeRegistry::GetTypeID<T>();
        }
    };
}

#endif // ECS_COMPONENT_HPP
