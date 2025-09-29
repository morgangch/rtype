#ifndef COMMON_NETWORK_PACKET_HPP
#define COMMON_NETWORK_PACKET_HPP
#include <cstdint>
#include "MessageType.hpp"

struct PacketHeader {
    std::uint16_t size;
    MessageType type;
    std::uint32_t entityId;
};
#endif // COMMON_NETWORK_PACKET_HPP
