#include "SFMLKeyConverter.hpp"

namespace rtype::client::input {
    
#ifdef RTYPE_USE_SFML
    
    Key SFMLKeyConverter::SFMLToKey(sf::Keyboard::Key sfmlKey) {
        switch (sfmlKey) {
            // Alphabet
            case sf::Keyboard::A: return Key::A;
            case sf::Keyboard::B: return Key::B;
            case sf::Keyboard::C: return Key::C;
            case sf::Keyboard::D: return Key::D;
            case sf::Keyboard::E: return Key::E;
            case sf::Keyboard::F: return Key::F;
            case sf::Keyboard::G: return Key::G;
            case sf::Keyboard::H: return Key::H;
            case sf::Keyboard::I: return Key::I;
            case sf::Keyboard::J: return Key::J;
            case sf::Keyboard::K: return Key::K;
            case sf::Keyboard::L: return Key::L;
            case sf::Keyboard::M: return Key::M;
            case sf::Keyboard::N: return Key::N;
            case sf::Keyboard::O: return Key::O;
            case sf::Keyboard::P: return Key::P;
            case sf::Keyboard::Q: return Key::Q;
            case sf::Keyboard::R: return Key::R;
            case sf::Keyboard::S: return Key::S;
            case sf::Keyboard::T: return Key::T;
            case sf::Keyboard::U: return Key::U;
            case sf::Keyboard::V: return Key::V;
            case sf::Keyboard::W: return Key::W;
            case sf::Keyboard::X: return Key::X;
            case sf::Keyboard::Y: return Key::Y;
            case sf::Keyboard::Z: return Key::Z;
            
            // Numbers
            case sf::Keyboard::Num0: return Key::Num0;
            case sf::Keyboard::Num1: return Key::Num1;
            case sf::Keyboard::Num2: return Key::Num2;
            case sf::Keyboard::Num3: return Key::Num3;
            case sf::Keyboard::Num4: return Key::Num4;
            case sf::Keyboard::Num5: return Key::Num5;
            case sf::Keyboard::Num6: return Key::Num6;
            case sf::Keyboard::Num7: return Key::Num7;
            case sf::Keyboard::Num8: return Key::Num8;
            case sf::Keyboard::Num9: return Key::Num9;
            
            // Function keys
            case sf::Keyboard::F1: return Key::F1;
            case sf::Keyboard::F2: return Key::F2;
            case sf::Keyboard::F3: return Key::F3;
            case sf::Keyboard::F4: return Key::F4;
            case sf::Keyboard::F5: return Key::F5;
            case sf::Keyboard::F6: return Key::F6;
            case sf::Keyboard::F7: return Key::F7;
            case sf::Keyboard::F8: return Key::F8;
            case sf::Keyboard::F9: return Key::F9;
            case sf::Keyboard::F10: return Key::F10;
            case sf::Keyboard::F11: return Key::F11;
            case sf::Keyboard::F12: return Key::F12;
            
            // Arrow keys
            case sf::Keyboard::Left: return Key::Left;
            case sf::Keyboard::Right: return Key::Right;
            case sf::Keyboard::Up: return Key::Up;
            case sf::Keyboard::Down: return Key::Down;
            
            // Common keys
            case sf::Keyboard::Space: return Key::Space;
            case sf::Keyboard::Enter: return Key::Enter;
            case sf::Keyboard::Escape: return Key::Escape;
            case sf::Keyboard::Tab: return Key::Tab;
            case sf::Keyboard::Backspace: return Key::Backspace;
            case sf::Keyboard::Delete: return Key::Delete;
            case sf::Keyboard::LShift: return Key::LShift;
            case sf::Keyboard::RShift: return Key::RShift;
            case sf::Keyboard::LControl: return Key::LCtrl;
            case sf::Keyboard::RControl: return Key::RCtrl;
            case sf::Keyboard::LAlt: return Key::LAlt;
            case sf::Keyboard::RAlt: return Key::RAlt;
            
            default: return Key::Count; // Invalid key
        }
    }
    
    MouseButton SFMLKeyConverter::SFMLToMouseButton(sf::Mouse::Button sfmlButton) {
        switch (sfmlButton) {
            case sf::Mouse::Left: return MouseButton::Left;
            case sf::Mouse::Right: return MouseButton::Right;
            case sf::Mouse::Middle: return MouseButton::Middle;
            default: return MouseButton::Count; // Invalid button
        }
    }
    
#endif
}