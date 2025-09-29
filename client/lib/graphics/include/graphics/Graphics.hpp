#pragma once

// Main graphics header - includes all graphics components
#include "graphics/Renderer.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/Camera.hpp"

namespace rtype::client::graphics {
    
    /**
     * @brief Graphics system manager
     * 
     * Simplifies graphics initialization and management
     */
    class Graphics {
    public:
        Graphics();
        ~Graphics();
        
        // Initialization
        bool Initialize(int width = 800, int height = 600, const std::string& title = "R-Type", const std::string& backend = "SFML");
        void Shutdown();
        
        // Main loop helpers
        bool IsRunning() const;
        void BeginFrame();
        void EndFrame();
        void PollEvents();
        
        // Getters
        Renderer* GetRenderer() { return m_renderer.get(); }
        Camera& GetCamera() { return m_camera; }
        
        // Quick drawing methods
        void DrawSprite(const Sprite& sprite);
        void DrawSprite(const Sprite& sprite, float x, float y);
        void DrawRectangle(float x, float y, float width, float height, uint32_t color = 0xFFFFFFFF);
        void DrawCircle(float x, float y, float radius, uint32_t color = 0xFFFFFFFF);
        
        // Texture loading
        std::shared_ptr<Texture> LoadTexture(const std::string& path);
        
    private:
        std::unique_ptr<Renderer> m_renderer;
        Camera m_camera;
        bool m_initialized = false;
    };
}