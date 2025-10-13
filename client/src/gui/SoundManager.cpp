// SoundManager.cpp
#include "gui/SoundManager.h"
#include <iostream>

namespace rtype::client::gui {

bool SoundManager::loadAll() {
    bool ok = true;
    // Required sounds
    ok &= load(AudioFactory::SfxId::LoseLife);

    // Optional sounds - load best-effort
    load(AudioFactory::SfxId::Shoot);
    load(AudioFactory::SfxId::ChargedShoot);
    load(AudioFactory::SfxId::EnemyDeath);
    load(AudioFactory::SfxId::BossDeath);

    return ok;
}

bool SoundManager::load(AudioFactory::SfxId id) {
    Id key = toId(id);

    if (m_buffers.find(key) != m_buffers.end())
        return true;

    std::string path = AudioFactory::getSfxPath(id);
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path)) {
        std::cerr << "SoundManager: could not load sound: " << path << std::endl;
        return false;
    }

    // Store the buffer first to ensure it remains alive for sf::Sound
    m_buffers.emplace(key, std::move(buffer));
    // Get reference to the stored buffer
    sf::SoundBuffer& storedBuffer = m_buffers[key];

    // Create sound and bind it to the stored buffer
    sf::Sound sound;
    sound.setBuffer(storedBuffer);

    m_sounds.emplace(key, std::move(sound));
    m_volumes.emplace(key, 100.0f);
    return true;
}

bool SoundManager::has(AudioFactory::SfxId id) const {
    return m_buffers.find(toId(id)) != m_buffers.end();
}

void SoundManager::play(AudioFactory::SfxId id) {
    Id key = toId(id);
    auto it = m_sounds.find(key);
    if (it == m_sounds.end()) return;
    auto volIt = m_volumes.find(key);
    if (volIt != m_volumes.end()) it->second.setVolume(volIt->second);
    it->second.play();
}

void SoundManager::setVolume(AudioFactory::SfxId id, float volume) {
    Id key = toId(id);
    m_volumes[key] = volume;
    auto it = m_sounds.find(key);
    if (it != m_sounds.end()) it->second.setVolume(volume);
}

} // namespace rtype::client::gui
