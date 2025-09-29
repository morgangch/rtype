#include "graphics/Renderer.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/Camera.hpp"
#include <iostream>
#include <unordered_map>

#ifdef RTYPE_USE_SFML
#include <SFML/Graphics.hpp>
#endif

namespace rtype::client::graphics {
    
    std::unique_ptr<Renderer> Renderer::Create(const std::string& backend) {
#ifdef RTYPE_USE_SFML
        if (backend == "SFML" || backend.empty()) {
            return std::make_unique<SFMLRenderer>();
        }
        
        std::cout << "[Renderer] Backend '" << backend << "' not available with SFML build" << std::endl;
        return nullptr;
#else
        std::cout << "[Renderer] Backend '" << backend << "' not available, using stub" << std::endl;
        return std::make_unique<StubRenderer>();
#endif
    }
    
#ifdef RTYPE_USE_SFML
    
    struct SFMLRenderer::SFMLData {
        sf::RenderWindow window;
        std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
        sf::View currentView;
        bool initialized = false;
    };
    
    SFMLRenderer::SFMLRenderer() : m_data(std::make_unique<SFMLData>()) {
    }
    
    SFMLRenderer::~SFMLRenderer() {
        Shutdown();
    }
    
    bool SFMLRenderer::Initialize(int width, int height, const std::string& title) {
        std::cout << "[SFMLRenderer] Initializing " << width << "x" << height << " - " << title << std::endl;
        
        m_data->window.create(sf::VideoMode(width, height), title);
        m_data->window.setVerticalSyncEnabled(true);
        m_data->window.setFramerateLimit(60);
        
        // Set up default view
        m_data->currentView = sf::View(sf::FloatRect(0, 0, width, height));
        m_data->window.setView(m_data->currentView);
        
        m_data->initialized = true;
        std::cout << "[SFMLRenderer] Initialized successfully" << std::endl;
        return true;
    }
    
    void SFMLRenderer::Shutdown() {
        if (m_data->initialized && m_data->window.isOpen()) {
            std::cout << "[SFMLRenderer] Shutting down" << std::endl;
            m_data->window.close();
            m_data->textureCache.clear();
            m_data->initialized = false;
        }
    }
    
    void SFMLRenderer::Clear() {
        m_data->window.clear(sf::Color::Black);
    }
    
    void SFMLRenderer::Present() {
        m_data->window.display();
    }
    
    std::shared_ptr<Texture> SFMLRenderer::LoadTexture(const std::string& path) {
        // Check cache first
        auto it = m_data->textureCache.find(path);
        if (it != m_data->textureCache.end()) {
            return it->second;
        }
        
        // Create new texture
        auto texture = std::make_shared<SFMLTexture>(path);
        if (texture->Load()) {
            m_data->textureCache[path] = texture;
            std::cout << "[SFMLRenderer] Loaded texture: " << path << std::endl;
            return texture;
        }
        
        std::cout << "[SFMLRenderer] Failed to load texture: " << path << std::endl;
        return nullptr;
    }
    
    void SFMLRenderer::UnloadTexture(const std::string& path) {
        auto it = m_data->textureCache.find(path);
        if (it != m_data->textureCache.end()) {
            std::cout << "[SFMLRenderer] Unloaded texture: " << path << std::endl;
            m_data->textureCache.erase(it);
        }
    }
    
    void SFMLRenderer::DrawSprite(const Sprite& sprite, const Camera& camera) {
        DrawSprite(sprite, sprite.GetX(), sprite.GetY(), camera);
    }
    
    void SFMLRenderer::DrawSprite(const Sprite& sprite, float x, float y, const Camera& camera) {
        if (!sprite.IsVisible() || !sprite.GetTexture()) {
            return;
        }
        
        auto* sfmlTexture = static_cast<sf::Texture*>(sprite.GetTexture()->GetNativeHandle());
        if (!sfmlTexture) return;
        
        sf::Sprite sfmlSprite(*sfmlTexture);
        
        // Set position (world coordinates)
        sfmlSprite.setPosition(x, y);
        
        // Set scale
        sfmlSprite.setScale(sprite.GetScaleX(), sprite.GetScaleY());
        
        // Set rotation
        sfmlSprite.setRotation(sprite.GetRotation());
        
        // Set source rectangle for sprite sheets
        int srcX, srcY, srcW, srcH;
        sprite.GetSourceRect(srcX, srcY, srcW, srcH);
        if (srcW > 0 && srcH > 0) {
            sfmlSprite.setTextureRect(sf::IntRect(srcX, srcY, srcW, srcH));
        }
        
        // Set color tint
        uint32_t tint = sprite.GetTint();
        uint8_t r = (tint >> 24) & 0xFF;
        uint8_t g = (tint >> 16) & 0xFF;
        uint8_t b = (tint >> 8) & 0xFF;
        uint8_t a = tint & 0xFF;
        sfmlSprite.setColor(sf::Color(r, g, b, a));
        
        m_data->window.draw(sfmlSprite);
    }
    
    void SFMLRenderer::DrawRectangle(float x, float y, float width, float height, uint32_t color) {
        sf::RectangleShape rect(sf::Vector2f(width, height));
        rect.setPosition(x, y);
        
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;
        rect.setFillColor(sf::Color(r, g, b, a));
        
        m_data->window.draw(rect);
    }
    
    void SFMLRenderer::DrawCircle(float x, float y, float radius, uint32_t color) {
        sf::CircleShape circle(radius);
        circle.setPosition(x - radius, y - radius); // Center the circle
        
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;
        circle.setFillColor(sf::Color(r, g, b, a));
        
        m_data->window.draw(circle);
    }
    
    void SFMLRenderer::SetCamera(const Camera& camera) {
        sf::View view;
        view.setCenter(camera.GetX(), camera.GetY());
        view.setSize(camera.GetViewportWidth() / camera.GetZoom(), 
                     camera.GetViewportHeight() / camera.GetZoom());
        view.setRotation(camera.GetRotation());
        
        m_data->currentView = view;
        m_data->window.setView(view);
    }
    
    bool SFMLRenderer::IsWindowOpen() const {
        return m_data->initialized && m_data->window.isOpen();
    }
    
    void SFMLRenderer::PollEvents() {
        sf::Event event;
        while (m_data->window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_data->window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "[SFMLRenderer] ESC pressed, closing window" << std::endl;
                    m_data->window.close();
                }
                // TODO: Forward other key events to input system
            }
        }
    }
    
    sf::RenderWindow* SFMLRenderer::GetNativeWindow() {
        return &m_data->window;
    }
    
#endif
}