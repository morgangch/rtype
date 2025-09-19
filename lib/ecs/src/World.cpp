#include "ECS/World.hpp"

namespace ECS {
    EntityID World::CreateEntity() {
        return m_entityManager.CreateEntity();
    }

    void World::DestroyEntity(EntityID entity) {
        if (IsEntityAlive(entity)) {
            m_componentManager.RemoveAllComponents(entity);
            m_entityManager.DestroyEntity(entity);
        }
    }

    bool World::IsEntityAlive(EntityID entity) const {
        return m_entityManager.IsEntityAlive(entity);
    }

    void World::Clear() {
        m_componentManager.Clear();
        m_entityManager.Clear();
    }

    size_t World::GetAliveEntityCount() const {
        return m_entityManager.GetAliveEntityCount();
    }
}
