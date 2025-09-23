#pragma once
#include <chrono>

class Timer {
public:
    Timer() { reset(); }
    void reset() { start = std::chrono::high_resolution_clock::now(); }
    float elapsed() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float>(now - start).count();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
