/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/
#ifndef TOOLS_H
#define TOOLS_H
#include <string>
#include <cstdint>

namespace rtype::tools {
    /**
     * Convert a uint32_t IP address to a string representation.
     * @param ip The IP address in uint32_t format.
     * @return The string representation of the IP address.
     */
    std::string ipToString(uint32_t ip);
    std::string ipToString(uint8_t *ip);

    /**
     * Get the current timestamp in milliseconds.
     * @return The current timestamp.
     */
    unsigned long getCurrentTimestamp();

}

#endif //TOOLS_H
