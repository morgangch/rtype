/**
 * @file System.h
 * @brief Base system class for the Entity Component System (ECS)
 * @author R-Type Team
 * @date 2025
 *
 * This file contains the base System class that all ECS systems should inherit from.
 * Systems contain the logic that operates on entities with specific component combinations.
 */

#ifndef ECS_SYSTEM_HPP
#define ECS_SYSTEM_HPP

#include "Types.h"
#include <string>

namespace ECS {
    // Forward declaration
    class World;

    /**
     * @brief Base class for all ECS systems
     *
     * Systems contain the game logic that operates on entities with specific
     * component combinations. Each system should inherit from this base class
     * and implement the Update method to define its behavior.
     *
     * Systems can be enabled/disabled to control when they execute, and have
     * priorities to determine execution order.
     */
    class System {
    private:
        /**
         * @brief Whether this system is currently enabled
         */
        bool m_enabled = true;

        /**
         * @brief Execution priority of this system (lower = earlier execution)
         */
        int m_priority = 0;

        /**
         * @brief Unique name identifier for this system
         */
        std::string m_name;

    public:
        /**
         * @brief Construct a new System
         *
         * @param name Unique name for this system
         * @param priority Execution priority (lower values execute first)
         */
        explicit System(const std::string& name, int priority = 0)
            : m_name(name), m_priority(priority) {}

        /**
         * @brief Virtual destructor
         */
        virtual ~System() = default;

        /**
         * @brief Update the system logic
         *
         * This method is called every frame when the system is enabled.
         * Implement your system's logic here.
         *
         * @param world Reference to the ECS world
         * @param deltaTime Time elapsed since last update in seconds
         */
        virtual void Update(World& world, float deltaTime) = 0;

        /**
         * @brief Called when the system is initialized
         *
         * Override this method to perform initialization logic.
         * Called once when the system is registered to the world.
         *
         * @param world Reference to the ECS world
         */
        virtual void Initialize(World& world) {}

        /**
         * @brief Called when the system is destroyed
         *
         * Override this method to perform cleanup logic.
         * Called once when the system is removed from the world.
         *
         * @param world Reference to the ECS world
         */
        virtual void Cleanup(World& world) {}

        /**
         * @brief Enable the system
         *
         * Enabled systems have their Update method called each frame.
         */
        void Enable() { m_enabled = true; }

        /**
         * @brief Disable the system
         *
         * Disabled systems skip their Update method execution.
         */
        void Disable() { m_enabled = false; }

        /**
         * @brief Check if the system is enabled
         *
         * @return true if enabled, false otherwise
         */
        bool IsEnabled() const { return m_enabled; }

        /**
         * @brief Set the system's execution priority
         *
         * @param priority New priority (lower values execute first)
         */
        void SetPriority(int priority) { m_priority = priority; }

        /**
         * @brief Get the system's execution priority
         *
         * @return Current priority value
         */
        int GetPriority() const { return m_priority; }

        /**
         * @brief Get the system's name
         *
         * @return System name
         */
        const std::string& GetName() const { return m_name; }
    };
}

#endif // ECS_SYSTEM_HPP
