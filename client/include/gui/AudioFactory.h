// AudioFactory.h
#ifndef CLIENT_INCLUDE_GUI_AUDIOFACTORY_H
#define CLIENT_INCLUDE_GUI_AUDIOFACTORY_H

#include <string>

/**
 * @file AudioFactory.h
 * @brief Centralized location for audio asset identifiers and paths.
 *
 * AudioFactory provides a small, centralized mapping between semantic
 * identifiers (MusicId / SfxId) and the filesystem paths used by the
 * client to load music and sound effects. Callers should use the
 * factory methods instead of hardcoding "assets/... .mp3" paths in many
 * places. This makes it easier to rename, replace or reformat audio
 * assets (for example switching from .mp3 to .ogg) in a single place.
 *
 * Usage example:
 * @code
 * const std::string menuPath = AudioFactory::getMusicPath(AudioFactory::MusicId::Menu);
 * mm.loadFromFile(menuPath);
 *
 * if (soundManager.has(AudioFactory::SfxId::Shoot)) {
 *     soundManager.play(AudioFactory::SfxId::Shoot);
 * }
 * @endcode
 *
 * Notes:
 * - Keep this header lightweight (only enums and path getters).
 * - If you need runtime configurability, consider loading paths from
 *   a JSON/TOML file and adding a small loader here.
 */

namespace rtype::client::gui {

class AudioFactory {
public:
    /**
     * @brief Identifiers for background music tracks used by the client.
     *
     * Use these semantic identifiers when requesting music paths so the
     * concrete filenames are centralized in the factory implementation.
     */
    enum class MusicId {
        Menu,      ///< Main menu background music
        Level,     ///< Standard level background music (looping)
        GameOver,  ///< Non-looping game over music
        BossFight  ///< Boss encounter music
    };

    /**
     * @brief Identifiers for short sound effects (SFX).
     *
     * Prefer calling these by semantic name instead of hardcoding asset
     * paths throughout the codebase.
     */
    enum class SfxId {
        LoseLife,     ///< Player hurt / lose life sound
        Shoot,        ///< Regular player shot
        ChargedShoot, ///< Charged/charged-up shot sound
        EnemyDeath,   ///< Regular enemy death sound
        BossDeath     ///< Boss death sound
    };

    /**
     * @brief Return the filesystem path for a music identifier.
     *
     * The returned path is suitable to pass to `MusicManager::loadFromFile`
     * (or directly to SFML) and is relative to the project root. The
     * factory currently returns .mp3 files; change the implementation if
     * you want to move to .ogg or configurable assets.
     *
     * @param id MusicId enum value
     * @return std::string Filesystem path to the music file
     */
    static std::string getMusicPath(MusicId id);

    /**
     * @brief Return the filesystem path for a short sound effect.
     *
     * The returned path can be passed to `sf::SoundBuffer::loadFromFile`.
     * Prefer using the `SoundManager` helper which loads and stores buffers
     * centrally and prevents lifetime issues.
     *
     * @param id SfxId enum value
     * @return std::string Filesystem path to the sound effect file
     */
    static std::string getSfxPath(SfxId id);
};

} // namespace rtype::client::gui

#endif // CLIENT_INCLUDE_GUI_AUDIOFACTORY_H
