/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef ENDIANE_CONVERTER_H
#define ENDIANE_CONVERTER_H

#include <cstdint>
#include <cstring>
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif
// For htons, htonl

/**
 * @brief Convert a value to network endian (big-endian) in place.
 * @tparam T Type of the value to convert
 * @param value Reference to the value to convert
 */
template<typename T>
inline void to_network_endian(T& value) {
    if constexpr (sizeof(T) == 2) value = htons(value);
    else if constexpr (sizeof(T) == 4) value = htonl(value);
    else if constexpr (sizeof(T) == 8) {
        uint64_t tmp;
        std::memcpy(&tmp, &value, 8);
        tmp = ((tmp & 0x00000000000000FFULL) << 56)
            | ((tmp & 0x000000000000FF00ULL) << 40)
            | ((tmp & 0x0000000000FF0000ULL) << 24)
            | ((tmp & 0x00000000FF000000ULL) << 8)
            | ((tmp & 0x000000FF00000000ULL) >> 8)
            | ((tmp & 0x0000FF0000000000ULL) >> 24)
            | ((tmp & 0x00FF000000000000ULL) >> 40)
            | ((tmp & 0xFF00000000000000ULL) >> 56);
        std::memcpy(&value, &tmp, 8);
    }
}

/**
 * @brief Convert a value from network endian (big-endian) to host endian in place.
 * @tparam T Type of the value to convert
 * @param value Reference to the value to convert
 */
template<typename T>
inline void from_network_endian(T& value) {
    to_network_endian(value); // même opération (involutive)
}

#endif //ENDIANE_CONVERTER_H
