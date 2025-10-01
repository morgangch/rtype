#include "InputManager.hpp"
#include <iostream>
#include <cstring>

namespace rtype::client::input {
    
    InputManager::InputManager() {
        Reset();
        std::cout << "[InputManager] Created" << std::endl;
    }
    
    InputManager::~InputManager() {
        std::cout << "[InputManager] Destroyed" << std::endl;
    }
    
    void InputManager::Update() {
        // Copy current states to previous
        std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));
        std::memcpy(m_previousMouseButtons, m_currentMouseButtons, sizeof(m_currentMouseButtons));
        m_previousMouse = m_currentMouse;
        
        // Reset wheel delta
        m_currentMouse.wheelDelta = 0;
    }
    
    void InputManager::Reset() {
        // Clear all states
        std::memset(m_currentKeys, 0, sizeof(m_currentKeys));
        std::memset(m_previousKeys, 0, sizeof(m_previousKeys));
        std::memset(m_currentMouseButtons, 0, sizeof(m_currentMouseButtons));
        std::memset(m_previousMouseButtons, 0, sizeof(m_previousMouseButtons));
        
        m_currentMouse = MouseState{};
        m_previousMouse = MouseState{};
    }
    
    bool InputManager::IsKeyPressed(Key key) const {
        int index = static_cast<int>(key);
        if (index < 0 || index >= static_cast<int>(Key::Count)) return false;
        return m_currentKeys[index] && !m_previousKeys[index];
    }
    
    bool InputManager::IsKeyDown(Key key) const {
        int index = static_cast<int>(key);
        if (index < 0 || index >= static_cast<int>(Key::Count)) return false;
        return m_currentKeys[index];
    }
    
    bool InputManager::IsKeyReleased(Key key) const {
        int index = static_cast<int>(key);
        if (index < 0 || index >= static_cast<int>(Key::Count)) return false;
        return !m_currentKeys[index] && m_previousKeys[index];
    }
    
    MouseState InputManager::GetMouseState() const {
        return m_currentMouse;
    }
    
    bool InputManager::IsMouseButtonPressed(MouseButton button) const {
        int index = static_cast<int>(button);
        if (index < 0 || index >= static_cast<int>(MouseButton::Count)) return false;
        return m_currentMouseButtons[index] && !m_previousMouseButtons[index];
    }
    
    bool InputManager::IsMouseButtonDown(MouseButton button) const {
        int index = static_cast<int>(button);
        if (index < 0 || index >= static_cast<int>(MouseButton::Count)) return false;
        return m_currentMouseButtons[index];
    }
    
    bool InputManager::IsMouseButtonReleased(MouseButton button) const {
        int index = static_cast<int>(button);
        if (index < 0 || index >= static_cast<int>(MouseButton::Count)) return false;
        return !m_currentMouseButtons[index] && m_previousMouseButtons[index];
    }
    
    // Convenience methods for Space Invaders
    float InputManager::GetPlayerMovement() const {
        float movement = 0.0f;
        if (IsKeyDown(Key::Left) || IsKeyDown(Key::MoveLeft) || IsKeyDown(Key::A)) {
            movement -= 1.0f;
        }
        if (IsKeyDown(Key::Right) || IsKeyDown(Key::MoveRight) || IsKeyDown(Key::D)) {
            movement += 1.0f;
        }
        return movement;
    }
    
    float InputManager::GetPlayerVerticalMovement() const {
        float movement = 0.0f;
        if (IsKeyDown(Key::Up) || IsKeyDown(Key::MoveUp) || IsKeyDown(Key::W)) {
            movement -= 1.0f;  // Up is negative Y
        }
        if (IsKeyDown(Key::Down) || IsKeyDown(Key::MoveDown) || IsKeyDown(Key::S)) {
            movement += 1.0f;  // Down is positive Y
        }
        return movement;
    }
    
    bool InputManager::IsFirePressed() const {
        return IsKeyPressed(Key::Space) || IsKeyPressed(Key::Fire);
    }
    
    bool InputManager::IsPausePressed() const {
        return IsKeyPressed(Key::Escape) || IsKeyPressed(Key::Pause);
    }
    
    // Event handling methods (called by graphics system)
    void InputManager::HandleKeyPressed(Key key) {
        int index = static_cast<int>(key);
        if (index >= 0 && index < static_cast<int>(Key::Count)) {
            m_currentKeys[index] = true;
        }
    }
    
    void InputManager::HandleKeyReleased(Key key) {
        int index = static_cast<int>(key);
        if (index >= 0 && index < static_cast<int>(Key::Count)) {
            m_currentKeys[index] = false;
        }
    }
    
    void InputManager::HandleMousePressed(MouseButton button, int x, int y) {
        int index = static_cast<int>(button);
        if (index >= 0 && index < static_cast<int>(MouseButton::Count)) {
            m_currentMouseButtons[index] = true;
        }
        m_currentMouse.x = x;
        m_currentMouse.y = y;
    }
    
    void InputManager::HandleMouseReleased(MouseButton button, int x, int y) {
        int index = static_cast<int>(button);
        if (index >= 0 && index < static_cast<int>(MouseButton::Count)) {
            m_currentMouseButtons[index] = false;
        }
        m_currentMouse.x = x;
        m_currentMouse.y = y;
    }
    
    void InputManager::HandleMouseMoved(int x, int y) {
        m_currentMouse.x = x;
        m_currentMouse.y = y;
    }
    
    void InputManager::UpdateKeyStates() {
        // This method is called by Update() and is already handled there
    }
    
    void InputManager::UpdateMouseStates() {
        // This method is called by Update() and is already handled there
    }
}