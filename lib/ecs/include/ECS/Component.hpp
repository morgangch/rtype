#ifndef ECS_COMPONENT_HPP
#define ECS_COMPONENT_HPP

#include "Types.hpp"

namespace ECS {
    class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual ComponentTypeID GetTypeID() const = 0;
    };

    class ComponentTypeRegistry {
    private:
        static ComponentTypeID GetNextTypeID() {
            static ComponentTypeID nextID = 1;
            return nextID++;
        }

    public:
        template<typename T>
        static ComponentTypeID GetTypeID() {
            static ComponentTypeID typeID = GetNextTypeID();
            return typeID;
        }
    };

    template<typename T>
    class Component : public IComponent {
    public:
        ComponentTypeID GetTypeID() const override {
            return GetStaticTypeID();
        }

        static ComponentTypeID GetStaticTypeID() {
            return ComponentTypeRegistry::GetTypeID<T>();
        }
    };
}

#endif // ECS_COMPONENT_HPP
