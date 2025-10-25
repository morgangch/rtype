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

        /**
         * @brief Parallax visual theme selection
         *
         * Allows switching between predefined visual themes (space, hallway, etc.)
         * and supports smooth transitions which can be used for level changes.
         */
        enum class Theme {
            SpaceDefault,
            HallwayLevel2
        };

        /**
         * @brief Immediately set the current theme or schedule a transition
         * @param theme Target theme
         * @param immediate If true apply immediately, otherwise start a transition
         */
        void setTheme(Theme theme, bool immediate = true);

        /**
         * @brief Start a timed transition to the specified theme
         * @param theme Target theme
         * @param duration Transition duration in seconds
         */
        void transitionToTheme(Theme theme, float duration);
        
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
        
        /**
         * @name Screen Configuration
         * @brief Core screen dimensions for the parallax system
         * @{
         */
        
        /** @brief Screen width in pixels - defines the horizontal bounds for parallax rendering */
        float m_screenWidth;
        
        /** @brief Screen height in pixels - defines the vertical bounds for parallax rendering */
        float m_screenHeight;
        
        /** @} */
        
        /**
         * @name Background Rendering
         * @brief Background gradient system for space atmosphere
         * @{
         */
        
        /** 
         * @brief Gradient background vertex array
         * 
         * SFML VertexArray containing a quad with color interpolation from dark blue
         * at the top to black at the bottom, creating a deep space atmosphere effect.
         */
        sf::VertexArray m_backgroundGradient;
        
        /** @} */
        
        /**
         * @name Parallax Star Layers
         * @brief Multi-layered star system ordered from far to near
         * 
         * The three star layers create depth perception through different movement speeds:
         * - Far stars: Slowest movement (20 px/s), smallest size, dimmest
         * - Medium stars: Medium movement (60 px/s), medium size, moderate brightness  
         * - Near stars: Fastest movement (120 px/s), largest size, brightest
         * @{
         */
        
        /** @brief Distant, slow-moving background stars (20 px/s) */
        ParallaxLayer m_farStars;
        
        /** @brief Medium distance stars with moderate speed (60 px/s) */
        ParallaxLayer m_mediumStars;
        
        /** @brief Close, fast-moving foreground stars (120 px/s) */
        ParallaxLayer m_nearStars;
        
        /** @} */
        
        /**
         * @name Foreground Elements
         * @brief Dynamic space debris for enhanced visual depth
         * @{
         */
        
        /** 
         * @brief Rotating space debris objects
         * 
         * Vector of SpaceDebris objects that provide foreground visual elements.
         * Each debris piece has individual rotation, size, color, and movement speed
         * to create varied and dynamic space environment effects.
         */
        std::vector<SpaceDebris> m_debris;

        /**
         * @brief Theme / transition state
         */
        Theme m_currentTheme{Theme::SpaceDefault};
        Theme m_targetTheme{Theme::SpaceDefault};
        float m_themeTransitionTimer{0.0f};
        float m_themeTransitionDuration{0.0f};
        float m_themeElapsed{0.0f}; // used for animated lights
        float m_themeBlend{0.0f}; // interpolation [0..1] between current and target theme

        // Hallway-specific helpers
        std::vector<sf::Vector2f> m_fixedRedLights; //!< fixed row of red flashing lights
        float m_hallwayStripeHeight{48.0f};         //!< height of the dark stripes on the hallway walls
        float m_hallwayDebrisScale{1.8f};           //!< scale factor for hallway debris
        int m_lightCount{8};                        //!< number of fixed red lights (lower = more separation)

        // Theme helpers
        void initializeHallwayTheme();
        void blendThemes(float t);

        // Corridor scrolling state (used to animate the panel grid and lights)
        float m_corridorScrollSpeed{140.0f}; // pixels per second (tunable)
        float m_panelOffsetX{0.0f};          // current horizontal offset for panel grid
        float m_lightOffsetX{0.0f};          // current horizontal offset for fixed lights

        // Corridor/panel layer used to make the hallway look like R-Type interior
        std::vector<sf::Vector2f> m_panelPositions; //!< top-left for each panel tile
        sf::Vector2f m_panelSize{160.0f, 120.0f};    //!< default panel tile size
        std::vector<int> m_panelDamaged;             //!< indices of panels that have damage marks
        std::vector<sf::Vector2f> m_pipePositions;   //!< positions of thin pipes running across the corridor

        void initializeHallwayPanels();
        void renderPanelLayer(sf::RenderWindow& window, float blend);
        
        /**
         * @brief Precomputed damage marks for panels to avoid per-frame randomness
         */
        struct PanelDamageMark {
            float x;    // local x within panel
            float y;    // local y within panel
            float len;  // length of the scratch
            float angle; // rotation degrees
        };
        std::vector<std::vector<PanelDamageMark>> m_panelDamageMarks; //!< per-panel damage marks
        /** @} */
    };
    
} // namespace rtype::client::gui

#endif // CLIENT_PARALLAX_SYSTEM_HPP
