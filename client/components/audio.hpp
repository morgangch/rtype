#ifndef CLIENT_AUDIO_SOURCE_HPP
#define CLIENT_AUDIO_SOURCE_HPP

#include <ECS/ECS.hpp>
#include <string>

namespace rtype::client::components {
    enum class AudioType {
        SFX,
        Music,
        Voice
    };

    class AudioSource : public ECS::Component<AudioSource> {
    public:
        std::string soundPath;
        AudioType type;
        float volume;
        bool looping;
        bool playing;
        bool is3D;

        AudioSource(const std::string& soundPath = "", AudioType type = AudioType::SFX);
    };
}

#endif // CLIENT_AUDIO_SOURCE_HPP
