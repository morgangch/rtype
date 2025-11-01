/**
 * @file AssetPaths.h
 * @brief Centralized asset path constants for maintainability
 * 
 * This file contains all asset paths used in the game.
 * Centralizing paths here makes it easier to:
 * - Update asset locations
 * - Handle missing files
 * - Swap assets for different themes
 * - Maintain consistency across the codebase
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_ASSET_PATHS_H
#define CLIENT_ASSET_PATHS_H

#include <string>

namespace rtype::client::assets {

    /**
     * @brief Base directory for all sprite assets
     */
    constexpr const char* SPRITES_DIR = "assets/sprites/";

    /**
     * @brief Player vessel sprite paths
     */
    namespace player {
        constexpr const char* PLAYER_SPRITE = "assets/sprites/PlayerVessel/PLAYER.gif";  // Crimson Striker (default)
        constexpr const char* PLAYER_AZURE = "assets/sprites/PlayerVessel/PLAYER_AZURE.gif";  // Azure Phantom (speed)
        constexpr const char* PLAYER_EMERALD = "assets/sprites/PlayerVessel/PLAYER_EMERALD.gif";  // Emerald Titan (power)
        constexpr const char* PLAYER_SOLAR = "assets/sprites/PlayerVessel/PLAYER_SOLAR.gif";  // Solar Guardian (defense)
    }

    /**
     * @brief Enemy sprite paths
     */
    namespace enemies {
        // Basic enemies
        constexpr const char* BASIC_ENEMY_1 = "assets/sprites/BasicEnemy/BASIC_ENEMY_1.gif";  // Basic
        constexpr const char* BASIC_ENEMY_2 = "assets/sprites/BasicEnemy/BASIC_ENEMY_2.gif";  // Snake, Suicide
        constexpr const char* BASIC_ENEMY_3 = "assets/sprites/BasicEnemy/BASIC_ENEMY_1.gif";  // Pata (reuse BASIC_1)

        // Advanced enemies
        constexpr const char* ADVANCED_ENEMY_1 = "assets/sprites/BasicEnemy/ADVANCED_ENEMY_1.gif";  // Shielded, Waver
        constexpr const char* ADVANCED_ENEMY_2 = "assets/sprites/BasicEnemy/BASIC_ENEMY_2.gif";     // Flanker (reuse BASIC_2)
        constexpr const char* ADVANCED_ENEMY_3 = "assets/sprites/BasicEnemy/ADVANCED_ENEMY_1.gif";  // Bomber (reuse ADVANCED_1)

        // Boss enemies
        constexpr const char* BOSS_ENEMY_1 = "assets/sprites/BasicEnemy/BOSS_ENEMY_1.gif";  // TankDestroyer
        constexpr const char* BOSS_ENEMY_2 = "assets/sprites/BasicEnemy/BOSS_ENEMY_1.gif";  // Serpent (reuse BOSS_1)
        constexpr const char* BOSS_ENEMY_3 = "assets/sprites/BasicEnemy/BOSS_ENEMY_1.gif";  // Fortress (reuse BOSS_1)
        constexpr const char* BOSS_ENEMY_4 = "assets/sprites/BasicEnemy/BOSS_ENEMY_1.gif";  // Core (reuse BOSS_1)
    }

    /**
     * @brief Projectile sprite paths
     */
    namespace projectiles {
        constexpr const char* PROJECTILE_1 = "assets/sprites/Projectile/PROJECTILE_1.gif";
        constexpr const char* PROJECTILE_2 = "assets/sprites/Projectile/PROJECTILE_2.gif";
        constexpr const char* PROJECTILE_3 = "assets/sprites/Projectile/PROJECTILE_3.gif";
        constexpr const char* PROJECTILE_4 = "assets/sprites/Projectile/PROJECTILE_4.gif";
    }

    /**
     * @brief Heart/life icon sprite paths
     */
    namespace hearts {
        constexpr const char* HEART_SPRITE = "assets/sprites/Heart/Heart_red.png";
    }

    /**
     * @brief Font paths
     */
    namespace fonts {
        constexpr const char* RTYPE_FONT = "assets/fonts/r-type.otf";
    }

    /**
     * @brief Map/level paths
     */
    namespace maps {
        constexpr const char* DEFAULT_MAP = "assets/maps/default.def";
    }

    /**
     * @brief UI icon paths
     */
    namespace ui {
        constexpr const char* SETTINGS_GEAR = "assets/sprites/Settings/MetalGear.png";
        constexpr const char* BUTTON = "assets/sprites/Buttons/Button.png";
        constexpr const char* RETURN_BUTTON = "assets/sprites/Buttons/Return.png";
        constexpr const char* READY_BUTTON = "assets/sprites/Buttons/Ready.png";
    }

} // namespace rtype::client::assets

#endif // CLIENT_ASSET_PATHS_H
