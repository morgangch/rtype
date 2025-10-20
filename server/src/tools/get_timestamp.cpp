/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** TODO: add description
*/

#include <chrono>
#include "tools.h"

unsigned long rtype::tools::getCurrentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    return duration_cast<milliseconds>(duration).count();
}
