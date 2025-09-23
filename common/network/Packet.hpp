#pragma once
#include <cstdint>
#include "MessageType.hpp"

struct PacketHeader {
    std::uint16_t size;
    MessageType type;
    std::uint32_t entityId;
};
