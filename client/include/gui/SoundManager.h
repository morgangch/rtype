// SoundManager.h
#ifndef CLIENT_INCLUDE_GUI_SOUNDMANAGER_H
#define CLIENT_INCLUDE_GUI_SOUNDMANAGER_H

#include "gui/AudioFactory.h"
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>

/**
 * @file SoundManager.h
 * @brief Small helper that owns and plays short sound effects (SFX).
 *
 * SoundManager centralizes loading, storing and playing of small
 * sound effects. It keeps ownership of sf::SoundBuffer instances so
 * that sf::Sound objects can safely reference them. Use this class
 * instead of having many independent sf::SoundBuffer/sf::Sound pairs
 * across the codebase.
 */

namespace rtype::client::gui {

class SoundManager {
public:
    SoundManager() = default;

    /**
     * @brief Load all known sound effects.
     *
     * Returns true if required sounds were loaded. Optional/secondary
     * sounds are loaded best-effort.
     */
    bool loadAll();

    /**
     * @brief Load a single sound effect identified by @p id.
     *
     * @param id Identifier from AudioFactory::SfxId
     * @return true on success, false otherwise
     */
    bool load(AudioFactory::SfxId id);

    /**
     * @brief Check whether the requested sound effect has been loaded.
     */
    bool has(AudioFactory::SfxId id) const;

    /**
     * @brief Play a sound effect. No-op if the sound isn't loaded.
     */
    void play(AudioFactory::SfxId id);

    /**
     * @brief Set playback volume for a specific effect (0..100).
     */
    void setVolume(AudioFactory::SfxId id, float volume);

private:
    using Id = int;
    static Id toId(AudioFactory::SfxId id) { return static_cast<Id>(id); }

    // Stored buffers — map from enum -> buffer. Buffers must outlive
    // sf::Sound instances that reference them.
    std::unordered_map<Id, sf::SoundBuffer> m_buffers;

    // Playable sound objects. We keep one sf::Sound per effect which
    // is sufficient for short effects. If you need overlapping
    // instances consider a small pool per effect.
    std::unordered_map<Id, sf::Sound> m_sounds;

    // Configured per-effect volumes.
    std::unordered_map<Id, float> m_volumes;
};

} // namespace rtype::client::gui

#endif // CLIENT_INCLUDE_GUI_SOUNDMANAGER_H
