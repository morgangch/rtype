#ifndef COMMON_UTILS_RANDOM_HPP
#define COMMON_UTILS_RANDOM_HPP
#include <random>

inline int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

inline float randomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}
#endif // COMMON_UTILS_RANDOM_HPP
