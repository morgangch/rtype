#include "Renderer.hpp"
#include "Texture.hpp"
#include <iostream>

namespace rtype::client::graphics {
    
    // Stub Renderer Implementation
    struct StubRenderer::StubData {
        bool windowOpen = false;
        int frameCount = 0;
    };
    
    StubRenderer::StubRenderer() : m_data(std::make_unique<StubData>()) {
    }
    
    StubRenderer::~StubRenderer() {
        Shutdown();
    }
    
    bool StubRenderer::Initialize(int width, int height, const std::string& title) {
        std::cout << "[StubRenderer] Initializing (stub mode) " << width << "x" << height << " - " << title << std::endl;
        m_data->windowOpen = true;
        return true;
    }
    
    void StubRenderer::Shutdown() {
        if (m_data->windowOpen) {
            std::cout << "[StubRenderer] Shutting down (stub mode)" << std::endl;
            m_data->windowOpen = false;
        }
    }
    
    void StubRenderer::Clear() {
        // Stub - just count frames
        m_data->frameCount++;
    }
    
    void StubRenderer::Present() {
        if (m_data->frameCount % 60 == 0) {
            std::cout << "[StubRenderer] Frame " << m_data->frameCount << std::endl;
        }
    }
    
    std::shared_ptr<Texture> StubRenderer::LoadTexture(const std::string& path) {
        std::cout << "[StubRenderer] Loading texture (stub): " << path << std::endl;
        auto texture = std::make_shared<StubTexture>(path);
        texture->Load();
        return texture;
    }
    
    void StubRenderer::UnloadTexture(const std::string& path) {
        std::cout << "[StubRenderer] Unloading texture (stub): " << path << std::endl;
    }
    
    void StubRenderer::DrawSprite(const Sprite& sprite) {
        // Stub
    }
    
    void StubRenderer::DrawSprite(const Sprite& sprite, float x, float y) {
        // Stub
    }
    
    void StubRenderer::DrawRectangle(float x, float y, float width, float height, uint32_t color) {
        // Stub
    }
    
    void StubRenderer::DrawCircle(float x, float y, float radius, uint32_t color) {
        // Stub
    }
    
    bool StubRenderer::IsWindowOpen() const {
        return m_data->windowOpen && m_data->frameCount < 300; // Auto-close after 5 seconds at 60fps
    }
    
    void StubRenderer::PollEvents() {
        // Simulate events
        if (m_data->frameCount >= 300) {
            m_data->windowOpen = false;
        }
    }
    
    void StubRenderer::SetInputManager(input::InputManager* inputManager) {
        // Stub implementation - does nothing
    }
}