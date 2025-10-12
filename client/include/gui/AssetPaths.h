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
        constexpr const char* PLAYER_SPRITE = "assets/sprites/PlayerVessel/PLAYER.gif";
    }

    /**
     * @brief Enemy sprite paths
     */
    namespace enemies {
        constexpr const char* BASIC_ENEMY_1 = "assets/sprites/BasicEnemy/BASICENEMY_1.gif";
        constexpr const char* BASIC_ENEMY_2 = "assets/sprites/BasicEnemy/BASICENEMY_2.gif";
        constexpr const char* BASIC_ENEMY_3 = "assets/sprites/BasicEnemy/BASICENEMY_3.gif";
        constexpr const char* BASIC_ENEMY_4 = "assets/sprites/BasicEnemy/BASICENEMY_4.gif";
    }

    /**
     * @brief Projectile sprite paths (currently unused - using colored rectangles)
     */
    namespace projectiles {
        // Reserved for future projectile sprites
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

} // namespace rtype::client::assets

#endif // CLIENT_ASSET_PATHS_H
