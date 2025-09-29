#pragma once

#include <memory>

namespace rtype::client::graphics {
    
    class Texture;
    
    /**
     * @brief Sprite class for 2D rendering
     */
    class Sprite {
    public:
        Sprite();
        ~Sprite();
        
        // Texture
        void SetTexture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> GetTexture() const;
        
        // Position
        void SetPosition(float x, float y);
        float GetX() const { return m_x; }
        float GetY() const { return m_y; }
        
        // Scale
        void SetScale(float scaleX, float scaleY);
        void SetScale(float scale) { SetScale(scale, scale); }
        float GetScaleX() const { return m_scaleX; }
        float GetScaleY() const { return m_scaleY; }
        
        // Rotation
        void SetRotation(float rotation);
        float GetRotation() const { return m_rotation; }
        
        // Source rectangle (for sprite sheets)
        void SetSourceRect(int x, int y, int width, int height);
        void GetSourceRect(int& x, int& y, int& width, int& height) const;
        
        // Color tint (RGBA)
        void SetTint(uint32_t color);
        uint32_t GetTint() const { return m_tint; }
        
        // Visibility
        void SetVisible(bool visible);
        bool IsVisible() const { return m_visible; }
        
    private:
        std::shared_ptr<Texture> m_texture;
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_scaleX = 1.0f;
        float m_scaleY = 1.0f;
        float m_rotation = 0.0f;
        int m_srcX = 0;
        int m_srcY = 0;
        int m_srcWidth = 0;
        int m_srcHeight = 0;
        uint32_t m_tint = 0xFFFFFFFF; // White, fully opaque
        bool m_visible = true;
    };
}