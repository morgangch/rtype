#ifndef CLIENT_ANIMATION_HPP
#define CLIENT_ANIMATION_HPP

#include <ECS/ECS.hpp>
#include <vector>

namespace rtype::client::components {
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

#endif // CLIENT_ANIMATION_HPP
