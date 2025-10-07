#include "ECS/ComponentManager.h"
#include <algorithm>

namespace ECS {
    void ComponentManager::RemoveAllComponents(EntityID entity) {
        auto it = m_entityComponents.find(entity);
        if (it != m_entityComponents.end()) {
            for (ComponentTypeID typeID : it->second) {
                auto arrayIt = m_componentArrays.find(typeID);
                if (arrayIt != m_componentArrays.end()) {
                    arrayIt->second->RemoveComponent(entity);
                }
            }
            m_entityComponents.erase(it);
        }
    }

    void ComponentManager::Clear() {
        for (auto& pair : m_componentArrays) {
            pair.second->Clear();
        }
        m_entityComponents.clear();
    }
}
