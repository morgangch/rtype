/**
 * @file ChainLightning.h
 * @brief Component marking a projectile to apply chained damage to nearby enemies
 */

#ifndef COMMON_COMPONENTS_CHAIN_LIGHTNING_H
#define COMMON_COMPONENTS_CHAIN_LIGHTNING_H

#include <ECS/ECS.h>
#include <array>

namespace rtype::common::components {

    class ChainLightning : public ECS::Component<ChainLightning> {
    public:
        int maxChains;              // number of extra arcs after first hit
        float range;                // search radius for next enemies
        std::array<int, 3> stageDamage; // damage for stages [first, second, third]

        explicit ChainLightning(int chains = 2, float r = 350.f)
            : maxChains(chains), range(r), stageDamage{2, 1, 1} {}
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_CHAIN_LIGHTNING_H
