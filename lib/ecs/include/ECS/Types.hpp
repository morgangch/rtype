#pragma once

#include <cstdint>

namespace ECS {
    using EntityID = std::uint32_t;
    using ComponentTypeID = std::uint32_t;

    constexpr EntityID INVALID_ENTITY = 0;
}
