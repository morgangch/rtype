#pragma once
#include <ECS/ECS.hpp>
#include <string>

namespace Client {
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
