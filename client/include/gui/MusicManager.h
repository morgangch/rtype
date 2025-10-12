// MusicManager.h
#pragma once

#include <SFML/Audio.hpp>
#include <string>

namespace rtype::client::gui {

/**
 * @file MusicManager.h
 * @brief Lightweight wrapper around sf::Music for playing level background music.
 *
 * The MusicManager encapsulates loading, playback control, volume and mute
 * state for a single music stream (sf::Music). It's intentionally small and
 * focused so the GUI (for example a mute button) or game states can reuse
 * the same logic without duplicating behaviour.
 *
 * Usage example:
 * @code
 * MusicManager mm;
 * if (mm.loadFromFile("assets/audio/music/level.mp3")) {
 *     mm.setVolume(50.f);
 *     mm.play(true); // loop
 * }
 * // toggle mute from UI
 * mm.setMuted(!mm.isMuted());
 * @endcode
 *
 * Notes:
 * - sf::Music streams audio from disk and is non-copyable; therefore
 *   MusicManager is non-copyable as well.
 * - Linking: ensure your client target links to SFML Audio (sfml-audio).
 * - Thread-safety: SFML audio objects are not guaranteed to be thread-safe;
 *   call into this class from the main thread or guard access externally.
 */

/**
 * @brief Simple music manager wrapper around sf::Music.
 *
 * Responsibilities:
 *  - Load music from a file path (streamed from disk).
 *  - Play / stop and looping control.
 *  - Volume control in the range [0..100].
 *  - Mute semantics that remember the previous volume and pause/resume
 *    playback when muted/unmuted.
 *
 * The class intentionally keeps a minimal state model (loaded, volume,
 * muted) so UI components can query the current state and present accurate
 * controls to the user (e.g. mute button icon, volume slider).
 */
class MusicManager {
public:
    /**
     * @brief Default-construct a MusicManager.
     *
     * The constructor does not open audio resources. Call
     * loadFromFile() to initialize the internal sf::Music stream before
     * calling play().
     */
    MusicManager() = default;

    /**
     * @brief Destructor stops playback and releases resources.
     *
     * The underlying sf::Music will be cleaned up automatically; no explicit
     * shutdown is required by callers.
     */
    ~MusicManager() = default;

    /**
     * @brief Non-copyable: sf::Music holds native resources and is
     * non-copyable. Disable copy operations to avoid accidental resource
     * sharing.
     */
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;

    /**
     * @brief Load a music file for streaming playback.
     *
     * The path is passed to sf::Music::openFromFile. This method must be
     * called before attempting to play. Returns true on success.
     *
     * @param path Filesystem path to an audio file (ogg, wav, flac, mp3 if
     * supported by the SFML build).
     * @return true if the file was opened successfully.
     */
    bool loadFromFile(const std::string& path);

    /**
     * @brief Start playback.
     *
     * If loop is true the music will loop when reaching the end. If the
     * music is not loaded this call is a no-op.
     *
     * @param loop true to enable looping (default = true)
     */
    void play(bool loop = true);

    /**
     * @brief Stop playback and rewind to the beginning.
     *
     * This stops playback entirely. Use play() to start again from the
     * beginning. If the music is not loaded this call is a no-op.
     */
    void stop();

    /**
     * @brief Set the playback volume.
     *
     * Volume is expressed in percent [0..100] and immediately affects the
     * underlying sf::Music. When muted, the volume value is remembered but
     * the actual output remains silent until unmuted.
     *
     * @param volume Value in range [0..100]
     */
    void setVolume(float volume);
    /**
     * @brief Get the currently-configured volume.
     *
     * This returns the remembered volume even when muted.
     *
     * @return Current volume [0..100]
     */
    float getVolume() const;

    /**
     * @brief Mute or unmute playback.
     *
     * When muted the manager will pause playback (if playing) and preserve
     * the configured volume value. Unmuting restores the previous volume
     * and resumes playback only if it was playing before muting.
     *
     * @param muted true to mute, false to unmute
     */
    void setMuted(bool muted);
    /**
     * @brief Query whether playback is currently muted.
     *
     * @return true if muted, false otherwise
     */
    bool isMuted() const;

    /**
     * @brief Return whether a music file has been successfully loaded.
     *
     * @return true if loadFromFile succeeded
     */
    bool isLoaded() const;

    /**
     * @brief Query whether the music stream is currently playing.
     *
     * @return true if the underlying sf::Music status is Playing
     */
    bool isPlaying() const;

private:
    /**
     * @brief Underlying SFML music stream.
     *
     * sf::Music streams audio from disk and cannot be copied. It is used
     * to perform seek/play/stop operations on the loaded music file.
     */
    sf::Music m_music;

    /**
     * @brief Remembered volume in percent [0..100].
     *
     * This value is applied to the sf::Music when not muted. When muted
     * the value is preserved so it can be restored when unmuting.
     */
    float m_volume{100.0f};

    /**
     * @brief Mute flag.
     *
     * When true playback is paused (or output silenced) and volume is
     * preserved in m_volume.
     */
    bool m_muted{false};

    /**
     * @brief Whether a music file has been successfully loaded into m_music.
     */
    bool m_loaded{false};

    /**
     * @brief Remember whether this music was playing before muting.
     *
     * Stored per-instance to avoid static/global state when multiple
     * MusicManager objects are used concurrently.
     */
    bool m_wasPlayingBeforeMute{false};
};

} // namespace rtype::client::gui
