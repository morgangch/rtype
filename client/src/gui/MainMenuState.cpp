/**
 * @file MainMenuState.cpp
 * @brief Implementation of the MainMenuState class
 * 
 * This file implements the main menu interface for the R-TYPE client.
 * It handles user input for username entry, button interactions, and
 * navigation to server selection screens. The implementation uses the
 * centralized GUIHelper utilities for consistent styling and behavior.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/MainMenuState.h"
#include "gui/PublicServerState.h"
#include "gui/PrivateServerState.h"
#include "gui/GameState.h"
#include "gui/ParallaxSystem.h"
#include "gui/AudioFactory.h"
#include "gui/SettingsState.h"
#include "gui/AssetPaths.h"
#include <iostream>
#include <cstdlib>

namespace rtype::client::gui {
    
    MainMenuState::~MainMenuState() = default;

    MainMenuState::MainMenuState(StateManager& stateManager)
        : stateManager(stateManager), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        setupUI();
        // Prepare overlay default color; size will be set in updateLayout/ensureParallaxInitialized
        m_overlay.setFillColor(sf::Color(0, 0, 0, 150));
    }
    
    void MainMenuState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Title setup
        titleText.setFont(font);
        titleText.setString("THE TOP RTYPE");
        titleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE);
        titleText.setFillColor(GUIHelper::Colors::TEXT);
        titleText.setStyle(sf::Text::Bold);
        
        // Username input setup
        usernameBox.setFillColor(GUIHelper::Colors::INPUT_BOX);
        usernameBox.setOutlineColor(GUIHelper::Colors::TEXT);
        usernameBox.setOutlineThickness(2.0f);
        
        usernameText.setFont(font);
        usernameText.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        usernameText.setFillColor(GUIHelper::Colors::TEXT);
        
        usernameHintText.setFont(font);
        usernameHintText.setString("Username");
        usernameHintText.setCharacterSize(GUIHelper::Sizes::HINT_FONT_SIZE);
        usernameHintText.setFillColor(GUIHelper::Colors::HINT_TEXT);
        
        // Button setup using GUIHelper
        GUIHelper::setupButton(publicServersButton, publicButtonRect, "Public", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        GUIHelper::setupButton(privateServersButton, privateButtonRect, "Private", GUIHelper::Sizes::BUTTON_FONT_SIZE);

        // Load shared button texture for Public/Private buttons
        buttonTextureLoaded = buttonTexture.loadFromFile(rtype::client::assets::ui::BUTTON);
        if (buttonTextureLoaded) {
            buttonTexture.setSmooth(true);
            publicButtonSprite.setTexture(buttonTexture);
            privateButtonSprite.setTexture(buttonTexture);
            // Center origins for easy centering and hover scaling
            sf::Vector2u texSize = buttonTexture.getSize();
            publicButtonSprite.setOrigin(static_cast<float>(texSize.x) * 0.5f, static_cast<float>(texSize.y) * 0.5f);
            privateButtonSprite.setOrigin(static_cast<float>(texSize.x) * 0.5f, static_cast<float>(texSize.y) * 0.5f);
            publicHovered = false;
            privateHovered = false;
        }

        // Settings button replaced by sprite; keep rect for positioning/click zone
        // Load settings gear sprite
        settingsSpriteLoaded = settingsTexture.loadFromFile(rtype::client::assets::ui::SETTINGS_GEAR);
        if (settingsSpriteLoaded) {
            settingsTexture.setSmooth(true);
            settingsSprite.setTexture(settingsTexture);
            // Center origin for clean rotation
            sf::Vector2u sz = settingsTexture.getSize();
            settingsSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
            settingsRotation = 0.0f;
            settingsHovered = false;
        } else {
            // Fallback: show legacy text button if texture missing
            GUIHelper::setupButton(settingsButtonText, settingsButtonRect, "Settings", 20.0f);
            settingsButtonRect.setFillColor(GUIHelper::Colors::BUTTON_NORMAL);
        }
    }
    
    void MainMenuState::onEnter() {
        // Called when this state becomes active
        const std::string menuMusic = AudioFactory::getMusicPath(AudioFactory::MusicId::Menu);
        if (m_musicManager.loadFromFile(menuMusic)) {
            m_musicManager.setVolume(35.0f);
            m_musicManager.play(true);
        } else {
            std::cerr << "MainMenuState: could not load menu music: " << menuMusic << std::endl;
        }

        // Keep parallax theme in sync when coming back to the menu
        if (m_parallaxSystem) {
            if (g_gameState) {
                setParallaxThemeFromLevel(g_gameState->getLevelIndex());
            } else {
                setParallaxThemeFromLevel(stateManager.getLastLevelIndex());
            }
        }
    }

    void MainMenuState::onExit() {
        // Stop menu music when leaving main menu
        m_musicManager.stop();
    }
    
    void MainMenuState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning (center top)
        GUIHelper::centerText(titleText, centerX, windowSize.y * 0.2f);
        
        // Username box positioning (middle)
        float boxWidth = std::min(400.0f, windowSize.x * 0.6f);
        float boxHeight = 50.0f;
        usernameBox.setSize(sf::Vector2f(boxWidth, boxHeight));
        usernameBox.setPosition(centerX - boxWidth / 2, centerY - boxHeight / 2);
        
        // Username text positioning
        sf::FloatRect boxBounds = usernameBox.getGlobalBounds();
        usernameHintText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        usernameText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        
        // Button positioning (below username box)
        float baseButtonWidth = std::min(200.0f, windowSize.x * 0.25f);
        float buttonHeight = 60.0f;
        float buttonSpacing = 20.0f;
        float buttonY = centerY + 80.0f;
        float nudge = 20.0f; // move left button further left, right button further right

        // Compute dynamic widths based on text size + padding
        const float horizontalPadding = 60.0f; // 30px each side
        sf::FloatRect pubTextBounds = publicServersButton.getLocalBounds();
        sf::FloatRect priTextBounds = privateServersButton.getLocalBounds();
        float publicWidth = std::max(baseButtonWidth, pubTextBounds.width + horizontalPadding);
        float privateWidth = std::max(baseButtonWidth, priTextBounds.width + horizontalPadding);

        // Public servers button (slightly more to the left)
        publicButtonRect.setSize(sf::Vector2f(publicWidth, buttonHeight));
        publicButtonRect.setPosition(centerX - publicWidth - buttonSpacing / 2.0f - nudge, buttonY);
        GUIHelper::centerText(publicServersButton, 
                  publicButtonRect.getPosition().x + publicWidth / 2.0f,
                  publicButtonRect.getPosition().y + buttonHeight / 2.0f);
        
        // Private servers button (slightly more to the right) — can have different width
        privateButtonRect.setSize(sf::Vector2f(privateWidth, buttonHeight));
        privateButtonRect.setPosition(centerX + buttonSpacing / 2.0f + nudge, buttonY);
        GUIHelper::centerText(privateServersButton,
                  privateButtonRect.getPosition().x + privateWidth / 2.0f,
                  privateButtonRect.getPosition().y + buttonHeight / 2.0f);

        // If button sprites are available, scale and position them to fit their rects
        if (buttonTextureLoaded) {
            sf::Vector2u texSize = buttonTexture.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                // Compute base uniform scale to fit within each rect while preserving aspect ratio
                const float sizeMul = 5.00f;
                publicBaseScale = std::min(publicWidth / static_cast<float>(texSize.x),
                                           buttonHeight / static_cast<float>(texSize.y)) * sizeMul;
                privateBaseScale = std::min(privateWidth / static_cast<float>(texSize.x),
                                            buttonHeight / static_cast<float>(texSize.y)) * sizeMul;

                publicButtonSprite.setScale(publicBaseScale, publicBaseScale);
                privateButtonSprite.setScale(privateBaseScale, privateBaseScale);

                // Center sprites within their respective rects
                const float spriteYOffset = 6.0f;
                sf::Vector2f pubCenter(publicButtonRect.getPosition().x + publicWidth * 0.5f,
                                       publicButtonRect.getPosition().y + buttonHeight * 0.5f);
                sf::Vector2f priCenter(privateButtonRect.getPosition().x + privateWidth * 0.5f,
                                       privateButtonRect.getPosition().y + buttonHeight * 0.5f);
                publicButtonSprite.setPosition(pubCenter.x, pubCenter.y + spriteYOffset);
                privateButtonSprite.setPosition(priCenter.x, priCenter.y + spriteYOffset);
            }
        }

    // Settings button positioning (top-left).
    float settingsWidth = 140.0f;
    float settingsHeight = 90.0f;
    float settingsX = 2.0f;
    float settingsY = 12.0f;
        settingsButtonRect.setSize(sf::Vector2f(settingsWidth, settingsHeight));
        settingsButtonRect.setPosition(settingsX, settingsY);
        if (settingsSpriteLoaded) {
            // Scale sprite to fit height of the button rect
            sf::Vector2u texSize = settingsTexture.getSize();
            if (texSize.y != 0) {
                float scale = settingsHeight * 1.2f / static_cast<float>(texSize.y);
                settingsSprite.setScale(scale, scale);
            }
            // Center in the rect
            sf::Vector2f center(settingsButtonRect.getPosition().x + settingsWidth * 0.5f,
                                settingsButtonRect.getPosition().y + settingsHeight * 0.5f);
            settingsSprite.setPosition(center);
        } else {
            GUIHelper::centerText(settingsButtonText,
                settingsButtonRect.getPosition().x + settingsWidth / 2,
                settingsButtonRect.getPosition().y + settingsHeight / 2);
        }

    // Update overlay size to current window
    m_overlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    }
    
    void MainMenuState::handleEvent(const sf::Event& event) {
        switch (event.type) {
            case sf::Event::Resized:
                handleResizeEvent(event);
                break;
            case sf::Event::MouseButtonPressed:
                handleMouseButtonEvent(event);
                break;
            case sf::Event::TextEntered:
                handleTextInputEvent(event);
                break;
            case sf::Event::MouseMoved:
                handleMouseMoveEvent(event);
                break;
            case sf::Event::KeyPressed:
                handleKeyPressEvent(event);
                break;
            default:
                break;
        }
    }
    
    void MainMenuState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void MainMenuState::handleMouseButtonEvent(const sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check username box click
            if (GUIHelper::isPointInRect(mousePos, usernameBox)) {
                isTyping = true;
                usernameBox.setOutlineColor(sf::Color::Cyan);
            }
            // Check public servers button
            else if (GUIHelper::isPointInRect(mousePos, publicButtonRect)) {
                onPublicServersClick();
            }
            // Check private servers button
            else if (GUIHelper::isPointInRect(mousePos, privateButtonRect)) {
                onPrivateServersClick();
            }
            // Check settings button
            else if (GUIHelper::isPointInRect(mousePos, settingsButtonRect)) {
                stateManager.changeState(std::make_unique<SettingsState>(stateManager));
            }
            // Click outside - stop typing
            else {
                isTyping = false;
                usernameBox.setOutlineColor(sf::Color::White);
            }
        }
    }
    
    void MainMenuState::handleTextInputEvent(const sf::Event& event) {
        if (isTyping) {
            if (event.text.unicode == 8) { // Backspace
                if (!username.empty()) {
                    username.pop_back();
                }
            }
            else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                if (username.length() < 15) { // Max username length
                    username += static_cast<char>(event.text.unicode);
                }
            }
        }
    }

    void MainMenuState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        // Button hover handling
        if (buttonTextureLoaded) {
            publicHovered = GUIHelper::isPointInRect(mousePos, publicButtonRect);
            privateHovered = GUIHelper::isPointInRect(mousePos, privateButtonRect);
        } else {
            // Fallback to rectangle hover visuals
            GUIHelper::applyButtonHover(publicButtonRect, publicServersButton, 
                                      GUIHelper::isPointInRect(mousePos, publicButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
            GUIHelper::applyButtonHover(privateButtonRect, privateServersButton, 
                                      GUIHelper::isPointInRect(mousePos, privateButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
        }

        // Settings hover: detect over rect; spin handled in update()
        if (settingsSpriteLoaded) {
            settingsHovered = GUIHelper::isPointInRect(mousePos, settingsButtonRect);
        } else {
            // Fallback hover effect for text button
            GUIHelper::applyButtonHover(settingsButtonRect, settingsButtonText,
                                      GUIHelper::isPointInRect(mousePos, settingsButtonRect),
                                      GUIHelper::Colors::BUTTON_NORMAL, GUIHelper::Colors::BUTTON_HOVER);
        }
    }
    
    void MainMenuState::handleKeyPressEvent(const sf::Event& event) {
        // Escape key to exit
        if (event.key.code == sf::Keyboard::Escape) {
            std::exit(0);
        }
    }
    
    void MainMenuState::update(float deltaTime) {
        // Cursor blinking animation
        cursorTimer += deltaTime;
        if (cursorTimer >= 0.5f) {
            showCursor = !showCursor;
            cursorTimer = 0.0f;
        }
        
        // Update username text with cursor
        usernameText.setString(username + (showCursor && isTyping ? "|" : ""));
        
        // Ensure text stays positioned correctly
        sf::FloatRect boxBounds = usernameBox.getGlobalBounds();
        usernameText.setPosition(boxBounds.left + 10, boxBounds.top + 15);

        // Spin gear on hover
        if (settingsSpriteLoaded && settingsHovered) {
            settingsRotation += 360.0f * deltaTime;
            if (settingsRotation >= 360.0f) settingsRotation -= 360.0f;
            settingsSprite.setRotation(settingsRotation);
        } else if (settingsSpriteLoaded && !settingsHovered && settingsRotation != 0.0f) {
            // Optional: gently ease back to 0 if needed; for now, keep last angle
        }

        // Update parallax system if created
        if (m_parallaxSystem) {
            m_parallaxSystem->update(deltaTime);
        }
    }
    
    void MainMenuState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());

        // Ensure parallax exists and sized to window
        ensureParallaxInitialized(window);

        // Render parallax background behind UI
        if (m_parallaxSystem) {
            m_parallaxSystem->render(window);
        }

        // Draw a semi-transparent overlay to keep UI readable
        window.draw(m_overlay);

        // Render title
        window.draw(titleText);
        
        // Render username input
        window.draw(usernameBox);
        if (username.empty() && !isTyping) {
            window.draw(usernameHintText);
        } else {
            window.draw(usernameText);
        }
        
        // Render Public/Private buttons (sprite if available; fallback to rectangles)
        if (buttonTextureLoaded) {
            // Apply slight hover scaling
            const float hoverScaleFactor = 1.06f;
            publicButtonSprite.setScale(publicBaseScale * (publicHovered ? hoverScaleFactor : 1.0f),
                                        publicBaseScale * (publicHovered ? hoverScaleFactor : 1.0f));
            privateButtonSprite.setScale(privateBaseScale * (privateHovered ? hoverScaleFactor : 1.0f),
                                         privateBaseScale * (privateHovered ? hoverScaleFactor : 1.0f));

            window.draw(publicButtonSprite);
            window.draw(privateButtonSprite);
            // Draw button labels on top
            window.draw(publicServersButton);
            window.draw(privateServersButton);
        } else {
            window.draw(publicButtonRect);
            window.draw(publicServersButton);
            window.draw(privateButtonRect);
            window.draw(privateServersButton);
        }

        // Render settings as sprite (fallback to text if needed)
        if (settingsSpriteLoaded) {
            window.draw(settingsSprite);
        } else {
            window.draw(settingsButtonRect);
            window.draw(settingsButtonText);
        }
    }

    void MainMenuState::ensureParallaxInitialized(const sf::RenderWindow& window) {
        if (m_parallaxInitialized) return;

        m_parallaxSystem = std::make_unique<ParallaxSystem>(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        );

        // If a GameState exists, pick a theme consistent with the current level
        if (g_gameState) {
            setParallaxThemeFromLevel(g_gameState->getLevelIndex());
        } else {
            // Otherwise use the last persisted level index from the state manager
            setParallaxThemeFromLevel(stateManager.getLastLevelIndex());
        }

        // Ensure overlay sized to window as well
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));

        m_parallaxInitialized = true;
    }

    void MainMenuState::setParallaxThemeFromLevel(int levelIndex) {
        if (!m_parallaxSystem) return;
        m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(levelIndex), true);
    }
    
    void MainMenuState::onPublicServersClick() {
        std::cout << "Switching to Public Server state" << std::endl;
        std::string finalUsername = username.empty() ? GUIHelper::generateRandomUsername() : username;
        stateManager.changeState(std::make_unique<PublicServerState>(stateManager, finalUsername));
    }
    
    void MainMenuState::onPrivateServersClick() {
        std::cout << "Switching to Private Server state" << std::endl;
        std::string finalUsername = username.empty() ? GUIHelper::generateRandomUsername() : username;
        stateManager.changeState(std::make_unique<PrivateServerState>(stateManager, finalUsername));
    }
}
