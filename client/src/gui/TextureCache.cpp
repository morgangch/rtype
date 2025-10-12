/**
 * @file TextureCache.cpp
 * @brief Implementation of a simple texture cache
 */

#include "gui/TextureCache.h"
#include <iostream>

namespace rtype::client::gui {

TextureCache& TextureCache::getInstance() {
    static TextureCache instance;
    return instance;
}

std::shared_ptr<sf::Texture> TextureCache::loadTexture(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(path);
    if (it != m_cache.end()) return it->second;

    auto tex = std::make_shared<sf::Texture>();
    if (!tex->loadFromFile(path)) {
        std::cerr << "TextureCache: failed to load texture: " << path << '\n';
        return nullptr;
    }
    m_cache.emplace(path, tex);
    return tex;
}

std::shared_ptr<sf::Texture> TextureCache::getTexture(const std::string& path) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(path);
    return (it != m_cache.end()) ? it->second : nullptr;
}

void TextureCache::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
}

} // namespace rtype::client::gui
