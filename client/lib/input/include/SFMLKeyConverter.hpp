#pragma once

#include "Keys.hpp"

#ifdef RTYPE_USE_SFML
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#endif

namespace rtype::client::input {
    
    /**
     * @brief Conversion utilities between SFML and our input enums
     */
    class SFMLKeyConverter {
    public:
#ifdef RTYPE_USE_SFML
        static Key SFMLToKey(sf::Keyboard::Key sfmlKey);
        static MouseButton SFMLToMouseButton(sf::Mouse::Button sfmlButton);
#endif
    };
}