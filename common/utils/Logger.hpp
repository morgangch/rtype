#pragma once
#include <iostream>

enum class LogLevel { INFO, WARN, ERROR };

inline void log(LogLevel lvl, const std::string& msg) {
    switch (lvl) {
        case LogLevel::INFO: std::cout << "[INFO] " << msg << "\n"; break;
        case LogLevel::WARN: std::cout << "[WARN] " << msg << "\n"; break;
        case LogLevel::ERROR: std::cerr << "[ERROR] " << msg << "\n"; break;
    }
}
