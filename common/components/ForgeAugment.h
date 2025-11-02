/**
 * @file ForgeAugment.h
 * @brief Component for player forge augments unlocked after boss kills
 */

#ifndef COMMON_COMPONENTS_FORGE_AUGMENT_H
#define COMMON_COMPONENTS_FORGE_AUGMENT_H

#include <ECS/ECS.h>

namespace rtype::common::components {

    enum class ForgeAugmentType {
        None = 0,
        DualLaser,      // Crimson Striker -> Dual laser (1.5 dmg per beam -> 2)
        BouncySplit,    // Azure Phantom -> 2 angled bouncing shots (1 dmg)
        ShortSpread,    // Shotgun: 5 pellets cone, short-lived (0.5 -> 1)
        GuardianTriBeam // Solar Guardian -> triple forward beams (original augment)
    };

    class ForgeAugment : public ECS::Component<ForgeAugment> {
    public:
        ForgeAugmentType type;
        bool unlocked;

        explicit ForgeAugment(ForgeAugmentType t = ForgeAugmentType::None, bool u = false)
            : type(t), unlocked(u) {}
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_FORGE_AUGMENT_H
