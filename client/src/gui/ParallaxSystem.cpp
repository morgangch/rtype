/**
 * @file ParallaxSystem.cpp
 * @brief Implementation of the ParallaxSystem class
 * 
 * This file implements the ParallaxSystem class methods for creating
 * and managing a multi-layered parallax scrolling background system.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/ParallaxSystem.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

namespace rtype::client::gui {

ParallaxSystem::ParallaxSystem(float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight) {
    // Seed random number generator
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    
    initializeGradientBackground();
    
    // Initialize star layers with different properties
    initializeStarLayer(m_farStars, 60, 20.0f, 0.5f, 1.5f, 
                       sf::Color(200, 200, 200, 120));
    
    initializeStarLayer(m_mediumStars, 40, 60.0f, 1.0f, 3.0f, 
                       sf::Color(220, 220, 220, 180));
    
    initializeStarLayer(m_nearStars, 25, 120.0f, 1.5f, 4.0f, 
                       sf::Color::White);
    
    initializeDebris();

    // Prepare a fixed row of red lights used by the hallway theme (evenly spaced)
    m_fixedRedLights.clear();
    float spacing = m_screenWidth / static_cast<float>(m_lightCount);
    // place the flashing red lights slightly higher above the center stripe
    float y = m_screenHeight * 0.38f;
    for (int i = 0; i < m_lightCount; i++) {
        m_fixedRedLights.emplace_back(spacing * (i + 0.5f), y);
    }
    m_themeBlend = 0.0f;

    // Initialize small corridor panels used in hallway theme
    initializeHallwayPanels();
}

void ParallaxSystem::setTheme(Theme theme, bool immediate) {
    if (immediate) {
        m_currentTheme = theme;
        m_targetTheme = theme;
        m_themeTransitionTimer = 0.0f;
        m_themeTransitionDuration = 0.0f;
        // Level 3 should visually reuse the hallway with a red variant
        if (theme == Theme::HallwayLevel2) {
            m_themeBlend = 1.0f;
            m_reactorBlend = 0.0f; // no reactor overlays for plain hallway
            initializeHallwayTheme();
        } else if (theme == Theme::ReactorLevel3) {
            // L3: hallway red variant, reactor overlays remain disabled
            m_themeBlend = 1.0f;
            m_reactorBlend = 0.0f;
            initializeHallwayTheme();
        } else if (theme == Theme::ReactorLevel4) {
            // L4: broken reactor variant - hallway with broken vertical bars
            m_themeBlend = 1.0f;
            m_reactorBlend = 0.0f; // keep reactor overlay disabled (no blue glows)
            initializeHallwayTheme();
            initializeReactorBrokenTheme();
        } else {
            m_themeBlend = 0.0f;
            m_reactorBlend = 0.0f;
            reset();
        }
    } else {
        transitionToTheme(theme, 1.0f);
    }
}

void ParallaxSystem::transitionToTheme(Theme theme, float duration) {
    m_targetTheme = theme;
    m_themeTransitionDuration = std::max(0.0001f, duration);
    m_themeTransitionTimer = 0.0f;
    // Prepare target theme parameters early. L3/L4 reuse hallway visuals.
    if (theme == Theme::HallwayLevel2 || theme == Theme::ReactorLevel3 || theme == Theme::ReactorLevel4) {
        initializeHallwayTheme();
        if (theme == Theme::ReactorLevel4) initializeReactorBrokenTheme();
    }
}

void ParallaxSystem::update(float deltaTime) {
    m_themeElapsed += deltaTime;
    updateStarLayer(m_farStars, deltaTime);
    updateStarLayer(m_mediumStars, deltaTime);
    updateStarLayer(m_nearStars, deltaTime);
    
    // Update space debris
    for (auto& debris : m_debris) {
        debris.position.x -= debris.speed * deltaTime;
        debris.rotation += debris.rotationSpeed * deltaTime;
        
        // Wrap around when debris exits screen
        if (debris.position.x < -debris.size.x - 20.0f) {
            debris.position.x = m_screenWidth + debris.size.x + 20.0f;
            debris.position.y = static_cast<float>(rand() % static_cast<int>(m_screenHeight));
        }
    }

    // Corridor scrolling (animate panel grid and lights to the left to simulate forward movement)
    if (m_themeBlend > 0.001f || m_currentTheme == Theme::HallwayLevel2 || m_targetTheme == Theme::HallwayLevel2) {
        // If already in hallway, use full speed; otherwise scale by blend so scrolling ramps in
        float blendFactor = (m_currentTheme == Theme::HallwayLevel2) ? 1.0f : m_themeBlend;
        float effectiveSpeed = m_corridorScrollSpeed * blendFactor;
        m_panelOffsetX -= effectiveSpeed * deltaTime;
        m_lightOffsetX -= effectiveSpeed * deltaTime;

        // Keep offsets within a bounded range for stable wrapping
        if (m_panelOffsetX <= -m_panelSize.x) m_panelOffsetX += m_panelSize.x;
        if (m_panelOffsetX >= m_panelSize.x) m_panelOffsetX -= m_panelSize.x;
        if (m_lightOffsetX <= -m_screenWidth) m_lightOffsetX += m_screenWidth;
        if (m_lightOffsetX >= m_screenWidth) m_lightOffsetX -= m_screenWidth;
    }

    // Theme blending / transition
    if (m_themeTransitionDuration > 0.0f) {
        m_themeTransitionTimer += deltaTime;
        float t = std::min(1.0f, m_themeTransitionTimer / m_themeTransitionDuration);
        m_themeBlend = t;
        // Reactor overlays disabled for L3 red hallway variant
    // Reactor overlays interpolated only when transitioning to ReactorLevel4
    if (m_targetTheme == Theme::ReactorLevel4) m_reactorBlend = t;
    else m_reactorBlend = 0.0f;
        blendThemes(t);
        if (t >= 1.0f) {
            m_currentTheme = m_targetTheme;
            m_themeTransitionDuration = 0.0f;
        }
    }
    // Vibration timer advance (for ReactorLevel4 subtle shake)
    if (m_currentTheme == Theme::ReactorLevel4 || m_targetTheme == Theme::ReactorLevel4) {
        m_vibrateTimer += deltaTime;
    }
}

ParallaxSystem::Theme ParallaxSystem::themeFromLevel(int levelIndex) {
    if (levelIndex <= 0) return Theme::SpaceDefault;
    if (levelIndex == 1) return Theme::HallwayLevel2;
    if (levelIndex == 2) return Theme::ReactorLevel3;
    if (levelIndex == 3) return Theme::ReactorLevel4;
    return Theme::SpaceDefault;
}

void ParallaxSystem::initializeHallwayPanels() {
    m_panelPositions.clear();
    m_panelDamaged.clear();
    m_pipePositions.clear();
    m_panelDamageMarks.clear();

    // Create a grid of panels spanning the screen, with a little overlap
    int cols = static_cast<int>(std::ceil(m_screenWidth / m_panelSize.x)) + 2;
    int rows = static_cast<int>(std::ceil(m_screenHeight / m_panelSize.y)) + 1;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float x = c * m_panelSize.x - m_panelSize.x * 0.5f;
            float y = r * m_panelSize.y - (m_panelSize.y * 0.15f * (c % 2)); // slight vertical staggering
            m_panelPositions.emplace_back(x, y);
            m_panelDamageMarks.emplace_back();
        }
    }

    // Randomly mark some panels as damaged (bullet holes / dents)
    int total = static_cast<int>(m_panelPositions.size());
    int damagedCount = std::max(3, total / 10);
    for (int i = 0; i < damagedCount; i++) {
        int idx = rand() % total;
        m_panelDamaged.push_back(idx);
        // Precompute a few damage marks for this panel so scratches do not change per-frame
        int dents = 2 + (rand() % 2);
        auto &marks = m_panelDamageMarks[idx];
        for (int d = 0; d < dents; ++d) {
            PanelDamageMark mark;
            mark.x = 8.0f + static_cast<float>(rand() % static_cast<int>(m_panelSize.x - 16.0f));
            mark.y = 8.0f + static_cast<float>(rand() % static_cast<int>(m_panelSize.y - 16.0f));
            mark.len = 6.0f + static_cast<float>(rand() % 12);
            mark.angle = static_cast<float>(rand() % 360);
            marks.push_back(mark);
        }
    }

    // Add a few long horizontal pipes near ceiling / floor
    m_pipePositions.emplace_back(0.0f, m_screenHeight * 0.18f);
    m_pipePositions.emplace_back(0.0f, m_screenHeight * 0.82f);
}

void ParallaxSystem::renderPanelLayer(sf::RenderWindow& window, float blend) {
    // Corridor panel base color (metallic grey) or red variant for L3
    bool redVariant = (m_currentTheme == Theme::ReactorLevel3) || (m_targetTheme == Theme::ReactorLevel3);
    sf::Color panelColor = redVariant ? sf::Color(100, 20, 20) : sf::Color(55, 55, 60);
    sf::Color panelEdge  = redVariant ? sf::Color(70, 10, 10)  : sf::Color(35, 35, 38);

    bool isL4 = (m_currentTheme == Theme::ReactorLevel4) || (m_targetTheme == Theme::ReactorLevel4);
    float vib = 0.0f;
    if (isL4 && m_vibrateAmplitude > 0.0f) {
        vib = std::sin(m_vibrateTimer * 30.0f) * m_vibrateAmplitude * m_themeBlend;
    }

    for (size_t i = 0; i < m_panelPositions.size(); i++) {
        const auto& pos = m_panelPositions[i];
        // If this panel is marked broken in L4, skip drawing it (missing piece)
        if (isL4 && std::find(m_brokenPanels.begin(), m_brokenPanels.end(), static_cast<int>(i)) != m_brokenPanels.end()) {
            continue;
        }
        sf::RectangleShape panel(m_panelSize);
    // Parallax offset: combine static parallax with the animated panel scroll
    float staticParallax = -m_themeBlend * 40.0f; // slight shift into the scene
    float x = pos.x + staticParallax + m_panelOffsetX + vib;
    // Wrap horizontally so the grid tiles continuously
    while (x < -m_panelSize.x) x += m_panelSize.x;
    while (x > m_screenWidth + m_panelSize.x) x -= m_panelSize.x;
    panel.setPosition(x, pos.y);
        // Make panel grid solid (opaque) instead of translucent
        panel.setFillColor(sf::Color(
            static_cast<sf::Uint8>(panelColor.r * (0.5f + 0.5f * blend)),
            static_cast<sf::Uint8>(panelColor.g * (0.5f + 0.5f * blend)),
            static_cast<sf::Uint8>(panelColor.b * (0.5f + 0.5f * blend)),
            static_cast<sf::Uint8>(255)));

        // draw panel body
        window.draw(panel);

        // draw panel border (thin)
        sf::RectangleShape border(m_panelSize);
        border.setPosition(panel.getPosition());
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(2.0f);
        // Solid border for the panel grid
        border.setOutlineColor(sf::Color(panelEdge.r, panelEdge.g, panelEdge.b, static_cast<sf::Uint8>(255)));
        window.draw(border);

        // if panel is marked damaged, draw small dents / holes
        bool damaged = false;
        for (int idx : m_panelDamaged) if (static_cast<size_t>(idx) == i) { damaged = true; break; }
        if (damaged) {
            // Draw precomputed damage marks (stable across frames) to avoid glitching
            auto &marks = m_panelDamageMarks[i];
            for (const auto &mark : marks) {
                sf::RectangleShape scratch(sf::Vector2f(mark.len, 2.0f));
                scratch.setOrigin(0.0f, 1.0f);
                // Mark positions stored relative to panel top-left
                float mx = panel.getPosition().x + mark.x;
                float my = panel.getPosition().y + mark.y;
                // Use integer positions to avoid subpixel shimmer
                scratch.setPosition(std::floor(mx + 0.5f), std::floor(my + 0.5f));
                scratch.setRotation(mark.angle);
                // slightly darker than panel with low alpha
                scratch.setFillColor(sf::Color(10, 10, 12, static_cast<sf::Uint8>(80 * blend)));
                window.draw(scratch);
            }
        }
    }

    // Draw pipes as long thin rounded-ish rectangles
    for (const auto& p : m_pipePositions) {
        sf::RectangleShape pipe(sf::Vector2f(m_screenWidth * 1.2f, 8.0f));
        pipe.setPosition(p.x - m_screenWidth * 0.1f, p.y);
        // Solid pipes
        if ((m_currentTheme == Theme::ReactorLevel3) || (m_targetTheme == Theme::ReactorLevel3) ||
            (m_currentTheme == Theme::ReactorLevel4) || (m_targetTheme == Theme::ReactorLevel4)) {
            pipe.setFillColor(sf::Color(90, 30, 30, static_cast<sf::Uint8>(255)));
        } else {
            pipe.setFillColor(sf::Color(80, 80, 85, static_cast<sf::Uint8>(255)));
        }
        window.draw(pipe);
        // small highlights
        sf::RectangleShape h(sf::Vector2f(m_screenWidth * 1.2f, 2.0f));
        h.setPosition(pipe.getPosition().x, pipe.getPosition().y + 1.5f);
        h.setFillColor(sf::Color(120, 120, 130, static_cast<sf::Uint8>(120)));
        window.draw(h);
    }
}

void ParallaxSystem::render(sf::RenderWindow& window) {
    // 1. Render gradient background
    window.draw(m_backgroundGradient);
    // Corridor panels sit at the same visual level as the background
    if (m_themeBlend > 0.01f) {
        renderPanelLayer(window, m_themeBlend);
    }
    
    // 2. Render star layers (back to front)
    renderStarLayer(window, m_farStars);
    renderStarLayer(window, m_mediumStars);
    renderStarLayer(window, m_nearStars);
    
    // If hallway is active (blend > 0) draw corridor accents
    if (m_themeBlend > 0.001f) {
        // For ReactorLevel3 we replace the central horizontal stripe and
        // fixed red lights with vertical glowing orange lines and a top
        // smoke overlay. For other hallway themes draw the original stripe
        // and red lights.
        bool isL3 = (m_currentTheme == Theme::ReactorLevel3) || (m_targetTheme == Theme::ReactorLevel3);

        if (!isL3) {
            // original horizontal stripe for hallway
            sf::RectangleShape stripe(sf::Vector2f(m_screenWidth, m_hallwayStripeHeight));
            stripe.setPosition(0.0f, (m_screenHeight - m_hallwayStripeHeight) * 0.5f);
            stripe.setFillColor(sf::Color(10, 10, 10, static_cast<sf::Uint8>(200.0f * m_themeBlend)));
            window.draw(stripe);

            // fixed red lights: size and alpha modulated by blend and a sin to flash
            for (size_t i = 0; i < m_fixedRedLights.size(); i++) {
                // Apply horizontal offset to lights so they move left; wrap across screen
                auto base = m_fixedRedLights[i];
                float lx = base.x + m_lightOffsetX;
                if (lx < 0.0f) lx += m_screenWidth;
                if (lx >= m_screenWidth) lx -= m_screenWidth;
                auto p = sf::Vector2f(lx, base.y);
                float baseRadius = 10.0f;
                float phase = static_cast<float>(i) / static_cast<float>(m_fixedRedLights.size());
                float alpha = 140.0f + 80.0f * std::sin(m_themeElapsed * 6.0f + phase * 6.28318f);
                alpha = std::clamp(alpha, 0.0f, 255.0f);
                sf::CircleShape light(baseRadius);
                light.setOrigin(baseRadius, baseRadius);
                light.setPosition(p);
                light.setFillColor(sf::Color(220, 30, 30, static_cast<sf::Uint8>(alpha * m_themeBlend)));
                // Draw soft glow behind the light
                sf::CircleShape glow(baseRadius * 2.2f);
                glow.setOrigin(glow.getRadius(), glow.getRadius());
                glow.setPosition(p);
                float glowAlpha = std::clamp(alpha * 0.5f * m_themeBlend, 0.0f, 255.0f);
                glow.setFillColor(sf::Color(220, 40, 40, static_cast<sf::Uint8>(glowAlpha)));
                window.draw(glow);
                window.draw(light);
            }
        } else {
            // Reactor L3/L4: draw vertical yellow glowing columns (split/broken for L4)
            const float separation = std::max(96.0f, m_panelSize.x * 0.5f);
            // start offset so lines move slightly with panel offset for motion
            float startX = std::fmod(m_panelOffsetX, separation);
            if (startX > 0.0f) startX -= separation;
            sf::RenderStates add; add.blendMode = sf::BlendAdd;

            bool isL4 = (m_currentTheme == Theme::ReactorLevel4) || (m_targetTheme == Theme::ReactorLevel4);
            // vibration offset when in L4
            float vib = 0.0f;
            if (isL4 && m_vibrateAmplitude > 0.0f) {
                vib = std::sin(m_vibrateTimer * 30.0f) * m_vibrateAmplitude * m_themeBlend;
            }

            int colIndex = 0;
            for (float x = startX; x < m_screenWidth + separation; x += separation, ++colIndex) {
                float xpos = x + vib;
                // choose base colors (yellow/orange)
                sf::Color baseColor(220, 180, 30, static_cast<sf::Uint8>(200.0f * m_themeBlend));
                sf::Color glowColor(255, 200, 80, static_cast<sf::Uint8>(120.0f * m_themeBlend));

                bool broken = (std::find(m_brokenColumns.begin(), m_brokenColumns.end(), colIndex) != m_brokenColumns.end());

                if (!broken) {
                    // Full column
                    sf::RectangleShape col(sf::Vector2f(6.0f, m_screenHeight));
                    col.setPosition(xpos, 0.0f);
                    col.setFillColor(baseColor);
                    window.draw(col);

                    // soft additive glow wider and more transparent
                    sf::RectangleShape glowCol(sf::Vector2f(30.0f, m_screenHeight));
                    glowCol.setPosition(xpos - 12.0f, 0.0f);
                    glowCol.setFillColor(glowColor);
                    window.draw(glowCol, add);
                } else {
                    // Broken: render as two halves (top + bottom) with a center gap
                    float gap = std::max(64.0f, m_panelSize.y * 0.5f);
                    float halfH = (m_screenHeight - gap) * 0.5f;

                    // Determine which halves to draw for a slightly varied broken look
                    bool drawTop = (colIndex % 2 == 0) || isL4;
                    bool drawBottom = (colIndex % 2 == 1) || isL4;

                    if (drawTop) {
                        sf::RectangleShape topCol(sf::Vector2f(6.0f, halfH));
                        topCol.setPosition(xpos, 0.0f);
                        topCol.setFillColor(baseColor);
                        window.draw(topCol);

                        sf::RectangleShape topGlow(sf::Vector2f(30.0f, halfH));
                        topGlow.setPosition(xpos - 12.0f, 0.0f);
                        topGlow.setFillColor(glowColor);
                        window.draw(topGlow, add);
                    }

                    if (drawBottom) {
                        sf::RectangleShape botCol(sf::Vector2f(6.0f, halfH));
                        botCol.setPosition(xpos, halfH + gap);
                        botCol.setFillColor(baseColor);
                        window.draw(botCol);

                        sf::RectangleShape botGlow(sf::Vector2f(30.0f, halfH));
                        botGlow.setPosition(xpos - 12.0f, halfH + gap);
                        botGlow.setFillColor(glowColor);
                        window.draw(botGlow, add);
                    }
                }
            }

            // Top smoke overlay (subtle drifting clouds at the top of the corridor)
            // Use theme elapsed to animate horizontal drift
            const int smokeCount = 10;
            for (int i = 0; i < smokeCount; ++i) {
                float phase = m_themeElapsed * 0.3f + static_cast<float>(i) * 0.7f;
                float sx = std::fmod((i * (m_screenWidth / static_cast<float>(smokeCount)) + std::sin(phase) * 40.0f + m_panelOffsetX), m_screenWidth);
                float width = 160.0f + 40.0f * std::sin(phase * 0.7f + i);
                float height = 48.0f + 12.0f * std::cos(phase * 0.5f + i);
                sf::RectangleShape puff(sf::Vector2f(width, height));
                puff.setOrigin(width * 0.5f, 0.0f);
                puff.setPosition(sx, 6.0f + std::sin(phase * 0.6f) * 10.0f);
                puff.setFillColor(sf::Color(180, 180, 180, static_cast<sf::Uint8>(60.0f * m_themeBlend)));
                window.draw(puff);
            }
        }
    }

    // 3. Reactor overlays (enabled for ReactorLevel4 via m_reactorBlend)
    if (m_reactorBlend > 0.001f) {
        renderReactor(window);
    }

    // 4. Render space debris (apply scale based on theme blend)
    if (m_themeBlend > 0.001f) {
        // when hallway active, debris are larger — interpolate size
        float scale = 1.0f + (m_hallwayDebrisScale - 1.0f) * m_themeBlend;
        for (const auto& debris : m_debris) {
            sf::Vector2f size = debris.size * scale;
            sf::RectangleShape debrisShape(size);
            debrisShape.setOrigin(size.x * 0.5f, size.y * 0.5f);
            debrisShape.setPosition(debris.position);
            debrisShape.setRotation(debris.rotation);
            debrisShape.setFillColor(debris.color);
            // glow
            sf::RectangleShape glow(sf::Vector2f(size.x + 4.0f, size.y + 4.0f));
            glow.setOrigin((size.x + 4.0f) * 0.5f, (size.y + 4.0f) * 0.5f);
            glow.setPosition(debris.position);
            glow.setRotation(debris.rotation);
            glow.setFillColor(sf::Color(debris.color.r, debris.color.g, debris.color.b, static_cast<sf::Uint8>(30.0f * m_themeBlend)));
            window.draw(glow);
            window.draw(debrisShape);
        }
    } else {
        renderDebris(window);
    }

    
}

void ParallaxSystem::initializeHallwayTheme() {
    // Darker, greyer background for hallway
    m_backgroundGradient[0].color = sf::Color(40, 40, 40);
    m_backgroundGradient[1].color = sf::Color(40, 40, 40);
    m_backgroundGradient[2].color = sf::Color(12, 12, 12);
    m_backgroundGradient[3].color = sf::Color(12, 12, 12);

    // Narrow/fewer stars (dimmer)
    initializeStarLayer(m_farStars, 30, 10.0f, 0.3f, 1.0f, sf::Color(100, 100, 100, 80));
    initializeStarLayer(m_mediumStars, 20, 20.0f, 0.6f, 1.5f, sf::Color(120, 120, 120, 120));
    initializeStarLayer(m_nearStars, 8, 40.0f, 1.0f, 2.0f, sf::Color(160, 160, 160, 160));

    // Enlarge debris base size to create larger foreground pieces
    for (auto& debris : m_debris) {
        debris.size.x *= m_hallwayDebrisScale;
        debris.size.y *= m_hallwayDebrisScale;
    }
}

void ParallaxSystem::blendThemes(float t) {
    (void)t;
}

void ParallaxSystem::reset() {
    // Reinitialize all layers
    initializeGradientBackground();
    
    initializeStarLayer(m_farStars, 60, 20.0f, 0.5f, 1.5f, 
                       sf::Color(200, 200, 200, 120));
    
    initializeStarLayer(m_mediumStars, 40, 60.0f, 1.0f, 3.0f, 
                       sf::Color(220, 220, 220, 180));
    
    initializeStarLayer(m_nearStars, 25, 120.0f, 1.5f, 4.0f, 
                       sf::Color::White);
    
    initializeDebris();

    // Reset theme to default space look
    m_currentTheme = Theme::SpaceDefault;
    m_targetTheme = Theme::SpaceDefault;
    m_themeBlend = 0.0f;
    m_reactorBlend = 0.0f;
    m_themeTransitionDuration = 0.0f;
    m_themeTransitionTimer = 0.0f;
    // Reset corridor scroll offsets
    m_panelOffsetX = 0.0f;
    m_lightOffsetX = 0.0f;
}

void ParallaxSystem::initializeGradientBackground() {
    m_backgroundGradient.setPrimitiveType(sf::Quads);
    m_backgroundGradient.resize(4);
    
    // Top vertices (dark blue)
    m_backgroundGradient[0].position = sf::Vector2f(0, 0);
    m_backgroundGradient[1].position = sf::Vector2f(m_screenWidth, 0);
    m_backgroundGradient[0].color = sf::Color(10, 20, 40);  // Dark blue
    m_backgroundGradient[1].color = sf::Color(10, 20, 40);
    
    // Bottom vertices (black)
    m_backgroundGradient[2].position = sf::Vector2f(m_screenWidth, m_screenHeight);
    m_backgroundGradient[3].position = sf::Vector2f(0, m_screenHeight);
    m_backgroundGradient[2].color = sf::Color::Black;
    m_backgroundGradient[3].color = sf::Color::Black;
}

void ParallaxSystem::initializeStarLayer(ParallaxLayer& layer, int particleCount, 
                                        float speed, float minSize, float maxSize, 
                                        const sf::Color& baseColor) {
    layer.speed = speed;
    layer.particleCount = particleCount;
    
    // Clear and reserve space
    layer.positions.clear();
    layer.sizes.clear();
    layer.colors.clear();
    layer.positions.reserve(particleCount);
    layer.sizes.reserve(particleCount);
    layer.colors.reserve(particleCount);
    
    for (int i = 0; i < particleCount; i++) {
        // Random position across screen width + buffer
        layer.positions.emplace_back(
            static_cast<float>(rand() % static_cast<int>(m_screenWidth + 200)),
            static_cast<float>(rand() % static_cast<int>(m_screenHeight))
        );
        
        // Random size within range
        float sizeRange = maxSize - minSize;
        layer.sizes.emplace_back(minSize + static_cast<float>(rand()) / RAND_MAX * sizeRange);
        
        // Color variation based on base color
        sf::Uint8 r = static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.r) + (rand() % 56) - 28));
        sf::Uint8 g = static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.g) + (rand() % 56) - 28));
        sf::Uint8 b = static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.b) + (rand() % 56) - 28));
        sf::Uint8 a = static_cast<sf::Uint8>(std::min(255, static_cast<int>(baseColor.a) + (rand() % 106) - 53));
        
        layer.colors.emplace_back(r, g, b, a);
    }
}

void ParallaxSystem::initializeDebris() {
    m_debris.clear();
    m_debris.reserve(8);
    
    for (int i = 0; i < 8; i++) {
        SpaceDebris debris;
        debris.position.x = static_cast<float>(rand() % static_cast<int>(m_screenWidth + 400));
        debris.position.y = static_cast<float>(rand() % static_cast<int>(m_screenHeight));
        debris.size.x = 8.0f + static_cast<float>(rand() % 20);
        debris.size.y = 6.0f + static_cast<float>(rand() % 15);
        debris.rotation = static_cast<float>(rand() % 360);
        debris.rotationSpeed = 10.0f + static_cast<float>(rand() % 30);
        debris.speed = 40.0f + static_cast<float>(rand() % 60);
        debris.color = sf::Color(100 + rand() % 80, 80 + rand() % 60, 70 + rand() % 50);
        m_debris.push_back(debris);
    }
}

void ParallaxSystem::updateStarLayer(ParallaxLayer& layer, float deltaTime) {
    for (auto& pos : layer.positions) {
        pos.x -= layer.speed * deltaTime;
        
        // Wrap around when star exits screen
        if (pos.x < -10.0f) {
            pos.x = m_screenWidth + 10.0f;
            pos.y = static_cast<float>(rand() % static_cast<int>(m_screenHeight));
        }
    }
}

void ParallaxSystem::renderStarLayer(sf::RenderWindow& window, const ParallaxLayer& layer) {
    for (size_t i = 0; i < layer.positions.size(); i++) {
        float radius = std::max(1.0f, layer.sizes[i]);
        sf::CircleShape star(radius);
        // Use integer positions to reduce sub-pixel jitter
        sf::Vector2f pos(std::floor(layer.positions[i].x + 0.5f), std::floor(layer.positions[i].y + 0.5f));
        star.setPosition(pos);
        star.setFillColor(layer.colors[i]);
        window.draw(star);
    }
}

void ParallaxSystem::renderDebris(sf::RenderWindow& window) {
    for (const auto& debris : m_debris) {
        // Render glow effect first (behind debris)
        sf::RectangleShape glow(sf::Vector2f(debris.size.x + 4.0f, debris.size.y + 4.0f));
        glow.setPosition(debris.position);
        glow.setOrigin((debris.size.x + 4.0f) * 0.5f, (debris.size.y + 4.0f) * 0.5f);
        glow.setRotation(debris.rotation);
        glow.setFillColor(sf::Color(debris.color.r, debris.color.g, debris.color.b, 30));
        window.draw(glow);
        
        // Render main debris shape
        sf::RectangleShape debrisShape(debris.size);
        debrisShape.setPosition(debris.position);
        debrisShape.setFillColor(debris.color);
        debrisShape.setOrigin(debris.size.x * 0.5f, debris.size.y * 0.5f);
        debrisShape.setRotation(debris.rotation);
        window.draw(debrisShape);
    }
}

void ParallaxSystem::initializeReactorTheme() {
    // Teal-dark background
    m_backgroundGradient[0].color = sf::Color(10, 22, 28);
    m_backgroundGradient[1].color = sf::Color(10, 22, 28);
    m_backgroundGradient[2].color = sf::Color(3, 8, 12);
    m_backgroundGradient[3].color = sf::Color(3, 8, 12);

    // Colder stars
    initializeStarLayer(m_farStars,   40, 15.0f, 0.5f, 1.2f, sf::Color(160, 220, 255, 100));
    initializeStarLayer(m_mediumStars,30, 45.0f, 0.8f, 2.5f, sf::Color(180, 240, 255, 160));
    initializeStarLayer(m_nearStars,  18, 90.0f, 1.2f, 3.0f, sf::Color(210, 255, 255, 220));

    // Reactor cores
    m_reactorCores.clear();
    const int coreCount = 3;
    for (int i = 0; i < coreCount; ++i) {
        float x = m_screenWidth * (0.25f + 0.25f * i);
        float y = m_screenHeight * (0.32f + 0.1f * (i % 2));
        sf::CircleShape core(40.f);
        core.setOrigin(40.f, 40.f);
        core.setPosition(x, y);
        core.setFillColor(sf::Color(0, 0, 0, 0));
        m_reactorCores.push_back(core);
    }

    // Smoke plumes
    m_reactorSmoke.clear();
    for (int i = 0; i < 14; ++i) {
        SmokePlume s;
        s.pos = { static_cast<float>(rand() % static_cast<int>(m_screenWidth)), m_screenHeight + static_cast<float>(rand() % 200) };
        s.vel = { (rand() % 20 - 10) * 0.4f, -30.0f - static_cast<float>(rand() % 30) };
        s.size = 30.0f + static_cast<float>(rand() % 40);
        s.alpha = 40.0f + static_cast<float>(rand() % 60);
        m_reactorSmoke.push_back(s);
    }

    // Energy arcs
    m_energyArcs.clear();
    for (int i = 0; i < 4; ++i) {
        EnergyArc arc;
        int segments = 8 + (rand() % 4);
        float x0 = m_screenWidth * (0.15f + 0.2f * i);
        float y0 = m_screenHeight * 0.18f;
        float x1 = x0 + 180.0f;
        float y1 = y0 + 60.0f;
        arc.points.reserve(segments + 1);
        for (int s = 0; s <= segments; ++s) {
            float t = static_cast<float>(s) / static_cast<float>(segments);
            float x = x0 + (x1 - x0) * t;
            float y = y0 + (y1 - y0) * t + std::sin(t * 12.0f) * 10.0f;
            arc.points.push_back({x, y});
        }
        arc.phase = static_cast<float>(rand() % 628) / 100.0f;
        arc.speed = 2.0f + static_cast<float>(rand() % 200) / 100.0f;
        m_energyArcs.push_back(arc);
    }
}

void ParallaxSystem::initializeReactorBrokenTheme() {
    // Start from the standard reactor theme
    initializeReactorTheme();

    // Mark some panels as broken / missing for the broken-reactor L4 look
    m_brokenPanels.clear();
    int totalPanels = static_cast<int>(m_panelPositions.size());
    int brokenCount = std::max(2, totalPanels / 8);
    for (int i = 0; i < brokenCount; ++i) {
        int idx = rand() % totalPanels;
        m_brokenPanels.push_back(idx);
    }

    // Remove reactor cores/arcs (no blue glows for broken reactor variant)
    m_reactorCores.clear();
    m_energyArcs.clear();
    m_reactorSmoke.clear();

    // Mark vertical columns that should appear broken for L4
    m_brokenColumns.clear();
    const float separation = std::max(96.0f, m_panelSize.x * 0.5f);
    int cols = static_cast<int>(std::ceil(m_screenWidth / separation)) + 2;
    int brokenCols = std::max(1, cols / 6);
    for (int i = 0; i < brokenCols; ++i) {
        int c = rand() % cols;
        m_brokenColumns.push_back(c);
    }

    // Enable a subtle vibration effect (amplitude in pixels)
    m_vibrateAmplitude = 2.5f + static_cast<float>(rand() % 6); // 2.5..8.5 px
    m_vibrateTimer = 0.0f;

    // Slightly darker background tint for L4
    m_backgroundGradient[0].color = sf::Color(8, 16, 18);
    m_backgroundGradient[1].color = sf::Color(8, 16, 18);
    m_backgroundGradient[2].color = sf::Color(2, 6, 8);
    m_backgroundGradient[3].color = sf::Color(2, 6, 8);
}

void ParallaxSystem::updateReactor(float dt) {
    for (auto &s : m_reactorSmoke) {
        s.pos += s.vel * dt;
        s.alpha = std::min(255.0f, s.alpha + 10.0f * dt);
        if (s.pos.y + s.size < -20.0f) {
            s.pos.y = m_screenHeight + 40.0f;
            s.pos.x = static_cast<float>(rand() % static_cast<int>(m_screenWidth));
            s.alpha = 40.0f;
        }
    }
    for (auto &arc : m_energyArcs) {
        arc.phase += arc.speed * dt;
    }
}

void ParallaxSystem::renderReactor(sf::RenderWindow& window) {
    sf::RenderStates add; add.blendMode = sf::BlendAdd;
    // glows
    for (const auto &core : m_reactorCores) {
        sf::Vector2f p = core.getPosition();
        auto drawGlow = [&](float r, sf::Color c) {
            sf::CircleShape glow(r);
            glow.setOrigin(r, r);
            glow.setPosition(p);
            c.a = static_cast<sf::Uint8>(std::clamp(180.0f * m_reactorBlend, 0.0f, 255.0f));
            glow.setFillColor(c);
            window.draw(glow, add);
        };
        drawGlow(80.f, sf::Color(20, 220, 255));
        drawGlow(55.f, sf::Color(50, 255, 220));
        drawGlow(35.f, sf::Color(180, 255, 240));
    }
    // smoke
    for (const auto &s : m_reactorSmoke) {
        sf::RectangleShape r({s.size * 2.0f, s.size});
        r.setOrigin(r.getSize() * 0.5f);
        r.setPosition(s.pos);
        r.setRotation(12.0f);
        r.setFillColor(sf::Color(180, 180, 180, static_cast<sf::Uint8>(s.alpha * m_reactorBlend)));
        window.draw(r);
    }
    // arcs
    for (const auto &arc : m_energyArcs) {
        if (arc.points.size() < 2) continue;
        sf::VertexArray line(sf::LineStrip, arc.points.size());
        for (size_t i = 0; i < arc.points.size(); ++i) {
            float wobble = std::sin(arc.phase + static_cast<float>(i) * 0.35f) * 3.0f;
            sf::Vector2f p = arc.points[i] + sf::Vector2f(0.0f, wobble);
            sf::Color c = sf::Color(100, 240, 255, static_cast<sf::Uint8>(180.0f * m_reactorBlend));
            line[i].position = p; line[i].color = c;
        }
        window.draw(line, add);
        sf::VertexArray glow(sf::LineStrip, arc.points.size());
        for (size_t i = 0; i < arc.points.size(); ++i) {
            float wobble = std::sin(arc.phase + static_cast<float>(i) * 0.35f + 1.57f) * 6.0f;
            sf::Vector2f p = arc.points[i] + sf::Vector2f(0.0f, wobble);
            sf::Color c = sf::Color(0, 255, 200, static_cast<sf::Uint8>(120.0f * m_reactorBlend));
            glow[i].position = p; glow[i].color = c;
        }
        window.draw(glow, add);
    }
}

} // namespace rtype::client::gui
