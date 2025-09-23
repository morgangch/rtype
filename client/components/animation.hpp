#pragma once
#include <ECS/ECS.hpp>
#include <vector>

namespace Client {
    struct AnimationFrame {
        int textureX, textureY;
        int width, height;
        float duration;
    };

    class Animation : public ECS::Component<Animation> {
    public:
        std::vector<AnimationFrame> frames;
        int currentFrame;
        float frameTimer;
        bool looping;
        bool playing;

        Animation();
    };
}
