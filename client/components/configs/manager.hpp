#pragma once
#include <unordered_map>
#include <string>
#include <memory>

namespace Client {
    template<typename T>
    class ResourceManager {
    public:
        static ResourceManager& getInstance();
        
        std::shared_ptr<T> load(const std::string& path);
        void unload(const std::string& path);
        void clear();
        
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> resources;
    };
    
    // Type aliases for common resources
    using TextureManager = ResourceManager<class Texture>;
    using SoundManager = ResourceManager<class Sound>;
    using FontManager = ResourceManager<class Font>;
}
