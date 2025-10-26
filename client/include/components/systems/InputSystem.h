/**
 * @file InputSystem.h
 * @brief Client-side input handling system
 * 
 * This file defines the InputSystem class which manages keyboard input state
 * tracking, including pressed, held, just-pressed, and just-released states.
 * 
 * Part of the ECS architecture for handling player input.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_INPUT_SYSTEM_HPP
#define CLIENT_INPUT_SYSTEM_HPP

#include <common/core/System.h>
#include <unordered_map>

namespace rtype::client::systems {
    /**
     * @brief Enumeration of possible key states
     * 
     * @details Tracks the lifecycle of a key press:
     * - Released: Key is not pressed (default state)
     * - JustPressed: Key was just pressed this frame (transitions to Held next frame)
     * - Held: Key is being held down (remains until release)
     * - JustReleased: Key was just released this frame (transitions to Released next frame)
     * 
     * This allows differentiation between continuous holding and single-frame events.
     */
    enum class KeyState {
        Released = 0,      ///< Key is not pressed
        JustPressed,       ///< Key was pressed this frame
        Held,              ///< Key is being held down
        JustReleased       ///< Key was released this frame
    };

    /**
     * @brief ECS system for handling keyboard input
     * 
     * @details Manages keyboard state tracking with frame-accurate input detection.
     * Supports both continuous (held) and discrete (just-pressed/released) input queries.
     * 
     * Usage pattern:
     * 1. Call handleKeyEvent() when keyboard events occur (from main event loop)
     * 2. Call update() each frame to transition states (JustPressed → Held, etc.)
     * 3. Query states using isKeyPressed() or isKeyJustPressed() in game logic
     * 
     * Thread safety: Not thread-safe. All methods should be called from the main thread.
     */
    class InputSystem : public System {
    public:
        /**
         * @brief Update the input system state
         * @param cm Component manager (unused in current implementation)
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * @details Transitions key states for the next frame:
         * - JustPressed → Held
         * - JustReleased → Released
         * 
         * Should be called once per frame before processing game logic.
         */
        void update(ComponentManager& cm, float deltaTime) override;
        
        /**
         * @brief Handle a keyboard event
         * @param key Key code (typically SFML key codes)
         * @param pressed True if key was pressed, false if released
         * 
         * @details Updates the internal key state based on the event:
         * - pressed=true: Sets state to JustPressed
         * - pressed=false: Sets state to JustReleased
         * 
         * Should be called from the main event loop when keyboard events occur.
         */
        void handleKeyEvent(int key, bool pressed);

        /**
         * @brief Check if a key is currently pressed (held or just pressed)
         * @param key Key code to check
         * @return True if the key is in Held or JustPressed state
         * 
         * @details Use this for continuous actions (e.g., movement while holding arrow keys).
         * Returns true for both JustPressed and Held states.
         */
        bool isKeyPressed(int key) const;
        
        /**
         * @brief Check if a key was just pressed this frame
         * @param key Key code to check
         * @return True if the key is in JustPressed state
         * 
         * @details Use this for single-frame actions (e.g., shooting, jumping).
         * Only returns true on the first frame the key is pressed.
         * 
         * @note This state automatically transitions to Held on the next frame.
         */
        bool isKeyJustPressed(int key) const;

    private:
        /**
         * @brief Internal key state storage
         * @details Maps key codes to their current KeyState
         */
        std::unordered_map<int, KeyState> keyStates;
        
        /**
         * @brief Update key states for the next frame
         * 
         * @details Internal helper that transitions transient states:
         * - JustPressed → Held
         * - JustReleased → Released
         */
        void updateKeyStates();
    };
}

#endif // CLIENT_INPUT_SYSTEM_HPP
