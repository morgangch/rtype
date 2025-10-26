/**
 * @file manager.h
 * @brief Generic resource manager for client-side asset loading
 * 
 * This file provides a template-based resource management system that handles
 * loading, caching, and unloading of game assets (textures, sounds, fonts).
 * 
 * Uses singleton pattern for global access and shared_ptr for automatic memory management.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_RESOURCE_MANAGER_HPP
#define CLIENT_RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <memory>

namespace rtype::client::configs {
    /**
     * @brief Generic resource manager template
     * @tparam T Resource type (e.g., Texture, Sound, Font)
     * 
     * @details Singleton-based manager that caches resources by file path to avoid
     * duplicate loading. Uses shared_ptr for automatic reference counting and cleanup.
     * 
     * Thread safety: Not thread-safe by default. External synchronization required
     * for multi-threaded resource loading.
     */
    template<typename T>
    class ResourceManager {
    public:
        /**
         * @brief Get the singleton instance
         * @return Reference to the global ResourceManager instance
         * 
         * @details Returns the same instance across all calls, ensuring centralized
         * resource management and preventing duplicate resource loading.
         */
        static ResourceManager& getInstance();

        /**
         * @brief Load a resource from the specified file path
         * @param path File path to the resource (relative or absolute)
         * @return Shared pointer to the loaded resource
         * 
         * @details If the resource is already loaded (cached), returns the existing
         * shared_ptr. Otherwise, loads the resource from disk and caches it.
         * 
         * @throws std::runtime_error If the resource file cannot be found or loaded
         */
        std::shared_ptr<T> load(const std::string& path);
        
        /**
         * @brief Unload a specific resource from the cache
         * @param path File path of the resource to unload
         * 
         * @details Removes the resource from the cache. The resource will be destroyed
         * when all shared_ptr references are released (automatic via RAII).
         */
        void unload(const std::string& path);
        
        /**
         * @brief Clear all cached resources
         * 
         * @details Removes all resources from the cache. Actual memory cleanup occurs
         * when all external shared_ptr references are released.
         * 
         * @warning Use with caution - may cause resource reloading if resources are
         * still in use elsewhere in the application.
         */
        void clear();

    private:
        /**
         * @brief Internal resource cache
         * @details Maps file paths to shared pointers of resources
         */
        std::unordered_map<std::string, std::shared_ptr<T>> resources;
    };

    /**
     * @brief Type alias for texture resource management
     * @details Provides convenient access to texture-specific resource manager
     */
    using TextureManager = ResourceManager<class Texture>;
    
    /**
     * @brief Type alias for sound resource management
     * @details Provides convenient access to sound-specific resource manager
     */
    using SoundManager = ResourceManager<class Sound>;
    
    /**
     * @brief Type alias for font resource management
     * @details Provides convenient access to font-specific resource manager
     */
    using FontManager = ResourceManager<class Font>;
}

#endif // CLIENT_RESOURCE_MANAGER_HPP
