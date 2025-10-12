/**
 * @file Animation.h
 * @brief Animation component for sprite frame management
 * 
 * This component handles sprite animation by cycling through frames
 * based on time or state changes.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_COMPONENTS_ANIMATION_H
#define CLIENT_COMPONENTS_ANIMATION_H

#include <ECS/ECS.h>

namespace rtype::client::components {
    /**
     * @class Animation
     * @brief Component for managing sprite frame animations
     * 
     * Example usage:
     * @code
     * // Create animation with 5 frames, 0.1s per frame
     * world.AddComponent<Animation>(entity, 5, 0.1f);
     * @endcode
     */
    class Animation : public ECS::Component<Animation> {
    public:
        /** @brief Total number of frames in the animation */
        int frameCount;
        
        /** @brief Current frame index (0 to frameCount-1) */
        int currentFrame;
        
        /** @brief Duration of each frame in seconds */
        float frameDuration;
        
        /** @brief Time elapsed since last frame change */
        float frameTimer;
        
        /** @brief Whether animation is currently playing */
        bool isPlaying;
        
        /** @brief Whether animation should loop */
        bool loop;
        
        /** @brief Width of a single frame in pixels */
        int frameWidth;
        
        /** @brief Height of a single frame in pixels */
        int frameHeight;
        
        /** @brief Animation direction: 1 = forward (1->5), -1 = reverse (5->1) */
        int direction;
        
        /**
         * @brief Constructor
         * @param frameCount Total number of frames
         * @param frameDuration Duration of each frame in seconds
         * @param frameWidth Width of each frame
         * @param frameHeight Height of each frame
         * @param loop Whether to loop the animation
         */
        Animation(int frameCount = 1, 
                  float frameDuration = 0.1f,
                  int frameWidth = 33,
                  int frameHeight = 17,
                  bool loop = true)
            : frameCount(frameCount),
              currentFrame(0),
              frameDuration(frameDuration),
              frameTimer(0.0f),
              isPlaying(false),
              loop(loop),
              frameWidth(frameWidth),
              frameHeight(frameHeight),
              direction(1) {}
        
        /**
         * @brief Start playing the animation
         */
        void play() {
            isPlaying = true;
            currentFrame = 0;
            frameTimer = 0.0f;
        }
        
        /**
         * @brief Stop the animation and reset to first frame
         */
        void stop() {
            isPlaying = false;
            currentFrame = 0;
            frameTimer = 0.0f;
        }
        
        /**
         * @brief Pause the animation at current frame
         */
        void pause() {
            isPlaying = false;
        }
    };
}

#endif // CLIENT_COMPONENTS_ANIMATION_H
