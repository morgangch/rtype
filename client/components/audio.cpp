#include "client/components/audio.hpp"

namespace rtype::client::components {
    AudioSource::AudioSource(const std::string& soundPath, AudioType type)
        : soundPath(soundPath), type(type), volume(1.0f), looping(false), playing(false), is3D(false) {}
}
