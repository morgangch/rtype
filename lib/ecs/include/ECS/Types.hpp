/**
 * @file Types.hpp
 * @brief Core type definitions for the Entity Component System (ECS)
 * @author R-Type Team
 * @date 2025
 *
 * This file contains fundamental type definitions used throughout the ECS library.
 * It defines entity identifiers, component type identifiers, and related constants.
 */

#ifndef ECS_TYPES_HPP
#define ECS_TYPES_HPP

#include <cstdint>

namespace ECS {
    /**
     * @brief Type alias for entity identifiers
     *
     * EntityID is used to uniquely identify entities within the ECS world.
     * Uses a 32-bit unsigned integer to support up to ~4 billion entities.
     */
    using EntityID = std::uint32_t;

    /**
     * @brief Type alias for component type identifiers
     *
     * ComponentTypeID is used to uniquely identify different component types.
     * Each component class gets assigned a unique type ID for fast lookups.
     */
    using ComponentTypeID = std::uint32_t;

    /**
     * @brief Constant representing an invalid/null entity
     *
     * This constant is used to represent an invalid entity ID.
     * Entity ID 0 is reserved and should never be assigned to a valid entity.
     */
    constexpr EntityID INVALID_ENTITY = 0;
}

#endif // ECS_TYPES_HPP
