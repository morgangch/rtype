/**
 * @file VesselClass.h
 * @brief Component for player vessel class definition
 * 
 * This component defines the different playable vessel classes with their unique
 * characteristics, weapon types, and stats modifiers.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef COMMON_COMPONENTS_VESSEL_CLASS_H
#define COMMON_COMPONENTS_VESSEL_CLASS_H

#include <ECS/ECS.h>
#include <string>
#include <cmath>
#include <algorithm>

namespace rtype::common::components {

    /**
     * @enum VesselType
     * @brief Available vessel classes
     */
    enum class VesselType {
        CrimsonStriker,  // Balanced - medium speed, solid damage
        AzurePhantom,    // Speed - agile, high fire rate
        EmeraldTitan,    // Power - heavy hitting, slow
        SolarGuardian    // Defense - shields, support
    };

    /**
     * @enum WeaponMode
     * @brief Weapon firing modes
     */
    enum class WeaponMode {
        Single,      // Single projectile
        Dual,        // Dual projectiles
        Burst,       // Multiple projectiles in burst
        Spread,      // Shotgun-like spread
        Piercing     // Piercing beam
    };

    /**
     * @struct VesselStats
     * @brief Stats modifiers for vessel classes
     */
    struct VesselStats {
        float speedMultiplier;      // Speed modifier (1.0 = 100%)
        float damageMultiplier;     // Damage modifier (1.0 = 100%)
        float defenseMultiplier;    // Defense modifier (1.0 = 100%)
        float fireRateMultiplier;   // Fire rate modifier (1.0 = 100%)
        
        VesselStats(float speed = 1.0f, float damage = 1.0f, float defense = 1.0f, float fireRate = 1.0f)
            : speedMultiplier(speed), damageMultiplier(damage), 
              defenseMultiplier(defense), fireRateMultiplier(fireRate) {}
    };

    /**
     * @class VesselClass
     * @brief Component defining a player's vessel class and characteristics
     */
    class VesselClass : public ECS::Component<VesselClass> {
    public:
        /** @brief Type of vessel */
        VesselType type;
        
        /** @brief Display name of the vessel */
        std::string name;
        
        /** @brief Stats modifiers */
        VesselStats stats;
        
        /** @brief Normal shot weapon mode */
        WeaponMode normalWeaponMode;
        
        /** @brief Charged shot weapon mode */
        WeaponMode chargedWeaponMode;
        
        /** @brief Normal shot damage */
        int normalShotDamage;
        
        /** @brief Charged shot damage */
        int chargedShotDamage;
        
        /** @brief Whether charged shot pierces */
        bool chargedShotPiercing;
        
        /** @brief Number of projectiles for burst/spread shots */
        int projectileCount;
        
        /** @brief Charged shot charge time (seconds) */
        float chargeTime;

        /**
         * @brief Default constructor - creates Crimson Striker
         */
        VesselClass()
            : type(VesselType::CrimsonStriker),
              name("Crimson Striker"),
              stats(1.0f, 1.0f, 1.0f, 1.0f),
              normalWeaponMode(WeaponMode::Single),
              chargedWeaponMode(WeaponMode::Piercing),
              normalShotDamage(1),
              chargedShotDamage(2),
              chargedShotPiercing(true),
              projectileCount(1),
              chargeTime(1.5f) {}

        /**
         * @brief Constructor with vessel type
         * @param vesselType The type of vessel to create
         */
        explicit VesselClass(VesselType vesselType) : VesselClass() {
            initializeVessel(vesselType);
        }

        /**
         * @brief Initialize vessel with predefined characteristics
         * @param vesselType The type of vessel
         */
        void initializeVessel(VesselType vesselType) {
            type = vesselType;
            
            switch (vesselType) {
                case VesselType::CrimsonStriker:
                    // Balanced Class
                    name = "Crimson Striker";
                    stats = VesselStats(1.0f, 1.0f, 1.0f, 1.0f);
                    normalWeaponMode = WeaponMode::Single;
                    chargedWeaponMode = WeaponMode::Piercing;
                    normalShotDamage = 1;
                    chargedShotDamage = 2;
                    chargedShotPiercing = true;
                    projectileCount = 1;
                    chargeTime = 1.5f;
                    break;

                case VesselType::AzurePhantom:
                    // Speed Class
                    name = "Azure Phantom";
                    stats = VesselStats(1.2f, 0.8f, 0.9f, 1.5f); // 120% speed, 80% damage, 90% defense, 150% fire rate
                    normalWeaponMode = WeaponMode::Dual;
                    chargedWeaponMode = WeaponMode::Burst;
                    normalShotDamage = 1; // 0.5 per projectile, but dual
                    chargedShotDamage = 1; // 1 per dart, 3 darts
                    chargedShotPiercing = true;
                    projectileCount = 3; // 3 homing darts
                    chargeTime = 1.0f;
                    break;

                case VesselType::EmeraldTitan:
                    // Power Class
                    name = "Emerald Titan";
                    stats = VesselStats(0.8f, 1.5f, 1.1f, 0.7f); // 80% speed, 150% damage, 110% defense, 70% fire rate
                    normalWeaponMode = WeaponMode::Single; // Explosive AoE
                    chargedWeaponMode = WeaponMode::Single; // Large plasma bomb
                    normalShotDamage = 2;
                    chargedShotDamage = 4;
                    chargedShotPiercing = true;
                    projectileCount = 1;
                    chargeTime = 2.5f;
                    break;

                case VesselType::SolarGuardian:
                    // Defense Class
                    name = "Solar Guardian";
                    stats = VesselStats(0.9f, 0.9f, 1.5f, 1.0f); // 90% speed, 90% damage, 150% defense
                    normalWeaponMode = WeaponMode::Spread; // Shotgun
                    chargedWeaponMode = WeaponMode::Single; // Shield (special)
                    normalShotDamage = 1; // 0.5 per pellet, 4 pellets
                    chargedShotDamage = 0; // Shield doesn't damage
                    chargedShotPiercing = true;
                    projectileCount = 4; // 4 shotgun pellets
                    chargeTime = 2.0f;
                    break;
            }
        }

        /**
         * @brief Get the sprite sheet row for this vessel
         * @return Y coordinate offset in the sprite sheet (row * 17 pixels)
         * 
         * PLAYER.gif is a sprite sheet with dimensions 166x86 (5 frames × 5 rows)
         * Each frame is 33x17 pixels. Vessels are on different rows:
         * - Row 0 (y=0): CrimsonStriker (default red)
         * - Row 1 (y=17): AzurePhantom (blue)
         * - Row 2 (y=34): EmeraldTitan (green)
         * - Row 3 (y=51): SolarGuardian (yellow/gold)
         */
        int getSpriteSheetRow() const {
            switch (type) {
                case VesselType::CrimsonStriker:
                    return 0;  // Row 0, y=0
                case VesselType::AzurePhantom:
                    return 17; // Row 1, y=17
                case VesselType::EmeraldTitan:
                    return 34; // Row 2, y=34
                case VesselType::SolarGuardian:
                    return 51; // Row 3, y=51
                default:
                    return 0;  // Default to CrimsonStriker
            }
        }

        /**
         * @brief Get the effective max speed for this vessel
         * @param baseSpeed Base speed value
         * @return Modified speed
         */
        float getEffectiveSpeed(float baseSpeed) const {
            return baseSpeed * stats.speedMultiplier;
        }

        /**
         * @brief Get the effective damage for this vessel
         * @param baseDamage Base damage value
         * @return Modified damage (minimum 1 if base > 0)
         */
        int getEffectiveDamage(int baseDamage) const {
            if (baseDamage == 0) return 0;
            int effectiveDamage = static_cast<int>(std::round(baseDamage * stats.damageMultiplier));
            return (std::max)(1, effectiveDamage); // Ensure minimum 1 damage
        }

        /**
         * @brief Get the effective fire cooldown for this vessel
         * @param baseCooldown Base cooldown value
         * @return Modified cooldown
         */
        float getEffectiveFireCooldown(float baseCooldown) const {
            return baseCooldown / stats.fireRateMultiplier;
        }

        /**
         * @brief Get the effective max health for this vessel
         * @param baseHealth Base health value
         * @return Modified health
         */
        int getEffectiveMaxHealth(int baseHealth) const {
            return static_cast<int>(baseHealth * stats.defenseMultiplier);
        }
    };

} // namespace rtype::common::components

#endif // COMMON_COMPONENTS_VESSEL_CLASS_H
