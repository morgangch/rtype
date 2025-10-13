/**
 * @file TextureCache.h
 * @brief Thread-safe texture cache for the client GUI
 *
 * This header declares a small, focused TextureCache utility used to centralize
 * loading and ownership of sf::Texture objects. The cache keeps textures in
 * memory using std::shared_ptr so callers (for example, sf::Sprite instances)
 * may safely reference a texture while the cache owns the shared storage.
 *
 * Usage:
 * - Call TextureCache::getInstance().loadTexture(path) during asset/factory
 *   initialization to preload textures off the hot render path.
 * - In the render loop prefer calling getTexture(path) to obtain an existing
 *   cached texture. If missing, loadTexture(path) can be used as a one-time
 *   fallback.
 *
 * Notes:
 * - The class is thread-safe for concurrent loads/reads using a mutex.
 * - The cache intentionally returns std::shared_ptr<sf::Texture> so ownership
 *   is clear and textures are freed when no longer referenced.
 */

#ifndef CLIENT_GUI_TEXTURECACHE_HPP
#define CLIENT_GUI_TEXTURECACHE_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace rtype::client::gui {

/**
 * @class TextureCache
 * @brief Simple thread-safe cache for sf::Texture instances
 *
 * Responsibilities:
 * - Load textures from disk into memory once and reuse them.
 * - Provide safe access to textures via std::shared_ptr to clearly express
 *   ownership semantics.
 * - Allow clearing the cache (useful when unloading a level).
 */
class TextureCache {
public:
    /**
     * @brief Get the global TextureCache singleton instance
     * @return Reference to the shared cache
     */
    static TextureCache& getInstance();

    /**
     * @brief Load a texture into the cache
     *
     * If the texture for `path` is already present the existing shared_ptr is
     * returned. On load failure this returns nullptr and does not insert into
     * the cache.
     *
     * @param path Filesystem path to the texture image
     * @return std::shared_ptr<sf::Texture> Shared pointer to loaded texture or
     *         nullptr on failure
     */
    std::shared_ptr<sf::Texture> loadTexture(const std::string& path);

    /**
     * @brief Get an already-loaded texture from the cache
     *
     * Returns nullptr when the texture is not present. This call does not
     * perform I/O and is safe to call from the hot render path.
     *
     * @param path Key / path used when loading the texture
     * @return std::shared_ptr<sf::Texture> Shared pointer if present, else
     *         nullptr
     */
    std::shared_ptr<sf::Texture> getTexture(const std::string& path) const;

    /**
     * @brief Remove all cached textures
     *
     * Use when unloading resources (for example when changing levels). Any
     * outstanding shared_ptrs held by callers will keep textures alive until
     * they are released.
     */
    void clear();

private:
    /**
     * @brief Private default constructor for singleton
     *
     * Instances should be obtained via getInstance(); copying or moving
     * the singleton is disallowed.
     */
    TextureCache() = default;

    /**
     * @brief Deleted copy constructor to prevent copying the singleton
     */
    TextureCache(const TextureCache&) = delete;

    /**
     * @brief Deleted copy assignment to prevent copying the singleton
     */
    TextureCache& operator=(const TextureCache&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving the singleton
     */
    TextureCache(TextureCache&&) = delete;

    /**
     * @brief Deleted move assignment to prevent moving the singleton
     */
    TextureCache& operator=(TextureCache&&) = delete;

    /**
     * @brief Mutex protecting concurrent access to the cache
     *
     * The mutex is mutable to allow const getTexture() to lock it for
     * thread-safe reads.
     */
    mutable std::mutex m_mutex; ///< Protects access to m_cache

    /**
     * @brief Internal map storing loaded textures
     *
     * Key: filesystem path used to load the texture
     * Value: shared_ptr owning the sf::Texture
     */
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_cache; ///< Path -> Texture
};

} // namespace rtype::client::gui

#endif // CLIENT_GUI_TEXTURECACHE_HPP
