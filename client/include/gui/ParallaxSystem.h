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
            HallwayLevel2,
            ReactorLevel3
        };

        /**
         * @brief Map a game level index to a Parallax theme.
         *
         * Centralizes the mapping logic so callers don't duplicate it.
         * @param levelIndex 0 = level1 (SpaceDefault), 1 = level2 (HallwayLevel2), ...
         * @return Theme mapped theme for the given level index
         */
        static Theme themeFromLevel(int levelIndex);

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

        // Reactor (Level 3) overlays
        struct EnergyArc {
            std::vector<sf::Vector2f> points;
            float phase{0.f};
            float speed{2.0f};
        };
        struct SmokePlume {
            sf::Vector2f pos;
            sf::Vector2f vel;
            float size{30.f};
            float alpha{60.f};
        };
        std::vector<sf::CircleShape> m_reactorCores; // glowing cores
        std::vector<SmokePlume> m_reactorSmoke;
        std::vector<EnergyArc> m_energyArcs;
        float m_reactorBlend{0.0f}; // interpolation [0..1] for reactor overlays

        /**
         * @name Theme and hallway visual state
         *
         * These members control the visual theme selection and timed
         * transitions between themes. The system supports an immediate
         * theme change via setTheme(...) or a smooth timed transition
         * via transitionToTheme(...).
         *
         * Key fields:
         * - m_currentTheme: the theme currently considered "active".
         * - m_targetTheme: the theme we're transitioning to (may equal current).
         * - m_themeTransitionTimer / m_themeTransitionDuration: progress timer for transitions.
         * - m_themeBlend: normalized interpolation [0..1] used by renderers to blend visuals.
         */
        Theme m_currentTheme{Theme::SpaceDefault}; // active theme
        Theme m_targetTheme{Theme::SpaceDefault}; // target theme for transitions
        float m_themeTransitionTimer{0.0f}; // elapsed time in current transition
        float m_themeTransitionDuration{0.0f};
        float m_themeElapsed{0.0f}; // used for animated lights
        float m_themeBlend{0.0f}; // interpolation [0..1] between current and target theme

        /**
         * @name Hallway theme helpers
         *
         * Data and helpers used by the `HallwayLevel2` theme. The hallway
         * presents a tiled corridor with a central dark stripe, larger debris
         * and a fixed row of red flashing lights. The fields below control
         * layout and appearance; helper methods prepare and blend the
         * hallway parameters.
         *
         * Members:
         * - m_fixedRedLights: world-space positions for the evenly spaced red lights.
         * - m_hallwayStripeHeight: pixel height of the dark center stripe.
         * - m_hallwayDebrisScale: multiplier applied to debris sizes for the hallway.
         * - m_lightCount: number of fixed lights to lay out across the screen.
         */
        std::vector<sf::Vector2f> m_fixedRedLights; //!< fixed row of red flashing lights
        float m_hallwayStripeHeight{48.0f};         //!< height of the dark stripes on the hallway walls
        float m_hallwayDebrisScale{1.8f};           //!< scale factor for hallway debris
        int m_lightCount{8};                        //!< number of fixed red lights (lower = more separation)

        /**
         * @brief Prepare hallway-specific parameters (panels, stars, debris scale)
         *
         * Called when transitioning to or immediately setting the HallwayLevel2
         * theme. This function reinitializes star layers and debris sizes to
         * match the hallway aesthetic.
         */
        void initializeHallwayTheme();
    void initializeReactorTheme();

        /**
         * @brief Optional per-parameter blend hook
         * @param t Normalized transition progress in [0,1]
         *
         * Called during a timed transition to allow smooth interpolation of
         * visual parameters (colors, alpha, sizes). The current implementation
         * prepares the target state immediately and uses m_themeBlend in
         * rendering, but this hook can be extended to lerp individual values.
         */
        void blendThemes(float t);
        void updateReactor(float dt);
        void renderReactor(sf::RenderWindow& window);

        /**
         * @name Corridor scrolling and panel layer
         *
         * These members power the tiled corridor visuals that sit on the
         * background layer. A small horizontal offset is advanced every
         * frame to give the sensation of forward motion: both the panel
         * tiles and the fixed red lights are shifted left over time and
         * wrapped to maintain a continuous effect.
         */
        float m_corridorScrollSpeed{140.0f}; // pixels per second (tunable)
        float m_panelOffsetX{0.0f};          // current horizontal offset for panel grid
        float m_lightOffsetX{0.0f};          // current horizontal offset for fixed lights

        /**
         * @brief Panel tile layout used to draw corridor walls
         *
         * - m_panelPositions: top-left positions for each tile (precomputed grid)
         * - m_panelSize: size of a single panel tile in pixels
         * - m_panelDamaged: indices of panels that show damage marks
         * - m_pipePositions: positions of long pipes drawn on the corridor
         */
        std::vector<sf::Vector2f> m_panelPositions; //!< top-left for each panel tile
        sf::Vector2f m_panelSize{160.0f, 120.0f};    //!< default panel tile size
        std::vector<int> m_panelDamaged;             //!< indices of panels that have damage marks
        std::vector<sf::Vector2f> m_pipePositions;   //!< positions of thin pipes running across the corridor

        /**
         * @brief Build the tiled panel grid and precompute damage marks
         *
         * Populates m_panelPositions, m_panelDamageMarks and m_pipePositions.
         * Called during initialization of the hallway theme so rendering
         * can be deterministic and free of per-frame randomness.
         */
        void initializeHallwayPanels();

        /**
         * @brief Render the tiled panel layer
         * @param window Render target
         * @param blend Theme blend factor in [0..1]
         *
         * Draws the corridor tile grid, panel borders, damage marks and pipes.
         * The supplied blend parameter is used to modulate colors and alpha
         * during transitions between themes.
         */
        void renderPanelLayer(sf::RenderWindow& window, float blend);
        
        /**
         * @brief Stable per-panel damage marks
         *
         * Each panel may contain a small list of precomputed scratches/dents
         * that are drawn deterministically each frame. Storing these marks
         * prevents flicker and visual jitter caused by generating random
         * marks every frame.
         */
        struct PanelDamageMark {
            float x;     ///< local x coordinate (pixels) within the panel
            float y;     ///< local y coordinate (pixels) within the panel
            float len;   ///< length of the scratch in pixels
            float angle; ///< rotation in degrees
        };
        std::vector<std::vector<PanelDamageMark>> m_panelDamageMarks; //!< per-panel damage marks
        /** @} */
    };
    
} // namespace rtype::client::gui

#endif // CLIENT_PARALLAX_SYSTEM_HPP
