/**
 * @file ParallaxSystem.h
 * @brief Parallax background system for space-themed games
 * 
 * This file contains the ParallaxSystem class which manages multiple layers
 * of moving background elements to create a realistic parallax scrolling effect.
 * The system includes gradient backgrounds, multiple star layers, and space debris.
 * 
 * Key features:
 * - Multi-layer star system with different speeds
 * - Gradient space background
 * - Rotating space debris with glow effects
 * - Smooth wrapping and continuous scrolling
 * - Performance optimized rendering
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_PARALLAX_SYSTEM_HPP
#define CLIENT_PARALLAX_SYSTEM_HPP

#include <SFML/Graphics.hpp>
#include <vector>

namespace rtype::client::gui {
    
    /**
     * @class ParallaxSystem
     * @brief Manages a multi-layered parallax scrolling background system
     * 
     * The ParallaxSystem creates an immersive space environment with multiple
     * layers of moving elements that scroll at different speeds to create
     * depth perception. Perfect for space-themed games like R-TYPE.
     * 
     * Layers (back to front):
     * 1. Gradient background (dark blue to black)
     * 2. Far stars (slow, small, dim)
     * 3. Medium stars (medium speed and size)
     * 4. Near stars (fast, large, bright)
     * 5. Space debris (rotating foreground elements)
     */
    class ParallaxSystem {
    public:
        /**
         * @brief Construct a new ParallaxSystem
         * @param screenWidth Width of the screen in pixels
         * @param screenHeight Height of the screen in pixels
         */
        ParallaxSystem(float screenWidth, float screenHeight);
        
        /**
         * @brief Update all parallax layers
         * @param deltaTime Time elapsed since last frame in seconds
         * 
         * Updates positions of all stars and debris, handles wrapping
         * when elements move off-screen, and manages rotation animations.
         */
        void update(float deltaTime);
        
        /**
         * @brief Render the complete parallax background
         * @param window The render window to draw to
         * 
         * Renders all layers in the correct order: gradient background,
         * star layers (far to near), and space debris.
         */
        void render(sf::RenderWindow& window);
        
        /**
         * @brief Reset the parallax system to initial state
         * 
         * Reinitializes all layers with new random positions and properties.
         * Useful when starting a new game or level.
         */
        void reset();
        
    private:
        /**
         * @brief Parallax layer structure for star systems
         * 
         * Contains all data needed for a single parallax layer including
         * particle positions, visual properties, and movement speed.
         */
        struct ParallaxLayer {
            std::vector<sf::Vector2f> positions;  ///< Positions of all particles
            std::vector<float> sizes;             ///< Size of each particle
            std::vector<sf::Color> colors;        ///< Color of each particle
            float speed;                          ///< Movement speed in pixels/second
            int particleCount;                    ///< Total number of particles
        };
        
        /**
         * @brief Space debris structure for foreground elements
         * 
         * Represents larger, rotating objects that add visual interest
         * and depth to the parallax background.
         */
        struct SpaceDebris {
            sf::Vector2f position;      ///< Current position
            sf::Vector2f size;          ///< Width and height
            float rotation;             ///< Current rotation angle
            float rotationSpeed;        ///< Rotation speed in degrees/second
            float speed;                ///< Movement speed in pixels/second
            sf::Color color;            ///< Debris color
        };
        
        /**
         * @brief Initialize the gradient background
         * 
         * Creates a vertex array for a smooth gradient from dark blue
         * at the top to black at the bottom.
         */
        void initializeGradientBackground();
        
        /**
         * @brief Initialize a star layer with random properties
         * @param layer The parallax layer to initialize
         * @param particleCount Number of stars to create
         * @param speed Movement speed in pixels per second
         * @param minSize Minimum star size
         * @param maxSize Maximum star size
         * @param baseColor Base color for stars (with random variation)
         */
        void initializeStarLayer(ParallaxLayer& layer, int particleCount, 
                                float speed, float minSize, float maxSize, 
                                const sf::Color& baseColor);
        
        /**
         * @brief Initialize space debris
         * 
         * Creates rotating debris objects with random sizes, positions,
         * and movement properties.
         */
        void initializeDebris();
        
        /**
         * @brief Update a specific star layer
         * @param layer The parallax layer to update
         * @param deltaTime Time elapsed since last frame
         */
        void updateStarLayer(ParallaxLayer& layer, float deltaTime);
        
        /**
         * @brief Render a specific star layer
         * @param window The render window to draw to
         * @param layer The parallax layer to render
         */
        void renderStarLayer(sf::RenderWindow& window, const ParallaxLayer& layer);
        
        /**
         * @brief Render space debris with glow effects
         * @param window The render window to draw to
         */
        void renderDebris(sf::RenderWindow& window);
        
        // Screen dimensions
        float m_screenWidth;   ///< Screen width in pixels
        float m_screenHeight;  ///< Screen height in pixels
        
        // Background gradient
        sf::VertexArray m_backgroundGradient;  ///< Gradient background vertex array
        
        // Parallax layers (far to near)
        ParallaxLayer m_farStars;     ///< Distant, slow-moving stars
        ParallaxLayer m_mediumStars;  ///< Medium distance stars
        ParallaxLayer m_nearStars;    ///< Close, fast-moving stars
        
        // Foreground elements
        std::vector<SpaceDebris> m_debris;  ///< Rotating space debris
    };
    
} // namespace rtype::client::gui

#endif // CLIENT_PARALLAX_SYSTEM_HPP
