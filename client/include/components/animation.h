/**
 * @file animation.hpp
 * @brief Animation component for sprite animation in the R-TYPE client
 * 
 * This file defines the Animation component which handles frame-by-frame
 * sprite animations. It supports looping animations, custom frame durations,
 * and playback control.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_ANIMATION_HPP
#define CLIENT_ANIMATION_HPP

#include <ECS/ECS.h>
#include <vector>

namespace rtype::client::components {
    /**
     * @struct AnimationFrame
     * @brief Represents a single frame in a sprite animation
     * 
     * Each frame defines a rectangular region of a texture (sprite sheet)
     * and how long it should be displayed.
     */
    struct AnimationFrame {
        /** @brief X coordinate of the frame in the sprite sheet (pixels) */
        int textureX;
        
        /** @brief Y coordinate of the frame in the sprite sheet (pixels) */
        int textureY;
        
        /** @brief Width of the frame in pixels */
        int width;
        
        /** @brief Height of the frame in pixels */
        int height;
        
        /** @brief How long to display this frame (seconds) */
        float duration;
    };

    /**
     * @class Animation
     * @brief Component for sprite-based frame animations
     * 
     * The Animation component enables entities to play sprite animations
     * from sprite sheets. It supports:
     * - Multiple frames with individual durations
     * - Looping or one-shot animations
     * - Playback control (play/pause)
     * - Automatic frame advancement based on time
     * 
     * Features:
     * - Frame-based animation from sprite sheets
     * - Configurable frame durations for varied pacing
     * - Looping support for continuous animations
     * - Timer-based frame advancement
     * 
     * Example usage:
     * @code
     * auto& anim = entity.addComponent<Animation>();
     * 
     * // Add explosion animation frames
     * anim.frames.push_back({0, 0, 32, 32, 0.1f});    // Frame 1, 0.1 sec
     * anim.frames.push_back({32, 0, 32, 32, 0.1f});   // Frame 2, 0.1 sec
     * anim.frames.push_back({64, 0, 32, 32, 0.1f});   // Frame 3, 0.1 sec
     * 
     * anim.looping = false;  // One-shot animation
     * anim.playing = true;   // Start playing immediately
     * @endcode
     */
    class Animation : public ECS::Component<Animation> {
    public:
        /** @brief List of animation frames in sequence */
        std::vector<AnimationFrame> frames;
        
        /** @brief Index of the currently displayed frame */
        int currentFrame;
        
        /** @brief Time accumulator for frame duration (seconds) */
        float frameTimer;
        
        /** @brief Whether the animation should loop back to frame 0 after completion */
        bool looping;
        
        /** @brief Whether the animation is currently playing */
        bool playing;

        /**
         * @brief Construct a new Animation component
         * 
         * Initializes an empty animation. Frames must be added manually.
         * - Starts at frame 0
         * - Timer at 0.0
         * - Not looping by default
         * - Not playing by default
         */
        Animation();
    };
}

#endif // CLIENT_ANIMATION_HPP

