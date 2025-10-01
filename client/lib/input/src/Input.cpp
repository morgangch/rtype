#include "Input.hpp"
#include <iostream>

namespace rtype::client::input {
    
    Input::Input() {
    }
    
    Input::~Input() {
        Shutdown();
    }
    
    bool Input::Initialize() {
        std::cout << "[Input] Initializing input system..." << std::endl;
        m_initialized = true;
        std::cout << "[Input] Input system initialized successfully" << std::endl;
        return true;
    }
    
    void Input::Shutdown() {
        if (m_initialized) {
            std::cout << "[Input] Shutting down input system" << std::endl;
            m_initialized = false;
        }
    }
    
    void Input::Update() {
        if (m_initialized) {
            m_inputManager.Update();
        }
    }
    
    void Input::Reset() {
        if (m_initialized) {
            m_inputManager.Reset();
        }
    }
    
    float Input::GetPlayerMovement() const {
        if (m_initialized) {
            return m_inputManager.GetPlayerMovement();
        }
        return 0.0f;
    }
    
    float Input::GetPlayerVerticalMovement() const {
        if (m_initialized) {
            return m_inputManager.GetPlayerVerticalMovement();
        }
        return 0.0f;
    }
    
    bool Input::IsFirePressed() const {
        if (m_initialized) {
            return m_inputManager.IsFirePressed();
        }
        return false;
    }
    
    bool Input::IsPausePressed() const {
        if (m_initialized) {
            return m_inputManager.IsPausePressed();
        }
        return false;
    }
    
    bool Input::IsKeyPressed(Key key) const {
        if (m_initialized) {
            return m_inputManager.IsKeyPressed(key);
        }
        return false;
    }
    
    bool Input::IsKeyDown(Key key) const {
        if (m_initialized) {
            return m_inputManager.IsKeyDown(key);
        }
        return false;
    }
}