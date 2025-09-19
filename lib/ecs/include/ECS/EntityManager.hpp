#pragma once

#include "Types.hpp"
#include <vector>
#include <queue>

namespace ECS {
    class EntityManager {
    private:
        std::vector<bool> m_aliveEntities;
        std::queue<EntityID> m_freeEntities;
        EntityID m_nextEntityID;

    public:
        EntityManager();
        ~EntityManager() = default;

        EntityID CreateEntity();
        void DestroyEntity(EntityID entity);
        bool IsEntityAlive(EntityID entity) const;
        void Clear();

        size_t GetAliveEntityCount() const;
    };
}
