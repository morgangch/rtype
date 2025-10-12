/**
 * @file MusicManager.cpp
 * @brief Music management system for the game
 * 
 * This file implements the MusicManager class which provides a simple
 * wrapper around SFML's sf::Music to handle loading, playback, volume,
 * and mute functionality. It maintains internal state to allow easy
 * control of background music in the game.
 * 
 * Features:
 * - Load music from file
 * - Play/stop music with optional looping
 * - Set and get volume (0 to 100)
 * - Mute and unmute music, remembering volume level
 * - Check if music is loaded and currently playing
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */
#include "gui/MusicManager.h"
#include <iostream>

namespace rtype::client::gui {

bool MusicManager::loadFromFile(const std::string& path) {
    m_loaded = m_music.openFromFile(path);
    if (!m_loaded) {
        std::cerr << "MusicManager: failed to load music: " << path << std::endl;
    } else {
        m_music.setVolume(m_volume);
    }
    return m_loaded;
}

void MusicManager::play(bool loop) {
    if (!m_loaded)
        return;

    m_music.setLoop(loop);
    m_music.play();

    if (m_muted)
        m_music.pause();
}

void MusicManager::stop() {
    if (!m_loaded) return;
    m_music.stop();
}

void MusicManager::setVolume(float volume) {
    m_volume = volume;
    if (!m_muted && m_loaded) {
        m_music.setVolume(m_volume);
    }
}

float MusicManager::getVolume() const {
    return m_volume;
}

void MusicManager::setMuted(bool muted) {
    static bool wasPlayingBeforeMute = false;

    if (m_muted == muted)
        return;

    if (!m_loaded) {
        m_muted = muted;
        return;
    }

    if (muted) {
        wasPlayingBeforeMute = m_music.getStatus() == sf::Music::Playing;
        m_music.pause();
        m_muted = true;
    } else {
        m_music.setVolume(m_volume);
        if (wasPlayingBeforeMute) {
            m_music.play();
        }
        m_muted = false;
    }
}

bool MusicManager::isMuted() const { return m_muted; }

bool MusicManager::isLoaded() const { return m_loaded; }

bool MusicManager::isPlaying() const {
    if (!m_loaded) return false;
    return m_music.getStatus() == sf::Music::Playing;
}

} // namespace rtype::client::gui
