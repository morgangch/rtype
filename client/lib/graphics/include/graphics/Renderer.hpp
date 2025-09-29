#pragma once

#include <memory>
#include <string>

// Forward declarations
namespace rtype::client::graphics {
    class Texture;
    class Sprite;
    class Camera;
}

#ifdef RTYPE_USE_SFML
namespace sf {
    class RenderWindow;
}
#endif

namespace rtype::client::graphics {
    
    /**
     * @brief Abstract renderer interface
     * 
     * Provides a graphics backend abstraction that can be implemented
     * with different rendering libraries (SFML, SDL2, OpenGL, etc.)
     */
    class Renderer {
    public:
        virtual ~Renderer() = default;
        
        // Factory method
        static std::unique_ptr<Renderer> Create(const std::string& backend = "SFML");
        
        // Core rendering
        virtual bool Initialize(int width, int height, const std::string& title) = 0;
        virtual void Shutdown() = 0;
        virtual void Clear() = 0;
        virtual void Present() = 0;
        
        // Texture management
        virtual std::shared_ptr<Texture> LoadTexture(const std::string& path) = 0;
        virtual void UnloadTexture(const std::string& path) = 0;
        
        // Drawing primitives
        virtual void DrawSprite(const Sprite& sprite, const Camera& camera) = 0;
        virtual void DrawSprite(const Sprite& sprite, float x, float y, const Camera& camera) = 0;
        virtual void DrawRectangle(float x, float y, float width, float height, uint32_t color) = 0;
        virtual void DrawCircle(float x, float y, float radius, uint32_t color) = 0;
        
        // Camera
        virtual void SetCamera(const Camera& camera) = 0;
        
        // Window management
        virtual bool IsWindowOpen() const = 0;
        virtual void PollEvents() = 0;
    };
    
    /**
     * @brief SFML-based renderer implementation
     */
    class SFMLRenderer : public Renderer {
    public:
        SFMLRenderer();
        ~SFMLRenderer() override;
        
        bool Initialize(int width, int height, const std::string& title) override;
        void Shutdown() override;
        void Clear() override;
        void Present() override;
        
        std::shared_ptr<Texture> LoadTexture(const std::string& path) override;
        void UnloadTexture(const std::string& path) override;
        
        void DrawSprite(const Sprite& sprite, const Camera& camera) override;
        void DrawSprite(const Sprite& sprite, float x, float y, const Camera& camera) override;
        void DrawRectangle(float x, float y, float width, float height, uint32_t color) override;
        void DrawCircle(float x, float y, float radius, uint32_t color) override;
        
        void SetCamera(const Camera& camera) override;
        
        bool IsWindowOpen() const override;
        void PollEvents() override;
        
#ifdef RTYPE_USE_SFML
        // SFML-specific methods
        sf::RenderWindow* GetNativeWindow();
#endif
        
    private:
        struct SFMLData;
        std::unique_ptr<SFMLData> m_data;
    };
    
    /**
     * @brief Stub renderer for testing without graphics libraries
     */
    class StubRenderer : public Renderer {
    public:
        StubRenderer();
        ~StubRenderer() override;
        
        bool Initialize(int width, int height, const std::string& title) override;
        void Shutdown() override;
        void Clear() override;
        void Present() override;
        
        std::shared_ptr<Texture> LoadTexture(const std::string& path) override;
        void UnloadTexture(const std::string& path) override;
        
        void DrawSprite(const Sprite& sprite, const Camera& camera) override;
        void DrawSprite(const Sprite& sprite, float x, float y, const Camera& camera) override;
        void DrawRectangle(float x, float y, float width, float height, uint32_t color) override;
        void DrawCircle(float x, float y, float radius, uint32_t color) override;
        
        void SetCamera(const Camera& camera) override;
        
        bool IsWindowOpen() const override;
        void PollEvents() override;
        
    private:
        struct StubData;
        std::unique_ptr<StubData> m_data;
    };
}
