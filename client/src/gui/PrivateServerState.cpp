/**
 * @file PrivateServerState.cpp
 * @brief Implementation of the private server connection state for the R-Type client GUI
 * 
 * This file contains the implementation of the PrivateServerState class, which handles
 * the private server connection interface. Users can either join an existing private
 * server using a 4-digit code or create a new private server as an admin.
 * 
 * Key features:
 * - Server code input validation (4-digit numbers 1000-9999)
 * - Join existing private server functionality
 * - Create new private server with random code generation
 * - Text input handling with cursor animation
 * - Real-time input validation and feedback
 * 
 * The state transitions to PrivateServerLobbyState upon successful server
 * connection or creation.
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/PrivateServerState.h"
#include "gui/PrivateServerLobbyState.h"
#include "gui/MainMenuState.h"
#include "gui/ParallaxSystem.h"
#include "gui/GameState.h"
#include "gui/VesselSelectionState.h"
#include <iostream>
#include "network/network.h"
#include "gui/AssetPaths.h"

namespace rtype::client::gui {
    PrivateServerState::~PrivateServerState() = default;

    PrivateServerState::PrivateServerState(StateManager& stateManager, const std::string& username)
        : stateManager(stateManager), username(username), isTyping(false), cursorTimer(0.0f), showCursor(true) {
        config.load(); // Load settings for network configuration
        setupUI();
        m_overlay.setFillColor(sf::Color(0,0,0,150));
    }
    
    void PrivateServerState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Title setup
        titleText.setFont(font);
        titleText.setString("Private Servers");
        titleText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 16);
        titleText.setFillColor(GUIHelper::Colors::TEXT);
        titleText.setStyle(sf::Text::Bold);
        
        // Server code input setup
        serverCodeBox.setFillColor(GUIHelper::Colors::INPUT_BOX);
        serverCodeBox.setOutlineColor(GUIHelper::Colors::TEXT);
        serverCodeBox.setOutlineThickness(2.0f);
        
        serverCodeText.setFont(font);
        serverCodeText.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        serverCodeText.setFillColor(GUIHelper::Colors::TEXT);
        
        serverCodeHintText.setFont(font);
        serverCodeHintText.setString("Enter server code");
        serverCodeHintText.setCharacterSize(GUIHelper::Sizes::HINT_FONT_SIZE);
        serverCodeHintText.setFillColor(GUIHelper::Colors::HINT_TEXT);
        
        // Button setup and shared sprite texture (match Public/Private menu)
        GUIHelper::setupButton(joinButton, joinButtonRect, "Join", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        GUIHelper::setupButton(createButton, createButtonRect, "Create", GUIHelper::Sizes::BUTTON_FONT_SIZE);
        buttonTextureLoaded = buttonTexture.loadFromFile(rtype::client::assets::ui::BUTTON);
        if (buttonTextureLoaded) {
            buttonTexture.setSmooth(true);
            joinButtonSprite.setTexture(buttonTexture);
            createButtonSprite.setTexture(buttonTexture);
            // Center origin for clean scaling/positioning
            sf::Vector2u texSz = buttonTexture.getSize();
            joinButtonSprite.setOrigin(static_cast<float>(texSz.x) * 0.5f, static_cast<float>(texSz.y) * 0.5f);
            createButtonSprite.setOrigin(static_cast<float>(texSz.x) * 0.5f, static_cast<float>(texSz.y) * 0.5f);
            joinHovered = false;
            createHovered = false;
        } else {
            // Fallback rectangle colors
            joinButtonRect.setFillColor(sf::Color(50, 100, 50, 200));
            createButtonRect.setFillColor(sf::Color(50, 50, 100, 200));
        }
        
        // Return button sprite
        returnSpriteLoaded = returnTexture.loadFromFile(rtype::client::assets::ui::RETURN_BUTTON);
        if (returnSpriteLoaded) {
            returnTexture.setSmooth(true);
            returnSprite.setTexture(returnTexture);
            sf::Vector2u sz = returnTexture.getSize();
            returnSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
        } else {
            GUIHelper::setupReturnButton(returnButton, returnButtonRect);
        }
    }
    
    void PrivateServerState::onEnter() {
        std::cout << "Entered Private Server state with username: " << username << std::endl;
    }
    
    void PrivateServerState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Title positioning
        GUIHelper::centerText(titleText, centerX, windowSize.y * 0.2f);
        
        // Server code input positioning
        float boxWidth = std::min(400.0f, windowSize.x * 0.6f);
        float boxHeight = 50.0f;
        serverCodeBox.setSize(sf::Vector2f(boxWidth, boxHeight));
        serverCodeBox.setPosition(centerX - boxWidth / 2, centerY - boxHeight / 2);
        
        // Server code text positioning
        sf::FloatRect boxBounds = serverCodeBox.getGlobalBounds();
        serverCodeHintText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        serverCodeText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        
        // Button positioning (match MainMenu public/private sizing and behavior)
        float baseButtonWidth = std::min(200.0f, windowSize.x * 0.25f);
        float buttonHeight = 60.0f;
        float buttonSpacing = 20.0f;
        float buttonY = centerY + 80.0f;
        float nudge = 20.0f;

        // Compute dynamic widths based on text + padding
        const float horizontalPadding = 60.0f; // 30px each side
        sf::FloatRect joinTextBounds = joinButton.getLocalBounds();
        sf::FloatRect createTextBounds = createButton.getLocalBounds();
        float joinWidth = std::max(baseButtonWidth, joinTextBounds.width + horizontalPadding);
        float createWidth = std::max(baseButtonWidth, createTextBounds.width + horizontalPadding);

        // Position rects for click areas
        joinButtonRect.setSize(sf::Vector2f(joinWidth, buttonHeight));
        joinButtonRect.setPosition(centerX - joinWidth - buttonSpacing / 2.0f - nudge, buttonY);
        GUIHelper::centerText(joinButton,
                  joinButtonRect.getPosition().x + joinWidth / 2.0f,
                  joinButtonRect.getPosition().y + buttonHeight / 2.0f);

        createButtonRect.setSize(sf::Vector2f(createWidth, buttonHeight));
        createButtonRect.setPosition(centerX + buttonSpacing / 2.0f + nudge, buttonY);
        GUIHelper::centerText(createButton,
                  createButtonRect.getPosition().x + createWidth / 2.0f,
                  createButtonRect.getPosition().y + buttonHeight / 2.0f);

        // If sprite texture available, scale and position sprites to fit rects like in MainMenu
        if (buttonTextureLoaded) {
            sf::Vector2u texSize = buttonTexture.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                const float sizeMul = 5.00f; // same as MainMenu
                joinBaseScale = std::min(joinWidth / static_cast<float>(texSize.x),
                                         buttonHeight / static_cast<float>(texSize.y)) * sizeMul;
                createBaseScale = std::min(createWidth / static_cast<float>(texSize.x),
                                           buttonHeight / static_cast<float>(texSize.y)) * sizeMul;
                joinButtonSprite.setScale(joinBaseScale, joinBaseScale);
                createButtonSprite.setScale(createBaseScale, createBaseScale);

                const float spriteYOffset = 6.0f;
                sf::Vector2f joinCenter(joinButtonRect.getPosition().x + joinWidth * 0.5f,
                                        joinButtonRect.getPosition().y + buttonHeight * 0.5f);
                sf::Vector2f createCenter(createButtonRect.getPosition().x + createWidth * 0.5f,
                                          createButtonRect.getPosition().y + buttonHeight * 0.5f);
                joinButtonSprite.setPosition(joinCenter.x, joinCenter.y + spriteYOffset);
                createButtonSprite.setPosition(createCenter.x, createCenter.y + spriteYOffset);
            }
        }
        
    // Return button positioning (top left)
    float returnButtonWidth = 300.0f;
    float returnButtonHeight = 120.0f;
        float leftMargin = 8.0f;
        float topMargin = 10.0f;
        if (returnSpriteLoaded) {
            sf::Vector2u tex = returnTexture.getSize();
            if (tex.x > 0 && tex.y > 0) {
                float scale = std::min(returnButtonWidth / static_cast<float>(tex.x),
                                       returnButtonHeight / static_cast<float>(tex.y));
                returnSprite.setScale(scale, scale);
                float scaledW = static_cast<float>(tex.x) * scale;
                float scaledH = static_cast<float>(tex.y) * scale;
                returnSprite.setPosition(leftMargin + scaledW * 0.5f, topMargin + scaledH * 0.5f);
                returnButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
                returnButtonRect.setPosition(leftMargin, topMargin);
                // Hover background removed; we only apply shrink-on-hover during rendering
            }
        } else {
            returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
            returnButtonRect.setPosition(leftMargin, topMargin);
            GUIHelper::centerText(returnButton,
                      returnButtonRect.getPosition().x + returnButtonWidth / 2,
                      returnButtonRect.getPosition().y + returnButtonHeight / 2);
        }
    }
    
    void PrivateServerState::handleEvent(const sf::Event& event) {
        switch (event.type) {
            case sf::Event::Resized:
                handleResizeEvent(event);
                break;
            case sf::Event::KeyPressed:
                handleKeyboardEvent(event);
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
            default:
                break;
        }
    }
    
    void PrivateServerState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void PrivateServerState::handleKeyboardEvent(const sf::Event& event) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
        }
    }
    
    void PrivateServerState::handleMouseButtonEvent(const sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check server code box click
            if (GUIHelper::isPointInRect(mousePos, serverCodeBox)) {
                isTyping = true;
                serverCodeBox.setOutlineColor(sf::Color::Cyan);
            }
            // Check join button click
            else if (GUIHelper::isPointInRect(mousePos, joinButtonRect)) {
                joinServer();
            }
            // Check create button click
            else if (GUIHelper::isPointInRect(mousePos, createButtonRect)) {
                createServer();
            }
            // Check return button click
            else if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
            // Click outside - stop typing
            else {
                isTyping = false;
                serverCodeBox.setOutlineColor(sf::Color::White);
            }
        }
    }
    
    void PrivateServerState::handleTextInputEvent(const sf::Event& event) {
        if (isTyping) {
            if (event.text.unicode == 8) { // Backspace
                if (!serverCode.empty()) {
                    serverCode.pop_back();
                }
            }
            else if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                if (serverCode.length() < 4) { // Max 4 digits
                    serverCode += static_cast<char>(event.text.unicode);
                }
            }
        }
    }
    
    void PrivateServerState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        if (buttonTextureLoaded) {
            // Sprite hover detection; visual handled in render by scaling
            joinHovered = GUIHelper::isPointInRect(mousePos, joinButtonRect);
            createHovered = GUIHelper::isPointInRect(mousePos, createButtonRect);
        } else {
            // Fallback rectangle hover visuals
            GUIHelper::applyButtonHover(joinButtonRect, joinButton, 
                                      GUIHelper::isPointInRect(mousePos, joinButtonRect),
                                      sf::Color(50, 100, 50, 200), sf::Color(70, 150, 70, 200));
            GUIHelper::applyButtonHover(createButtonRect, createButton, 
                                      GUIHelper::isPointInRect(mousePos, createButtonRect),
                                      sf::Color(50, 50, 100, 200), sf::Color(70, 70, 150, 200));
        }
        
    // Return hover flag only (visuals applied at render)
    returnHovered = GUIHelper::isPointInRect(mousePos, returnButtonRect);
    }
    
    void PrivateServerState::update(float deltaTime) {
        // Cursor blinking animation
        cursorTimer += deltaTime;
        if (cursorTimer >= 0.5f) {
            showCursor = !showCursor;
            cursorTimer = 0.0f;
        }
        
        // Update server code text with cursor
        serverCodeText.setString(serverCode + (showCursor && isTyping ? "|" : ""));
        
        // Ensure text stays positioned correctly
        sf::FloatRect boxBounds = serverCodeBox.getGlobalBounds();
        serverCodeText.setPosition(boxBounds.left + 10, boxBounds.top + 15);
        // Update parallax
        if (m_parallaxSystem) {
            m_parallaxSystem->update(deltaTime);
        }
    }
    
    void PrivateServerState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        ensureParallaxInitialized(window);
        if (m_parallaxSystem) m_parallaxSystem->render(window);
        window.draw(m_overlay);
        
        // Render title
        window.draw(titleText);
        
        // Render server code input
        window.draw(serverCodeBox);
        if (serverCode.empty() && !isTyping) {
            window.draw(serverCodeHintText);
        } else {
            window.draw(serverCodeText);
        }
        
        // Render Join/Create buttons (sprite if available)
        if (buttonTextureLoaded) {
            const float hoverScaleFactor = 1.06f;
            joinButtonSprite.setScale(joinBaseScale * (joinHovered ? hoverScaleFactor : 1.0f),
                                      joinBaseScale * (joinHovered ? hoverScaleFactor : 1.0f));
            createButtonSprite.setScale(createBaseScale * (createHovered ? hoverScaleFactor : 1.0f),
                                        createBaseScale * (createHovered ? hoverScaleFactor : 1.0f));
            window.draw(joinButtonSprite);
            window.draw(createButtonSprite);
            // Draw labels on top for clarity
            window.draw(joinButton);
            window.draw(createButton);
        } else {
            window.draw(joinButtonRect);
            window.draw(joinButton);
            window.draw(createButtonRect);
            window.draw(createButton);
        }
        if (returnSpriteLoaded) {
            sf::Vector2f originalScale = returnSprite.getScale();
            if (returnHovered) returnSprite.setScale(originalScale.x * 0.94f, originalScale.y * 0.94f);
            window.draw(returnSprite);
            if (returnHovered) returnSprite.setScale(originalScale);
        } else {
            window.draw(returnButtonRect);
            window.draw(returnButton);
        }
    }
    
    void PrivateServerState::joinServer() {
        if (GUIHelper::isValidServerCode(serverCode)) {
            std::cout << "Joining private server with code: " << serverCode << std::endl;
            
            // Convert server code to room ID (assuming server code maps to room ID)
            uint32_t roomId = static_cast<uint32_t>(std::stoi(serverCode));

            // Use network settings from config
            std::string serverIp = config.getIP();
            int serverPort = getValidatedPort();
            
            std::cout << "Redirecting to vessel selection..." << std::endl;
            
            // Push vessel selection state
            stateManager.pushState(std::make_unique<VesselSelectionState>(
                stateManager, username, serverIp, serverPort, roomId
            ));

        } else {
            std::cout << "Invalid server code. Please enter a 4-digit number between 1000-9999." << std::endl;
        }
    }
    
    void PrivateServerState::createServer() {
        std::string serverIp = config.getIP();
        int serverPort = getValidatedPort();
        
        std::cout << "Creating server, redirecting to vessel selection..." << std::endl;
        
        // Push vessel selection state (room code 0 = create new)
        stateManager.pushState(std::make_unique<VesselSelectionState>(
            stateManager, username, serverIp, serverPort, 0
        ));
    }
    
    int PrivateServerState::getValidatedPort() {
        try {
            int port = std::stoi(config.getPort());
            // Validate port range (1-65535). Port 0 is reserved and invalid for TCP/UDP.
            if (port < 1 || port > 65535) {
                std::cerr << "[PrivateServerState] Invalid port number in config (" << port 
                          << "). Valid range is 1-65535. Using default port 4242." << std::endl;
                return 4242;
            }
            return port;
        } catch (const std::exception& e) {
            std::cerr << "[PrivateServerState] Failed to parse port from config: " << e.what() 
                      << ". Using default port 4242." << std::endl;
            return 4242;
        }
    }
    
    void PrivateServerState::onExit() {
        std::cout << "Exiting Private Server state" << std::endl;
        // Note: We don't disconnect here because we might be transitioning to the lobby
        // The lobby will manage the connection
    }

    void PrivateServerState::ensureParallaxInitialized(const sf::RenderWindow& window) {
        if (m_parallaxInitialized) return;
        m_parallaxSystem = std::make_unique<ParallaxSystem>(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        );
        if (g_gameState) {
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(g_gameState->getLevelIndex()), true);
        } else {
            // Keep visuals consistent with last played level when no active game exists
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
    }
    
}
