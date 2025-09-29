#include "graphics/Camera.hpp"
#include <cmath>

namespace rtype::client::graphics {
    
    Camera::Camera() {
    }
    
    Camera::Camera(float x, float y, float viewportWidth, float viewportHeight)
        : m_x(x), m_y(y), m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight) {
    }
    
    Camera::~Camera() {
    }
    
    void Camera::SetPosition(float x, float y) {
        m_x = x;
        m_y = y;
    }
    
    void Camera::SetViewportSize(float width, float height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }
    
    void Camera::SetZoom(float zoom) {
        m_zoom = zoom > 0.0f ? zoom : 0.1f; // Prevent negative or zero zoom
    }
    
    void Camera::SetRotation(float rotation) {
        m_rotation = rotation;
    }
    
    void Camera::WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const {
        // Simple transformation without rotation for now
        screenX = (worldX - m_x) * m_zoom + m_viewportWidth * 0.5f;
        screenY = (worldY - m_y) * m_zoom + m_viewportHeight * 0.5f;
    }
    
    void Camera::ScreenToWorld(float screenX, float screenY, float& worldX, float& worldY) const {
        // Inverse transformation
        worldX = (screenX - m_viewportWidth * 0.5f) / m_zoom + m_x;
        worldY = (screenY - m_viewportHeight * 0.5f) / m_zoom + m_y;
    }
    
    void Camera::Move(float deltaX, float deltaY) {
        m_x += deltaX;
        m_y += deltaY;
    }
    
    void Camera::LookAt(float x, float y) {
        m_x = x;
        m_y = y;
    }
}