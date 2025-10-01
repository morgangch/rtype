#include "Graphics.hpp"
#include "Renderer.hpp"
#include <iostream>

namespace rtype::client::graphics {
    
    Graphics::Graphics() {
    }
    
    Graphics::~Graphics() {
        Shutdown();
    }
    
    bool Graphics::Initialize(int width, int height, const std::string& title, const std::string& backend) {
        std::cout << "[Graphics] Initializing graphics system..." << std::endl;
        
        // Create renderer
        m_renderer = Renderer::Create(backend);
        if (!m_renderer) {
            std::cout << "[Graphics] Failed to create renderer" << std::endl;
            return false;
        }
        
        // Initialize renderer
        if (!m_renderer->Initialize(width, height, title)) {
            std::cout << "[Graphics] Failed to initialize renderer" << std::endl;
            return false;
        }
        
        m_initialized = true;
        std::cout << "[Graphics] Graphics system initialized successfully" << std::endl;
        return true;
    }
    
    void Graphics::Shutdown() {
        if (m_initialized) {
            std::cout << "[Graphics] Shutting down graphics system" << std::endl;
            if (m_renderer) {
                m_renderer->Shutdown();
                m_renderer.reset();
            }
            m_initialized = false;
        }
    }
    
    bool Graphics::IsRunning() const {
        return m_initialized && m_renderer && m_renderer->IsWindowOpen();
    }
    
    void Graphics::BeginFrame() {
        if (m_renderer) {
            m_renderer->Clear();
        }
    }
    
    void Graphics::EndFrame() {
        if (m_renderer) {
            m_renderer->Present();
        }
    }
    
    void Graphics::PollEvents() {
        if (m_renderer) {
            m_renderer->PollEvents();
        }
    }
    
    void Graphics::DrawSprite(const Sprite& sprite) {
        if (m_renderer) {
            m_renderer->DrawSprite(sprite);
        }
    }
    
    void Graphics::DrawSprite(const Sprite& sprite, float x, float y) {
        if (m_renderer) {
            m_renderer->DrawSprite(sprite, x, y);
        }
    }
    
    void Graphics::DrawRectangle(float x, float y, float width, float height, uint32_t color) {
        if (m_renderer) {
            m_renderer->DrawRectangle(x, y, width, height, color);
        }
    }
    
    void Graphics::DrawCircle(float x, float y, float radius, uint32_t color) {
        if (m_renderer) {
            m_renderer->DrawCircle(x, y, radius, color);
        }
    }
    
    std::shared_ptr<Texture> Graphics::LoadTexture(const std::string& path) {
        if (m_renderer) {
            return m_renderer->LoadTexture(path);
        }
        return nullptr;
    }
    
    void Graphics::SetInputManager(input::InputManager* inputManager) {
        if (m_renderer) {
            m_renderer->SetInputManager(inputManager);
        }
    }
    
#ifdef RTYPE_USE_SFML
    sf::RenderWindow* Graphics::GetSFMLWindow() {
        if (m_renderer) {
            // Try to cast to SFMLRenderer
            SFMLRenderer* sfmlRenderer = dynamic_cast<SFMLRenderer*>(m_renderer.get());
            if (sfmlRenderer) {
                return sfmlRenderer->GetNativeWindow();
            }
        }
        return nullptr;
    }
#endif
}
