#ifndef CLIENT_GUI_KEYTOSTRING_H
#define CLIENT_GUI_KEYTOSTRING_H

#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace rtype::client::gui {

std::string keyToString(sf::Keyboard::Key key);

} // namespace rtype::client::gui

#endif // CLIENT_GUI_KEYTOSTRING_H
