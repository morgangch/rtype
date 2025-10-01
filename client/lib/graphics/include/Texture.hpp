#pragma once

#include <memory>
#include <string>

namespace rtype::client::graphics {
    
    /**
     * @brief Abstract texture interface
     */
    class Texture {
    public:
        virtual ~Texture() = default;
        
        virtual bool Load() = 0;
        virtual void Unload() = 0;
        virtual bool IsLoaded() const = 0;
        virtual void* GetNativeHandle() const = 0;
        
        // Getters
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        const std::string& GetPath() const { return m_path; }
        
    protected:
        std::string m_path;
        int m_width = 0;
        int m_height = 0;
    };
    
    /**
     * @brief SFML texture implementation
     */
    class SFMLTexture : public Texture {
    public:
        explicit SFMLTexture(const std::string& path);
        ~SFMLTexture() override;
        
        bool Load() override;
        void Unload() override;
        bool IsLoaded() const override;
        void* GetNativeHandle() const override;
        
        // SFML-specific
        void SetSmooth(bool smooth);
        void SetRepeated(bool repeated);
        
    private:
        struct SFMLData;
        std::unique_ptr<SFMLData> m_data;
    };
    
    /**
     * @brief Stub texture for testing
     */
    class StubTexture : public Texture {
    public:
        explicit StubTexture(const std::string& path);
        ~StubTexture() override;
        
        bool Load() override;
        void Unload() override;
        bool IsLoaded() const override;
        void* GetNativeHandle() const override;
        
    private:
        struct StubData;
        std::unique_ptr<StubData> m_data;
    };
}
