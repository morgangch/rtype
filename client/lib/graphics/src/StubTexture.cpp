#include "Texture.hpp"
#include <iostream>

namespace rtype::client::graphics {
    
    // Stub Texture Implementation
    struct StubTexture::StubData {
        bool loaded = false;
    };
    
    StubTexture::StubTexture(const std::string& path) 
        : m_data(std::make_unique<StubData>()) {
        m_path = path;
    }
    
    StubTexture::~StubTexture() {
        Unload();
    }
    
    bool StubTexture::Load() {
        if (!m_data->loaded) {
            std::cout << "[StubTexture] Loading (stub): " << m_path << std::endl;
            m_data->loaded = true;
            m_width = 64; // Default stub size
            m_height = 64;
        }
        return true;
    }
    
    void StubTexture::Unload() {
        if (m_data->loaded) {
            std::cout << "[StubTexture] Unloading (stub): " << m_path << std::endl;
            m_data->loaded = false;
            m_width = 0;
            m_height = 0;
        }
    }
    
    bool StubTexture::IsLoaded() const {
        return m_data->loaded;
    }
    
    void* StubTexture::GetNativeHandle() const {
        return nullptr; // Stub has no native handle
    }
}