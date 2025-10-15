/**
 * @file SystemManager.cpp
 * @brief Implementation of the SystemManager class
 * @author R-Type Team
 * @date 2025
 */

#include "ECS/SystemManager.h"
#include "ECS/World.h"
#include <stdexcept>

namespace ECS {
    void SystemManager::SortSystems() {
        if (!m_needsSort) return;

        std::sort(m_systems.begin(), m_systems.end(),
            [](const std::unique_ptr<System>& a, const std::unique_ptr<System>& b) {
                return a->GetPriority() < b->GetPriority();
            });

        m_needsSort = false;
    }

    bool SystemManager::RemoveSystem(World& world, const std::string& systemName) {
        auto it = m_systemMap.find(systemName);
        if (it == m_systemMap.end()) {
            return false;
        }

        System* systemPtr = it->second;

        // Call cleanup before removal
        systemPtr->Cleanup(world);

        // Remove from map
        m_systemMap.erase(it);

        // Remove from vector
        auto systemIt = std::find_if(m_systems.begin(), m_systems.end(),
            [systemPtr](const std::unique_ptr<System>& system) {
                return system.get() == systemPtr;
            });

        if (systemIt != m_systems.end()) {
            m_systems.erase(systemIt);
            return true;
        }

        return false;
    }

    bool SystemManager::RemoveSystem(World& world, System* system) {
        if (!system) return false;
        return RemoveSystem(world, system->GetName());
    }

    System* SystemManager::GetSystem(const std::string& systemName) {
        auto it = m_systemMap.find(systemName);
        return (it != m_systemMap.end()) ? it->second : nullptr;
    }

    bool SystemManager::EnableSystem(const std::string& systemName) {
        System* system = GetSystem(systemName);
        if (system) {
            system->Enable();
            return true;
        }
        return false;
    }

    bool SystemManager::DisableSystem(const std::string& systemName) {
        System* system = GetSystem(systemName);
        if (system) {
            system->Disable();
            return true;
        }
        return false;
    }

    bool SystemManager::IsSystemEnabled(const std::string& systemName) const {
        auto it = m_systemMap.find(systemName);
        return (it != m_systemMap.end()) ? it->second->IsEnabled() : false;
    }

    void SystemManager::UpdateSystems(World& world, float deltaTime) {
        SortSystems();

        for (auto& system : m_systems) {
            if (system->IsEnabled()) {
                system->Update(world, deltaTime);
            }
        }
    }

    bool SystemManager::SetSystemPriority(const std::string& systemName, int priority) {
        System* system = GetSystem(systemName);
        if (system) {
            system->SetPriority(priority);
            m_needsSort = true;
            return true;
        }
        return false;
    }

    std::vector<std::string> SystemManager::GetSystemNames() const {
        std::vector<std::string> names;
        names.reserve(m_systems.size());

        for (const auto& system : m_systems) {
            names.push_back(system->GetName());
        }

        return names;
    }

    void SystemManager::Clear(World& world) {
        // Call cleanup on all systems before clearing
        for (auto& system : m_systems) {
            system->Cleanup(world);
        }

        m_systems.clear();
        m_systemMap.clear();
        m_needsSort = false;
    }
}
