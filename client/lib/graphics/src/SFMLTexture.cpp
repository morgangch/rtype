#include "graphics/Texture.hpp"
#include <iostream>

#ifdef RTYPE_USE_SFML
#include <SFML/Graphics.hpp>
#endif

namespace rtype::client::graphics {
    
#ifdef RTYPE_USE_SFML
    
    struct SFMLTexture::SFMLData {
        sf::Texture texture;
        bool loaded = false;
    };
    
    SFMLTexture::SFMLTexture(const std::string& path) 
        : m_data(std::make_unique<SFMLData>()) {
        m_path = path;
    }
    
    SFMLTexture::~SFMLTexture() {
        Unload();
    }
    
    bool SFMLTexture::Load() {
        if (m_data->loaded) {
            return true;
        }
        
        if (m_data->texture.loadFromFile(m_path)) {
            m_data->loaded = true;
            m_width = m_data->texture.getSize().x;
            m_height = m_data->texture.getSize().y;
            std::cout << "[SFMLTexture] Loaded: " << m_path 
                      << " (" << m_width << "x" << m_height << ")" << std::endl;
            return true;
        }
        
        std::cout << "[SFMLTexture] Failed to load: " << m_path << std::endl;
        return false;
    }
    
    void SFMLTexture::Unload() {
        if (m_data->loaded) {
            std::cout << "[SFMLTexture] Unloaded: " << m_path << std::endl;
            m_data->loaded = false;
            m_width = 0;
            m_height = 0;
        }
    }
    
    bool SFMLTexture::IsLoaded() const {
        return m_data->loaded;
    }
    
    void* SFMLTexture::GetNativeHandle() const {
        return m_data->loaded ? (void*)&m_data->texture : nullptr;
    }
    
    void SFMLTexture::SetSmooth(bool smooth) {
        if (m_data->loaded) {
            m_data->texture.setSmooth(smooth);
        }
    }
    
    void SFMLTexture::SetRepeated(bool repeated) {
        if (m_data->loaded) {
            m_data->texture.setRepeated(repeated);
        }
    }
    
#endif
}