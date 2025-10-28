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
#include "gui/SettingsConfig.h"
#include "gui/KeyToString.h"
#include "gui/MainMenuState.h"
#include "gui/GUIHelper.h"
#include "gui/ParallaxSystem.h"
#include "gui/GameState.h"
#include "gui/Accessibility.h"
#include <memory>
#include "gui/AssetPaths.h"

namespace rtype::client::gui {

/**
 * @class SettingsState
 * @brief Implements the settings menu logic for the R-TYPE client
 *
 * Handles user input for IP and port fields, manages UI layout and animations,
 * and processes navigation back to the main menu.
 */

/**
 * @brief Construct a new SettingsState object
 * @param stateManager Reference to the state manager for handling state transitions
 * 
 * Initializes all UI elements including:
 * - Settings title and input fields for IP/Port
 * - Keybinds section with editable key assignments
 * - Return button for navigation
 * - Loads existing settings from config.json if available
 */
SettingsState::SettingsState(StateManager& stateManager)
    : stateManager(stateManager), typingBox1(false), typingBox2(false), cursorTimer(0.0f), showCursor(true), editingKeybind(-1)
{
    const sf::Font& font = GUIHelper::getFont();

    // Load settings from config file
    config.load();

    // Load IP and Port from config
    box1Value = config.getIP();
    box2Value = config.getPort();

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

    // Return button sprite setup
    returnSpriteLoaded = returnTexture.loadFromFile(rtype::client::assets::ui::RETURN_BUTTON);
    if (returnSpriteLoaded) {
        returnTexture.setSmooth(true);
        returnSprite.setTexture(returnTexture);
        // Center origin for clean centering in rect
        sf::Vector2u sz = returnTexture.getSize();
        returnSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
    } else {
        // Fallback to legacy text button if texture missing
        GUIHelper::setupReturnButton(returnButton, returnButtonRect);
    }

    // Keybinds setup
    keybindTitleText.setFont(font);
    keybindTitleText.setString("Keybinds");
    keybindTitleText.setCharacterSize(32);
    keybindTitleText.setFillColor(sf::Color::White);
    const char* actionNames[] = {"Up", "Down", "Left", "Right", "Shoot"};
    const char* actionKeys[] = {"up", "down", "left", "right", "shoot"};
    
    // Load keybinds from config
    for (int i = 0; i < KeybindCount; i++) {
        keybindActionTexts[i].setFont(font);
        keybindActionTexts[i].setString(actionNames[i]);
        keybindActionTexts[i].setCharacterSize(22);
        keybindActionTexts[i].setFillColor(sf::Color::White);
        keybindKeyTexts[i].setFont(font);
        keybindKeyTexts[i].setCharacterSize(22);
        keybindKeyTexts[i].setFillColor(sf::Color(180,180,180));
        
        // Load from config
        keybinds[i] = config.getKeybind(actionKeys[i]);
        
        keybindRects[i].setSize(sf::Vector2f(120, 36));
        keybindRects[i].setFillColor(sf::Color(40, 40, 40));
        keybindRects[i].setOutlineColor(sf::Color::White);
        keybindRects[i].setOutlineThickness(1.5f);
    }
    keybindHintText.setFont(font);
    keybindHintText.setCharacterSize(18);
    keybindHintText.setFillColor(sf::Color(200,200,80));
    keybindHintText.setString("");
    resetKeybindsButton.setFont(font);
    resetKeybindsButton.setString("Reset to Default");
    resetKeybindsButton.setCharacterSize(18);
    resetKeybindsButton.setFillColor(sf::Color(180,180,180));
    resetKeybindsRect.setSize(sf::Vector2f(160, 36));
    resetKeybindsRect.setFillColor(sf::Color(40, 40, 40));
    resetKeybindsRect.setOutlineColor(sf::Color::White);
    resetKeybindsRect.setOutlineThickness(1.5f);

    // Daltonism modes
    daltonismModes = {"None", "Protanopia", "Deuteranopia", "Tritanopia", "Achromatopsia"};
    // Clamp loaded value into range
    {
        int loaded = config.getDaltonismMode();
        if (loaded < 0 || loaded >= static_cast<int>(daltonismModes.size()))
            loaded = 0;
        currentDaltonismIndex = loaded;
    }

    // Apply loaded accessibility mode globally
    Accessibility::instance().setMode(currentDaltonismIndex);

    // Daltonism UI setup
    daltonismTitleText.setFont(font);
    daltonismTitleText.setString("Daltonism Mode");
    daltonismTitleText.setCharacterSize(32);
    daltonismTitleText.setFillColor(sf::Color::White);

    daltonismValueText.setFont(font);
    daltonismValueText.setCharacterSize(24);
    daltonismValueText.setFillColor(sf::Color(200, 200, 200));
    daltonismValueText.setString(daltonismModes[currentDaltonismIndex]);

    // "<" button
    daltonismLeftRect.setSize(sf::Vector2f(40.f, 40.f));
    daltonismLeftRect.setFillColor(sf::Color(40, 40, 40));
    daltonismLeftRect.setOutlineColor(sf::Color::White);
    daltonismLeftRect.setOutlineThickness(2.f);
    daltonismLeftText.setFont(font);
    daltonismLeftText.setCharacterSize(24);
    daltonismLeftText.setString("<");
    daltonismLeftText.setFillColor(sf::Color::White);

    // ">" button
    daltonismRightRect.setSize(sf::Vector2f(40.f, 40.f));
    daltonismRightRect.setFillColor(sf::Color(40, 40, 40));
    daltonismRightRect.setOutlineColor(sf::Color::White);
    daltonismRightRect.setOutlineThickness(2.f);
    daltonismRightText.setFont(font);
    daltonismRightText.setCharacterSize(24);
    daltonismRightText.setString(">");
    daltonismRightText.setFillColor(sf::Color::White);
}

/**
 * @brief Called when entering the settings state. Sets up UI element positions.
 */
void SettingsState::onEnter() {
    float windowWidth = 1280.0f, windowHeight = 720.0f;
    float keybindsX = windowWidth / 2.0f - 420.0f;
    float keybindsY = windowHeight / 2.0f - 170.0f;
    float ipportX = windowWidth / 2.0f + 60.0f;
    float ipportY = windowHeight / 2.0f - 180.0f;

    // Settings title (top center)
    titleText.setPosition(windowWidth / 2.0f - titleText.getLocalBounds().width / 2.0f, 40.0f);

    // Keybinds section (up and right)
    keybindTitleText.setString("Keybinds");
    keybindTitleText.setPosition(keybindsX, keybindsY);
    for (int i = 0; i < KeybindCount; i++) {
        keybindActionTexts[i].setPosition(keybindsX, keybindsY + 50.0f + i * 54.0f);
        keybindRects[i].setPosition(keybindsX + 185.0f, keybindsY + 50.0f + i * 54.0f);
        keybindKeyTexts[i].setPosition(keybindsX + 195.0f, keybindsY + 55.0f + i * 54.0f);
    }
    resetKeybindsRect.setPosition(keybindsX, keybindsY + 50.0f + KeybindCount * 54.0f + 10.0f);
    resetKeybindsButton.setPosition(resetKeybindsRect.getPosition().x + 10.0f, resetKeybindsRect.getPosition().y + 6.0f);
    keybindHintText.setPosition(keybindsX, keybindsY + 50.0f + KeybindCount * 54.0f + 60.0f);

    // IP/PORT section (up and left)
    ipPortTitleText.setFont(GUIHelper::getFont());
    ipPortTitleText.setString("IP / PORT");
    ipPortTitleText.setCharacterSize(32);
    ipPortTitleText.setFillColor(sf::Color::White);
    ipPortTitleText.setPosition(ipportX, ipportY);
    box1Rect.setPosition(ipportX, ipportY + 60.0f);
    box2Rect.setPosition(ipportX, ipportY + 140.0f);
    box1Hint.setPosition(box1Rect.getPosition().x + 10.0f, box1Rect.getPosition().y + 15.0f);
    box2Hint.setPosition(box2Rect.getPosition().x + 10.0f, box2Rect.getPosition().y + 15.0f);

    // Daltonism section (below IP/PORT)
    float daltonismY = ipportY + 300.0f;
    daltonismTitleText.setPosition(ipportX, daltonismY);
    // Format: make the mode text itself the button
    daltonismValueText.setPosition(ipportX, daltonismY + 55.0f);
    daltonismValueText.setString(daltonismModes[currentDaltonismIndex]);

    // Return button (top left)
    float returnButtonWidth = 300.0f;  // larger, consistent size across states
    float returnButtonHeight = 120.0f;  // larger, consistent size across states
    // Anchor sprite near the left edge; match hitbox to the sprite's visual size
    float leftMargin = 8.0f;
    float topMargin = 10.0f; // moved a bit up
    if (returnSpriteLoaded) {
        sf::Vector2u texSize = returnTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            float scale = std::min(returnButtonWidth / static_cast<float>(texSize.x),
                                   returnButtonHeight / static_cast<float>(texSize.y));
            returnSprite.setScale(scale, scale);
            float scaledW = static_cast<float>(texSize.x) * scale;
            float scaledH = static_cast<float>(texSize.y) * scale;
            // Set sprite centered within its own bounds using top-left anchor
            returnSprite.setPosition(leftMargin + scaledW * 0.5f, topMargin + scaledH * 0.5f);
            // Make the clickable rect exactly match the sprite's bounds
            returnButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
            returnButtonRect.setPosition(leftMargin, topMargin);
            // Hover background removed; we only apply a slight shrink-on-hover to the sprite
        }
    } else {
        returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
        returnButtonRect.setPosition(leftMargin, topMargin);
        GUIHelper::centerText(returnButton,
            returnButtonRect.getPosition().x + returnButtonWidth / 2.0f,
            returnButtonRect.getPosition().y + returnButtonHeight / 2.0f);
    }

    // Overlay default
    m_overlay.setFillColor(sf::Color(0,0,0,150));
}

/**
 * @brief Called when exiting the settings state.
 */
void SettingsState::onExit() {
    // Save all settings to config file
    const char* actionKeys[] = {"up", "down", "left", "right", "shoot"};
    for (int i = 0; i < KeybindCount; i++) {
        config.setKeybind(actionKeys[i], keybinds[i]);
    }
    config.setIP(box1Value);
    config.setPort(box2Value);
    // Save daltonism mode
    config.setDaltonismMode(currentDaltonismIndex);
    config.save();
}

SettingsState::~SettingsState() = default;

void SettingsState::ensureParallaxInitialized(const sf::RenderWindow& window) {
    if (m_parallaxInitialized) return;
    m_parallaxSystem = std::make_unique<ParallaxSystem>(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    if (g_gameState) {
        m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(g_gameState->getLevelIndex()), true);
    } else {
        // Fallback to last known level index persisted in StateManager
        m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
    }
    m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    m_parallaxInitialized = true;
}

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

        // Keybinds: check if a keybind row is clicked
        for (int i = 0; i < KeybindCount; i++) {
            if (keybindRects[i].getGlobalBounds().contains(mousePos)) {
                editingKeybind = i;
                keybindHintText.setString("Press a key for " + keybindActionTexts[i].getString());
            }
        }
        // Reset button
        if (resetKeybindsRect.getGlobalBounds().contains(mousePos)) {
            config.resetToDefaults();
            const char* actionKeys[] = {"up", "down", "left", "right", "shoot"};
            for (int i = 0; i < KeybindCount; i++) {
                keybinds[i] = config.getKeybind(actionKeys[i]);
            }
            box1Value = config.getIP();
            box2Value = config.getPort();
            // Update daltonism
            int loaded = config.getDaltonismMode();
            if (loaded < 0 || loaded >= static_cast<int>(daltonismModes.size())) loaded = 0;
            currentDaltonismIndex = loaded;
            daltonismValueText.setString(daltonismModes[currentDaltonismIndex]);
            Accessibility::instance().setMode(currentDaltonismIndex);
            editingKeybind = -1;
            keybindHintText.setString("");
        }

        // Daltonism: clicking the text cycles through modes
        if (daltonismValueText.getGlobalBounds().contains(mousePos)) {
            currentDaltonismIndex = (currentDaltonismIndex + 1) % static_cast<int>(daltonismModes.size());
            daltonismValueText.setString(daltonismModes[currentDaltonismIndex]);
            Accessibility::instance().setMode(currentDaltonismIndex);
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
    }
    if (event.type == sf::Event::TextEntered) {
        if (typingBox1) {
            if (event.text.unicode == 8 && !box1Value.empty()) box1Value.pop_back();
            else if (event.text.unicode >= 32 && event.text.unicode < 127 && box1Value.size() < 15)
                box1Value += static_cast<char>(event.text.unicode);
        }
        if (typingBox2) {
            if (event.text.unicode == 8 && !box2Value.empty()) box2Value.pop_back();
            else if (event.text.unicode >= 32 && event.text.unicode < 127 && box2Value.size() < 5)
                box2Value += static_cast<char>(event.text.unicode);
        }
    }
    // Keybind assignment: use KeyPressed event to correctly capture sf::Keyboard::Key values
    if (event.type == sf::Event::KeyPressed && editingKeybind != -1) {
        // Validate against reserved keys
        if (event.key.code == sf::Keyboard::Escape) {
            keybindHintText.setString("Escape is reserved for menu navigation.");
            keybindHintText.setFillColor(sf::Color::Red);
            // Do not close the keybind editor; let user pick another key
        } else if (event.key.code == sf::Keyboard::B) {
            keybindHintText.setString("B is reserved for admin commands.");
            keybindHintText.setFillColor(sf::Color::Red);
        } else {
            // Valid key - assign it
            keybinds[editingKeybind] = event.key.code;
            editingKeybind = -1;
            keybindHintText.setString("");
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

    // Return hover detection (sprite)
    sf::Vector2i mousePos = sf::Mouse::getPosition();
    returnHovered = GUIHelper::isPointInRect(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)), returnButtonRect);

    // Keybinds: update key text labels
    for (int i = 0; i < KeybindCount; i++) {
        std::string keyName = rtype::client::gui::keyToString(keybinds[i]);
        if (keyName.empty()) keyName = "?";
        keybindKeyTexts[i].setString(keyName);
        // Hover effect for keybind rows
        sf::Vector2f mouseF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        if (keybindRects[i].getGlobalBounds().contains(mouseF))
            keybindRects[i].setFillColor(sf::Color(60, 60, 60));
        else
            keybindRects[i].setFillColor(sf::Color(40, 40, 40));
    }
    // Reset button hover
    if (resetKeybindsRect.getGlobalBounds().contains(sf::Vector2f(mousePos.x, mousePos.y)))
        resetKeybindsRect.setFillColor(sf::Color(60, 60, 60));
    else
        resetKeybindsRect.setFillColor(sf::Color(40, 40, 40));

    // Hover effect for daltonism text
    if (daltonismValueText.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
        daltonismValueText.setFillColor(sf::Color::White);
    } else {
        daltonismValueText.setFillColor(sf::Color(200,200,200));
    }

    // Update parallax
    if (m_parallaxSystem) {
        m_parallaxSystem->update(deltaTime);
    }
}

/**
 * @brief Renders the settings menu UI to the window.
 * @param window The render window to draw to
 */
void SettingsState::render(sf::RenderWindow& window) {
    // Ensure parallax and overlay
    ensureParallaxInitialized(window);
    if (m_parallaxSystem) m_parallaxSystem->render(window);
    window.draw(m_overlay);

    window.draw(titleText);

    // Draw keybinds section
    window.draw(keybindTitleText);
    for (int i = 0; i < KeybindCount; i++) {
        window.draw(keybindActionTexts[i]);
        window.draw(keybindRects[i]);
        window.draw(keybindKeyTexts[i]);
    }
    window.draw(resetKeybindsRect);
    window.draw(resetKeybindsButton);
    if (editingKeybind != -1)
        window.draw(keybindHintText);

    // Draw IP/PORT section
    window.draw(ipPortTitleText);

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

    // Daltonism
    window.draw(daltonismTitleText);
    window.draw(daltonismValueText);

    // Apply colorblind post-process over the whole frame
    if (Accessibility::instance().isEnabled()) {
        static sf::Texture screenTexture;
        sf::Vector2u size = window.getSize();
        if (screenTexture.getSize() != size) {
            screenTexture.create(size.x, size.y);
        }
        // Capture current frame
        screenTexture.update(window);
        sf::Sprite screenSprite(screenTexture);
        if (auto* shader = Accessibility::instance().getShader()) {
            sf::RenderStates states;
            states.shader = shader;
            window.draw(screenSprite, states);
        }
    }

    // Draw return button
    if (returnSpriteLoaded) {
        // Apply slight shrink on hover
        sf::Vector2f originalScale = returnSprite.getScale();
        if (returnHovered) {
            returnSprite.setScale(originalScale.x * 0.94f, originalScale.y * 0.94f);
        }
        window.draw(returnSprite);
        // Restore scale for next frame/layout
        if (returnHovered) {
            returnSprite.setScale(originalScale);
        }
    } else {
        window.draw(returnButtonRect);
        window.draw(returnButton);
    }
}

} // namespace rtype::client::gui
