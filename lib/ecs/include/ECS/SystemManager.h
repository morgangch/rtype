/**
 * @file SystemManager.h
 * @brief System management for the Entity Component System (ECS)
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the SystemManager class which handles registration,
 * execution, and lifecycle management of systems within the ECS world.
 */

#ifndef ECS_SYSTEMMANAGER_HPP
#define ECS_SYSTEMMANAGER_HPP

#include "System.h"
#include "Types.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

namespace ECS {
    // Forward declaration
    class World;

    /**
     * @brief Manages system registration, execution, and lifecycle
     *
     * The SystemManager is responsible for:
     * - Registering and storing systems
     * - Managing system execution order based on priority
     * - Enabling/disabling systems
     * - Removing systems from the world
     * - Calling system update methods each frame
     */
    class SystemManager {
    private:
        /**
         * @brief Container for storing registered systems
         */
        std::vector<std::unique_ptr<System>> m_systems;

        /**
         * @brief Map for quick system lookup by name
         */
        std::unordered_map<std::string, System*> m_systemMap;

        /**
         * @brief Flag indicating if systems need to be resorted by priority
         */
        bool m_needsSort = false;

        /**
         * @brief Sort systems by priority (lower priority executes first)
         */
        void SortSystems();

    public:
        /**
         * @brief Default constructor
         */
        SystemManager() = default;

        /**
         * @brief Default destructor
         */
        ~SystemManager() = default;

        /**
         * @brief Register a new system to the manager
         *
         * Creates and registers a system of type T with the provided arguments.
         * The system will be initialized and added to the execution queue.
         *
         * @tparam T The system type to register (must inherit from System)
         * @tparam Args Constructor argument types for the system
         * @param world Reference to the ECS world
         * @param args Arguments to forward to the system constructor
         * @return T* Pointer to the registered system
         */
        template<typename T, typename... Args>
        T* RegisterSystem(World& world, Args&&... args) {
            static_assert(std::is_base_of_v<System, T>, "T must inherit from System");

            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T* systemPtr = system.get();

            // Check if a system with this name already exists
            const std::string& name = system->GetName();
            if (m_systemMap.find(name) != m_systemMap.end()) {
                throw std::runtime_error("System with name '" + name + "' already exists");
            }

            // Initialize the system
            system->Initialize(world);

            // Add to containers
            m_systemMap[name] = systemPtr;
            m_systems.push_back(std::move(system));
            m_needsSort = true;

            return systemPtr;
        }

        /**
         * @brief Remove a system by name
         *
         * Removes and destroys the system with the specified name.
         * The system's Cleanup method will be called before removal.
         *
         * @param world Reference to the ECS world
         * @param systemName Name of the system to remove
         * @return true if system was found and removed, false otherwise
         */
        bool RemoveSystem(World& world, const std::string& systemName);

        /**
         * @brief Remove a system by pointer
         *
         * Removes and destroys the specified system.
         * The system's Cleanup method will be called before removal.
         *
         * @param world Reference to the ECS world
         * @param system Pointer to the system to remove
         * @return true if system was found and removed, false otherwise
         */
        bool RemoveSystem(World& world, System* system);

        /**
         * @brief Get a system by name
         *
         * @param systemName Name of the system to retrieve
         * @return System* Pointer to the system, or nullptr if not found
         */
        System* GetSystem(const std::string& systemName);

        /**
         * @brief Get a system by type
         *
         * @tparam T The system type to retrieve
         * @return T* Pointer to the system, or nullptr if not found
         */
        template<typename T>
        T* GetSystem() {
            for (auto& system : m_systems) {
                if (auto* castedSystem = dynamic_cast<T*>(system.get())) {
                    return castedSystem;
                }
            }
            return nullptr;
        }

        /**
         * @brief Enable a system by name
         *
         * @param systemName Name of the system to enable
         * @return true if system was found and enabled, false otherwise
         */
        bool EnableSystem(const std::string& systemName);

        /**
         * @brief Disable a system by name
         *
         * @param systemName Name of the system to disable
         * @return true if system was found and disabled, false otherwise
         */
        bool DisableSystem(const std::string& systemName);

        /**
         * @brief Check if a system is enabled
         *
         * @param systemName Name of the system to check
         * @return true if system exists and is enabled, false otherwise
         */
        bool IsSystemEnabled(const std::string& systemName) const;

        /**
         * @brief Update all enabled systems
         *
         * Calls the Update method on all enabled systems in priority order.
         * Systems with lower priority values are updated first.
         *
         * @param world Reference to the ECS world
         * @param deltaTime Time elapsed since last update in seconds
         */
        void UpdateSystems(World& world, float deltaTime);

        /**
         * @brief Set the priority of a system
         *
         * Changes the execution priority of the specified system.
         * Lower priority values execute first.
         *
         * @param systemName Name of the system
         * @param priority New priority value
         * @return true if system was found and priority was set, false otherwise
         */
        bool SetSystemPriority(const std::string& systemName, int priority);

        /**
         * @brief Get the number of registered systems
         *
         * @return size_t Number of systems currently registered
         */
        size_t GetSystemCount() const { return m_systems.size(); }

        /**
         * @brief Get names of all registered systems
         *
         * @return std::vector<std::string> Vector containing all system names
         */
        std::vector<std::string> GetSystemNames() const;

        /**
         * @brief Clear all systems
         *
         * Removes and destroys all registered systems.
         * Each system's Cleanup method will be called before removal.
         *
         * @param world Reference to the ECS world
         */
        void Clear(World& world);
    };
}

#endif // ECS_SYSTEMMANAGER_HPP
