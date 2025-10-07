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
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void RemoveComponent(EntityID entity) = 0;
        virtual void Clear() = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray {
    private:
        std::unordered_map<EntityID, std::unique_ptr<T>> m_components;

    public:
        void AddComponent(EntityID entity, std::unique_ptr<T> component) {
            m_components[entity] = std::move(component);
        }

        T* GetComponent(EntityID entity) {
            auto it = m_components.find(entity);
            return (it != m_components.end()) ? it->second.get() : nullptr;
        }

        void RemoveComponent(EntityID entity) override {
            m_components.erase(entity);
        }

        bool HasComponent(EntityID entity) const {
            return m_components.find(entity) != m_components.end();
        }

        void Clear() override {
            m_components.clear();
        }

        auto begin() { return m_components.begin(); }
        auto end() { return m_components.end(); }
        auto begin() const { return m_components.begin(); }
        auto end() const { return m_components.end(); }
    };

    class ComponentManager {
    private:
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentArray>> m_componentArrays;
        std::unordered_map<EntityID, std::vector<ComponentTypeID>> m_entityComponents;

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
        ComponentManager() = default;
        ~ComponentManager() = default;

        template<typename T, typename... Args>
        T* AddComponent(EntityID entity, Args&&... args) {
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* componentPtr = component.get();

            GetComponentArray<T>()->AddComponent(entity, std::move(component));

            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            m_entityComponents[entity].push_back(typeID);

            return componentPtr;
        }

        template<typename T>
        T* GetComponent(EntityID entity) {
            return GetComponentArray<T>()->GetComponent(entity);
        }

        template<typename T>
        void RemoveComponent(EntityID entity) {
            GetComponentArray<T>()->RemoveComponent(entity);

            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            auto& components = m_entityComponents[entity];
            components.erase(std::remove(components.begin(), components.end(), typeID), components.end());
        }

        template<typename T>
        bool HasComponent(EntityID entity) const {
            ComponentTypeID typeID = Component<T>::GetStaticTypeID();
            auto it = m_componentArrays.find(typeID);
            if (it == m_componentArrays.end()) {
                return false;
            }
            return static_cast<const ComponentArray<T>*>(it->second.get())->HasComponent(entity);
        }

        void RemoveAllComponents(EntityID entity);
        void Clear();

        template<typename T>
        ComponentArray<T>* GetAllComponents() {
            return GetComponentArray<T>();
        }
    };
}

#endif // ECS_COMPONENTMANAGER_HPP
