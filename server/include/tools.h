/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Utility functions for IP conversion and timestamp generation
*/
#ifndef TOOLS_H
#define TOOLS_H
#include <string>
#include <cstdint>

namespace rtype::tools {
    /**
     * @brief Convert a uint32_t IP address to dotted-decimal string format
     * 
     * Converts a 32-bit integer IP address (network byte order) to
     * human-readable string format (e.g., "192.168.1.1").
     * 
     * @param ip The IP address in uint32_t format (network byte order)
     * @return String representation in "XXX.XXX.XXX.XXX" format
     */
    std::string ipToString(uint32_t ip);
    
    /**
     * @brief Convert a byte array IP address to dotted-decimal string format
     * 
     * Converts a 4-byte array representing an IP address to
     * human-readable string format.
     * 
     * @param ip Pointer to 4-byte array [byte0, byte1, byte2, byte3]
     * @return String representation in "XXX.XXX.XXX.XXX" format
     */
    std::string ipToString(uint8_t *ip);

    /**
     * @brief Get the current system timestamp in milliseconds
     * 
     * Returns the current time since Unix epoch (January 1, 1970)
     * in milliseconds. Useful for timeout detection and latency tracking.
     * 
     * @return Current timestamp in milliseconds
     */
    unsigned long getCurrentTimestamp();

}

#endif //TOOLS_H
