#ifndef ECS_TYPES_HPP
#define ECS_TYPES_HPP

#include <cstdint>

namespace ECS {
    using EntityID = std::uint32_t;
    using ComponentTypeID = std::uint32_t;

    constexpr EntityID INVALID_ENTITY = 0;
}

#endif // ECS_TYPES_HPP
