/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ShieldVisual - Visual component for shield effect
*/

#ifndef CLIENT_SHIELD_VISUAL_H
#define CLIENT_SHIELD_VISUAL_H

#include <ECS/ECS.h>
#include <SFML/Graphics.hpp>

namespace rtype::client::components {
    /**
     * @brief Visual component to display a shield around an entity
     * 
     * Displays a semi-transparent colored circle around the entity
     * to indicate protection (e.g., for Shielded enemies)
     */
    class ShieldVisual : public ECS::Component<ShieldVisual> {
    public:
        float radius;           ///< Shield circle radius
        sf::Color color;        ///< Shield color (with alpha for transparency)
        float pulseSpeed;       ///< Speed of pulsing animation (0 = no pulse)
        float pulseTimer;       ///< Internal timer for pulse animation
        float thickness;        ///< Border thickness
        bool enabled;           ///< Is shield active/visible?

        ShieldVisual(float radius = 50.0f, 
                    sf::Color color = sf::Color(100, 150, 255, 100),
                    float pulseSpeed = 2.0f,
                    float thickness = 3.0f)
            : radius(radius), 
              color(color), 
              pulseSpeed(pulseSpeed), 
              pulseTimer(0.0f),
              thickness(thickness),
              enabled(true) {}
    };
}

#endif // CLIENT_SHIELD_VISUAL_H
