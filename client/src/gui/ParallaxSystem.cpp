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
}

void ParallaxSystem::update(float deltaTime) {
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
}

void ParallaxSystem::render(sf::RenderWindow& window) {
    // 1. Render gradient background
    window.draw(m_backgroundGradient);
    
    // 2. Render star layers (back to front)
    renderStarLayer(window, m_farStars);
    renderStarLayer(window, m_mediumStars);
    renderStarLayer(window, m_nearStars);
    
    // 3. Render space debris
    renderDebris(window);
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
    for (size_t i = 0; i < layer.positions.size(); ++i) {
        sf::CircleShape star(layer.sizes[i]);
        star.setPosition(layer.positions[i]);
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

} // namespace rtype::client::gui
