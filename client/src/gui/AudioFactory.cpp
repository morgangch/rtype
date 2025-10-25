// AudioFactory.cpp
#include "gui/AudioFactory.h"

namespace rtype::client::gui {

std::string AudioFactory::getMusicPath(MusicId id) {
    switch (id) {
        case MusicId::Menu:        return "assets/audio/music/menu.mp3";
        case MusicId::Level1:      return "assets/audio/music/level1.mp3";
        case MusicId::Level2:      return "assets/audio/music/level2.mp3";
        case MusicId::Level3:      return "assets/audio/music/level3.mp3";
        case MusicId::GameOver:    return "assets/audio/music/gameover.mp3";
        case MusicId::BossFight1:  return "assets/audio/enemy/music/bossfight_tankdestroyer.mp3";
        case MusicId::BossFight2:  return "assets/audio/enemy/music/bossfight_thevesselcore.mp3";
        case MusicId::BossFight3:  return "assets/audio/enemy/music/bossfight_theguardianofthecore.mp3";
        case MusicId::BossFight4:  return "assets/audio/enemy/music/bossfight_reactorovermind.mp3";
        default: return {};
    }
}

std::string AudioFactory::getSfxPath(SfxId id) {
    switch (id) {
        case SfxId::LoseLife:      return "assets/audio/player/loselife.mp3";
        case SfxId::Shoot:         return "assets/audio/particles/shoot.mp3";
        case SfxId::ChargedShoot:  return "assets/audio/particles/chargedshoot.mp3";
        case SfxId::EnemyDeath:    return "assets/audio/enemy/effect/regulardeath.mp3";
        case SfxId::BossDeath:     return "assets/audio/enemy/effect/bossdeath.mp3";
        default: return {};
    }
}

} // namespace rtype::client::gui
