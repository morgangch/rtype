#include "graphics/Sprite.hpp"
#include "graphics/Texture.hpp"

namespace rtype::client::graphics {
    
    Sprite::Sprite() {
    }
    
    Sprite::~Sprite() {
    }
    
    void Sprite::SetTexture(std::shared_ptr<Texture> texture) {
        m_texture = texture;
        if (texture && m_srcWidth == 0 && m_srcHeight == 0) {
            // Auto-set source rect to full texture
            m_srcWidth = texture->GetWidth();
            m_srcHeight = texture->GetHeight();
        }
    }
    
    std::shared_ptr<Texture> Sprite::GetTexture() const {
        return m_texture;
    }
    
    void Sprite::SetPosition(float x, float y) {
        m_x = x;
        m_y = y;
    }
    
    void Sprite::SetScale(float scaleX, float scaleY) {
        m_scaleX = scaleX;
        m_scaleY = scaleY;
    }
    
    void Sprite::SetRotation(float rotation) {
        m_rotation = rotation;
    }
    
    void Sprite::SetSourceRect(int x, int y, int width, int height) {
        m_srcX = x;
        m_srcY = y;
        m_srcWidth = width;
        m_srcHeight = height;
    }
    
    void Sprite::GetSourceRect(int& x, int& y, int& width, int& height) const {
        x = m_srcX;
        y = m_srcY;
        width = m_srcWidth;
        height = m_srcHeight;
    }
    
    void Sprite::SetTint(uint32_t color) {
        m_tint = color;
    }
    
    void Sprite::SetVisible(bool visible) {
        m_visible = visible;
    }
}