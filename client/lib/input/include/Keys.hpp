#pragma once

namespace rtype::client::input {
    
    enum class Key {
        // Alphabet
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        
        // Numbers
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        
        // Function keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        
        // Arrow keys
        Left, Right, Up, Down,
        
        // Common keys
        Space, Enter, Escape, Tab, Backspace, Delete,
        LShift, RShift, LCtrl, RCtrl, LAlt, RAlt,
        
        // Space Invaders specific
        Fire = Space,
        MoveLeft = Left,
        MoveRight = Right,
        MoveUp = Up,
        MoveDown = Down,
        Pause = Escape,
        
        Count // Keep this last
    };
    
    enum class MouseButton {
        Left,
        Right,
        Middle,
        Count
    };
    
    struct MouseState {
        int x = 0;
        int y = 0;
        bool buttons[static_cast<int>(MouseButton::Count)] = {false};
        int wheelDelta = 0;
    };
}