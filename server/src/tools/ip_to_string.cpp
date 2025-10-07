/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include "tools.h"
#include <string>
#include <cstdint>

std::string rtype::tools::ipToString(uint32_t ip) {
    std::string ip_str = "";
    ip_str += std::to_string(static_cast<uint8_t>(ip & 0xFF)) + ".";
    ip_str += std::to_string(static_cast<uint8_t>((ip >> 8) & 0xFF)) + ".";
    ip_str += std::to_string(static_cast<uint8_t>((ip >> 16) & 0xFF)) + ".";
    ip_str += std::to_string(static_cast<uint8_t>((ip >> 24) & 0xFF));
    return ip_str;
}

std::string rtype::tools::ipToString(uint8_t *ip) {
    std::string ip_str = "";
    ip_str += std::to_string(ip[0]) + ".";
    ip_str += std::to_string(ip[1]) + ".";
    ip_str += std::to_string(ip[2]) + ".";
    ip_str += std::to_string(ip[3]);
    return ip_str;
}