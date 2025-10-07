/**
 * @file audio.hpp
 * @brief Audio component for sound and music management in the R-TYPE client
 * 
 * This file defines the AudioSource component which represents an audio entity
 * in the ECS system. It handles both sound effects and background music with
 * configurable properties like volume, looping, and 3D positioning.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_AUDIO_SOURCE_HPP
#define CLIENT_AUDIO_SOURCE_HPP

#include <ECS/ECS.hpp>
#include <string>

namespace rtype::client::components {
    /**
     * @enum AudioType
     * @brief Categorizes different types of audio in the game
     * 
     * This enum allows the audio system to treat different audio types
     * with appropriate priority, mixing, and processing.
     */
    enum class AudioType {
        SFX,    ///< Sound effects (explosions, shots, hits)
        Music,  ///< Background music
        Voice   ///< Voice clips and dialogue
    };

    /**
     * @class AudioSource
     * @brief Component representing an audio source in the game world
     * 
     * The AudioSource component enables entities to play sounds and music.
     * It supports various audio types with configurable properties including:
     * - Volume control (0.0 to 1.0)
     * - Looping for continuous sounds
     * - 3D spatial audio positioning
     * - Play/pause state management
     * 
     * Example usage:
     * @code
     * auto& audio = entity.addComponent<AudioSource>("explosion.wav", AudioType::SFX);
     * audio.volume = 0.8f;
     * audio.playing = true;
     * audio.is3D = true;  // Enable 3D positioning based on entity position
     * @endcode
     */
    class AudioSource : public ECS::Component<AudioSource> {
    public:
        /** @brief Path to the audio file (relative to assets folder) */
        std::string soundPath;
        
        /** @brief Type of audio (SFX, Music, or Voice) */
        AudioType type;
        
        /** @brief Volume level (0.0 = silent, 1.0 = full volume) */
        float volume;
        
        /** @brief Whether the audio should loop continuously */
        bool looping;
        
        /** @brief Current playback state */
        bool playing;
        
        /** @brief Whether to use 3D positional audio based on entity position */
        bool is3D;

        /**
         * @brief Construct a new AudioSource component
         * @param soundPath Path to the audio file
         * @param type Type of audio (default: SFX)
         * 
         * Initializes an audio source with default settings:
         * - Volume: 1.0 (full)
         * - Looping: false
         * - Playing: false (needs to be started)
         * - 3D audio: false
         */
        AudioSource(const std::string& soundPath = "", AudioType type = AudioType::SFX)
            : soundPath(soundPath), type(type), volume(1.0f), looping(false), playing(false), is3D(false) {}
    };
}

#endif // CLIENT_AUDIO_SOURCE_HPP

