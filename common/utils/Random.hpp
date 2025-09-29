#ifndef COMMON_UTILS_RANDOM_HPP
#define COMMON_UTILS_RANDOM_HPP

#include <random>

namespace rtype::common::utils {
    inline std::mt19937& rng() {
        static std::mt19937 engine(std::random_device{}());
        return engine;
    }

    inline int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng());
    }

    inline float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng());
    }
}

#endif // COMMON_UTILS_RANDOM_HPP
