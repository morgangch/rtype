#pragma once

// Main input header - includes all input components
#include "InputManager.hpp"
#include "Keys.hpp"

namespace rtype::client::input {
    
    /**
     * @brief Input system manager
     * 
     * Simplifies input initialization and management
     */
    class Input {
    public:
        Input();
        ~Input();
        
        // Initialization
        bool Initialize();
        void Shutdown();
        
        // Main loop helpers
        void Update();
        void Reset();
        
        // Getters
        InputManager& GetInputManager() { return m_inputManager; }
        
        // Quick access methods for common operations
        float GetPlayerMovement() const;
        float GetPlayerVerticalMovement() const;
        bool IsFirePressed() const;
        bool IsPausePressed() const;
        bool IsKeyPressed(Key key) const;
        bool IsKeyDown(Key key) const;
        
    private:
        InputManager m_inputManager;
        bool m_initialized = false;
    };
}