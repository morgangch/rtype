/**
 * @file SettingsState.cpp
 * @brief Implementation of the settings menu state for the R-TYPE client
 *
 * This file contains the implementation of the SettingsState class, which provides
 * the settings interface for the R-TYPE game. It allows users to input IP address
 * and port, and navigate back to the main menu.
 *
 * Features:
 * - Title display ("Settings")
 * - IP and Port input fields with placeholder text and character limits
 * - Real-time text input with cursor animation
 * - Responsive layout and hover effects
 * - Return button to go back to the main menu
 *
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/SettingsState.h"
#include "gui/MainMenuState.h"
#include "gui/GUIHelper.h"
#include <memory>

namespace rtype::client::gui {

/**
 * @class SettingsState
 * @brief Implements the settings menu logic for the R-TYPE client
 *
 * Handles user input for IP and port fields, manages UI layout and animations,
 * and processes navigation back to the main menu.
 */

SettingsState::SettingsState(StateManager& stateManager)
    : stateManager(stateManager), typingBox1(false), typingBox2(false), cursorTimer(0.0f), showCursor(true)
{
    const sf::Font& font = GUIHelper::getFont();

    titleText.setFont(font);
    titleText.setString("Settings");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);

    box1Rect.setSize(sf::Vector2f(300, 50));
    box1Rect.setFillColor(sf::Color(40, 40, 40));
    box1Rect.setOutlineColor(sf::Color::White);
    box1Rect.setOutlineThickness(2.0f);

    box2Rect.setSize(sf::Vector2f(300, 50));
    box2Rect.setFillColor(sf::Color(40, 40, 40));
    box2Rect.setOutlineColor(sf::Color::White);
    box2Rect.setOutlineThickness(2.0f);

    box1Text.setFont(font);
    box1Text.setCharacterSize(28);
    box1Text.setFillColor(sf::Color::White);

    box2Text.setFont(font);
    box2Text.setCharacterSize(28);
    box2Text.setFillColor(sf::Color::White);

    box1Hint.setFont(font);
    box1Hint.setString("Change IP");
    box1Hint.setCharacterSize(18);
    box1Hint.setFillColor(sf::Color(180,180,180));

    box2Hint.setFont(font);
    box2Hint.setString("Change PORT");
    box2Hint.setCharacterSize(18);
    box2Hint.setFillColor(sf::Color(180,180,180));

    // Return button setup
    GUIHelper::setupReturnButton(returnButton, returnButtonRect);
}

/**
 * @brief Called when entering the settings state. Sets up UI element positions.
 */
void SettingsState::onEnter() {
    float centerX = 640.0f, centerY = 360.0f;
    titleText.setPosition(centerX - titleText.getLocalBounds().width/2, 100.0f);

    box1Rect.setPosition(centerX - 150, centerY - 40);
    box2Rect.setPosition(centerX - 150, centerY + 40);

    box1Hint.setPosition(box1Rect.getPosition().x + 10, box1Rect.getPosition().y + 15);
    box2Hint.setPosition(box2Rect.getPosition().x + 10, box2Rect.getPosition().y + 15);

    // Return button positioning (top left)
    float returnButtonWidth = 150.0f;
    float returnButtonHeight = 50.0f;
    returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
    returnButtonRect.setPosition(20.0f, 20.0f);
    GUIHelper::centerText(returnButton,
        returnButtonRect.getPosition().x + returnButtonWidth / 2,
        returnButtonRect.getPosition().y + returnButtonHeight / 2);
}

/**
 * @brief Called when exiting the settings state.
 */
void SettingsState::onExit() {}

/**
 * @brief Handles SFML events for input and navigation in the settings menu.
 * @param event The SFML event to process
 */
void SettingsState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        // Check return button click
        if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            return;
        }
        typingBox1 = box1Rect.getGlobalBounds().contains(mousePos);
        typingBox2 = box2Rect.getGlobalBounds().contains(mousePos);
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
    }
    if (event.type == sf::Event::TextEntered) {
        if (typingBox1) {
            if (event.text.unicode == 8 && !box1Value.empty()) box1Value.pop_back();
            else if (event.text.unicode >= 32 && event.text.unicode < 127 && box1Value.size() < 9)
                box1Value += static_cast<char>(event.text.unicode);
        }
        if (typingBox2) {
            if (event.text.unicode == 8 && !box2Value.empty()) box2Value.pop_back();
            else if (event.text.unicode >= 32 && event.text.unicode < 127 && box2Value.size() < 4)
                box2Value += static_cast<char>(event.text.unicode);
        }
    }
}

/**
 * @brief Updates cursor animation and button hover effects.
 * @param deltaTime Time elapsed since last update
 */
void SettingsState::update(float deltaTime) {
    cursorTimer += deltaTime;
    if (cursorTimer >= 0.5f) {
        showCursor = !showCursor;
        cursorTimer = 0.0f;
    }
    box1Text.setString(box1Value + (showCursor && typingBox1 ? "|" : ""));
    box2Text.setString(box2Value + (showCursor && typingBox2 ? "|" : ""));
    box1Text.setPosition(box1Rect.getPosition().x + 10, box1Rect.getPosition().y + 15);
    box2Text.setPosition(box2Rect.getPosition().x + 10, box2Rect.getPosition().y + 15);

    // Button hover effect for return button
    sf::Vector2i mousePos = sf::Mouse::getPosition();
    GUIHelper::applyButtonHover(returnButtonRect, returnButton,
        GUIHelper::isPointInRect(sf::Vector2f(mousePos.x, mousePos.y), returnButtonRect),
        GUIHelper::Colors::RETURN_BUTTON, sf::Color(150, 70, 70, 200));
}

/**
 * @brief Renders the settings menu UI to the window.
 * @param window The render window to draw to
 */
void SettingsState::render(sf::RenderWindow& window) {
    window.draw(titleText);

    window.draw(box1Rect);
    if (box1Value.empty() && !typingBox1)
        window.draw(box1Hint);
    else
        window.draw(box1Text);

    window.draw(box2Rect);
    if (box2Value.empty() && !typingBox2)
        window.draw(box2Hint);
    else
        window.draw(box2Text);

    // Draw return button
    window.draw(returnButtonRect);
    window.draw(returnButton);
}

} // namespace rtype::client::gui
