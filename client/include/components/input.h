/**
 * @file input.h
 * @brief Input system for keyboard and input state management
 * 
 * This file defines the InputSystem which handles keyboard input state tracking,
 * providing frame-perfect input detection with states like just pressed,
 * held, and just released.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_INPUT_SYSTEM_HPP
#define CLIENT_INPUT_SYSTEM_HPP

#include <common/core/System.h>
#include <unordered_map>

namespace rtype::client::components {
    /**
     * @enum KeyState
     * @brief Represents the state of a keyboard key
     * 
     * This enum allows for precise input detection, distinguishing between
     * different types of key presses:
     * - Released: Key is not being pressed
     * - JustPressed: Key was pressed this frame (first frame only)
     * - Held: Key has been pressed for multiple frames
     * - JustReleased: Key was released this frame (first frame only)
     */
    enum class KeyState {
        Released = 0,    ///< Key is not pressed
        JustPressed,     ///< Key was pressed this frame (first frame of press)
        Held,            ///< Key is being held down (second+ frame of press)
        JustReleased     ///< Key was released this frame (first frame of release)
    };

    /**
     * @class InputSystem
     * @brief System for managing keyboard input state
     * 
     * The InputSystem tracks the state of all keyboard keys and provides
     * query methods to check for different types of input events. This is
     * particularly useful for:
     * - Detecting single-frame events (jump, fire)
     * - Continuous movement input (walking, flying)
     * - Input buffering and combo detection
     * 
     * Features:
     * - Per-frame state tracking for all keys
     * - Distinguishes between "just pressed" and "held"
     * - Automatic state transitions each frame
     * - Thread-safe key state queries
     * 
     * Example usage:
     * @code
     * InputSystem inputSystem;
     * 
     * // In event loop
     * if (event.type == sf::Event::KeyPressed) {
     *     inputSystem.handleKeyEvent(event.key.code, true);
     * }
     * 
     * // In update loop
     * inputSystem.update(componentManager, deltaTime);
     * 
     * // Query input state
     * if (inputSystem.isKeyJustPressed(sf::Keyboard::Space)) {
     *     player.jump();  // Only triggers once per press
     * }
     * 
     * if (inputSystem.isKeyPressed(sf::Keyboard::D)) {
     *     player.moveRight(deltaTime);  // Continuous movement
     * }
     * @endcode
     */
    class InputSystem : public System {
    public:
        /**
         * @brief Update the input system state
         * @param cm Component manager (not used but required by System interface)
         * @param deltaTime Time elapsed since last update
         * 
         * Updates all key states, transitioning:
         * - JustPressed → Held
         * - JustReleased → Released
         */
        void update(ComponentManager& cm, float deltaTime) override;
        
        /**
         * @brief Handle a keyboard event
         * @param key The keyboard key code
         * @param pressed True if key was pressed, false if released
         * 
         * Should be called from the event loop for KeyPressed and KeyReleased events.
         */
        void handleKeyEvent(int key, bool pressed);

        /**
         * @brief Check if a key is currently pressed (Held or JustPressed)
         * @param key The keyboard key code to check
         * @return True if the key is pressed, false otherwise
         */
        bool isKeyPressed(int key) const;
        
        /**
         * @brief Check if a key was just pressed this frame
         * @param key The keyboard key code to check
         * @return True if the key state is JustPressed, false otherwise
         * 
         * This is useful for single-shot actions like jumping or shooting
         * that should only trigger once per press.
         */
        bool isKeyJustPressed(int key) const;

    private:
        /** @brief Map of key codes to their current states */
        std::unordered_map<int, KeyState> keyStates;
        
        /**
         * @brief Update key states for the next frame
         * 
         * Transitions JustPressed → Held and JustReleased → Released.
         * Called automatically by update().
         */
        void updateKeyStates();
    };
}

#endif // CLIENT_INPUT_SYSTEM_HPP

