#pragma once

#include "Keys.hpp"
#include <memory>
#include <functional>

namespace rtype::client::input {
    
    /**
     * @brief Main input management system
     * 
     * Handles all input devices and provides a unified interface
     * for the game to query input state.
     */
    class InputManager {
    public:
        InputManager();
        ~InputManager();
        
        // Core functionality
        void Update();
        void Shutdown();
        
        // Keyboard input
        bool IsKeyPressed(Key key) const;   // Just pressed this frame
        bool IsKeyDown(Key key) const;      // Held down
        bool IsKeyUp(Key key) const;        // Just released this frame
        
        // Mouse input
        MouseState GetMouseState() const;
        bool IsMouseButtonPressed(MouseButton button) const;
        bool IsMouseButtonDown(MouseButton button) const;
        bool IsMouseButtonUp(MouseButton button) const;
        
        // Event callbacks
        using KeyCallback = std::function<void(Key, bool)>;
        using MouseCallback = std::function<void(MouseButton, bool, int, int)>;
        
        void SetKeyCallback(KeyCallback callback);
        void SetMouseCallback(MouseCallback callback);
        
        // Integration with graphics system
        void HandleKeyEvent(Key key, bool pressed);
        void HandleMouseEvent(MouseButton button, bool pressed, int x, int y);
        void HandleMouseMove(int x, int y);
        void HandleMouseWheel(int delta);
        
    private:
        struct InputData;
        std::unique_ptr<InputData> m_data;
    };
}