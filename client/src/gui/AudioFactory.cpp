// AudioFactory.cpp
#include "gui/AudioFactory.h"

namespace rtype::client::gui {

std::string AudioFactory::getMusicPath(MusicId id) {
    switch (id) {
        case MusicId::Menu:        return "assets/audio/music/menu.mp3";
        case MusicId::Level:       return "assets/audio/music/level.mp3";
        case MusicId::GameOver:    return "assets/audio/music/gameover.mp3";
        case MusicId::BossFight:   return "assets/audio/enemy/music/bossfight.mp3";
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
