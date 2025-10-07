#ifndef ECS_WORLD_HPP
#define ECS_WORLD_HPP

#include "Types.h"
#include "EntityManager.h"
#include "ComponentManager.h"

namespace ECS {
    class World {
    private:
        EntityManager m_entityManager;
        ComponentManager m_componentManager;

    public:
        World() = default;
        ~World() = default;

        // Entity operations
        EntityID CreateEntity();
        void DestroyEntity(EntityID entity);
        bool IsEntityAlive(EntityID entity) const;

        // Component operations
        template<typename T, typename... Args>
        T* AddComponent(EntityID entity, Args&&... args) {
            return m_componentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T* GetComponent(EntityID entity) {
            return m_componentManager.GetComponent<T>(entity);
        }

        template<typename T>
        void RemoveComponent(EntityID entity) {
            m_componentManager.RemoveComponent<T>(entity);
        }

        template<typename T>
        bool HasComponent(EntityID entity) const {
            return m_componentManager.HasComponent<T>(entity);
        }

        template<typename T>
        ComponentArray<T>* GetAllComponents() {
            return m_componentManager.GetAllComponents<T>();
        }

        // Utility
        void Clear();
        size_t GetAliveEntityCount() const;
    };
}

#endif // ECS_WORLD_HPP
