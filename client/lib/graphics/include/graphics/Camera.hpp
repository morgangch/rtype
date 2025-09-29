#pragma once

namespace rtype::client::graphics {
    
    /**
     * @brief 2D Camera for view management
     */
    class Camera {
    public:
        Camera();
        Camera(float x, float y, float viewportWidth, float viewportHeight);
        ~Camera();
        
        // Position
        void SetPosition(float x, float y);
        float GetX() const { return m_x; }
        float GetY() const { return m_y; }
        
        // Viewport size
        void SetViewportSize(float width, float height);
        float GetViewportWidth() const { return m_viewportWidth; }
        float GetViewportHeight() const { return m_viewportHeight; }
        
        // Zoom
        void SetZoom(float zoom);
        float GetZoom() const { return m_zoom; }
        
        // Rotation
        void SetRotation(float rotation);
        float GetRotation() const { return m_rotation; }
        
        // Coordinate conversion
        void WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const;
        void ScreenToWorld(float screenX, float screenY, float& worldX, float& worldY) const;
        
        // Movement helpers
        void Move(float deltaX, float deltaY);
        void LookAt(float x, float y);
        
    private:
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_viewportWidth = 800.0f;
        float m_viewportHeight = 600.0f;
        float m_zoom = 1.0f;
        float m_rotation = 0.0f;
    };
}