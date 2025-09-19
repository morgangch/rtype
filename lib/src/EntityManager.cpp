#include "ECS/EntityManager.hpp"
#include <algorithm>

namespace ECS {
    EntityManager::EntityManager() : m_nextEntityID(1) {
        // Reserve space for better performance
        m_aliveEntities.reserve(1000);
    }

    EntityID EntityManager::CreateEntity() {
        EntityID entityID;

        if (!m_freeEntities.empty()) {
            entityID = m_freeEntities.front();
            m_freeEntities.pop();
        } else {
            entityID = m_nextEntityID++;
        }

        // Ensure the alive entities vector is large enough
        if (entityID >= m_aliveEntities.size()) {
            m_aliveEntities.resize(entityID + 1, false);
        }

        m_aliveEntities[entityID] = true;
        return entityID;
    }

    void EntityManager::DestroyEntity(EntityID entity) {
        if (entity != INVALID_ENTITY && entity < m_aliveEntities.size() && m_aliveEntities[entity]) {
            m_aliveEntities[entity] = false;
            m_freeEntities.push(entity);
        }
    }

    bool EntityManager::IsEntityAlive(EntityID entity) const {
        return entity != INVALID_ENTITY &&
               entity < m_aliveEntities.size() &&
               m_aliveEntities[entity];
    }

    void EntityManager::Clear() {
        m_aliveEntities.clear();
        std::queue<EntityID> empty;
        m_freeEntities.swap(empty);
        m_nextEntityID = 1;
    }

    size_t EntityManager::GetAliveEntityCount() const {
        return std::count(m_aliveEntities.begin(), m_aliveEntities.end(), true);
    }
}
