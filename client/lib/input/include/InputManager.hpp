#pragma once

#include "Keys.hpp"

namespace rtype::client::input {
    
    /**
     * @brief Input manager for handling keyboard, mouse and gamepad inputs
     * 
     * Provides a unified interface for all input devices with state tracking
     */
    class InputManager {
    public:
        InputManager();
        ~InputManager();
        
        // Core methods
        void Update();
        void Reset();
        
        // Keyboard state
        bool IsKeyPressed(Key key) const;    // Just pressed this frame
        bool IsKeyDown(Key key) const;       // Held down
        bool IsKeyReleased(Key key) const;   // Just released this frame
        
        // Mouse state  
        MouseState GetMouseState() const;
        bool IsMouseButtonPressed(MouseButton button) const;
        bool IsMouseButtonDown(MouseButton button) const;
        bool IsMouseButtonReleased(MouseButton button) const;
        
        // Convenience methods for Space Invaders
        float GetPlayerMovement() const;     // Returns -1.0 (left), 0.0 (none), or 1.0 (right)
        float GetPlayerVerticalMovement() const;  // Returns -1.0 (up), 0.0 (none), or 1.0 (down)
        bool IsFirePressed() const;          // Space key just pressed
        bool IsPausePressed() const;         // Escape key just pressed
        
        // Integration with SFML events (called by renderer)
        void HandleKeyPressed(Key key);
        void HandleKeyReleased(Key key);
        void HandleMousePressed(MouseButton button, int x, int y);
        void HandleMouseReleased(MouseButton button, int x, int y);
        void HandleMouseMoved(int x, int y);
        
    private:
        // Key states
        bool m_currentKeys[static_cast<int>(Key::Count)] = {false};
        bool m_previousKeys[static_cast<int>(Key::Count)] = {false};
        
        // Mouse states
        MouseState m_currentMouse;
        MouseState m_previousMouse;
        bool m_currentMouseButtons[static_cast<int>(MouseButton::Count)] = {false};
        bool m_previousMouseButtons[static_cast<int>(MouseButton::Count)] = {false};
        
        // Helper methods
        void UpdateKeyStates();
        void UpdateMouseStates();
    };
}