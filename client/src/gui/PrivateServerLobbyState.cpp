/**
 * @file PrivateServerLobbyState.cpp
 * @brief Implementation of the private server lobby state for the R-Type client GUI
 * 
 * This file contains the implementation of the PrivateServerLobbyState class, which handles
 * the private server lobby interface where players wait for game start. The lobby supports
 * both admin and regular player roles, with different UI interactions for each role.
 * 
 * Key features:
 * - Ready state management for players
 * - Admin controls for starting games
 * - Real-time player count display
 * - Server code display for sharing
 * - Dynamic button behavior based on user role
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/PrivateServerLobbyState.h"
#include "gui/GameState.h"
#include "gui/MainMenuState.h"
#include "gui/ParallaxSystem.h"
#include "network/senders.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include "gui/AssetPaths.h"

// External declaration of global player server ID set by JOIN_ROOM_ACCEPTED handler
extern uint32_t g_playerServerId;

namespace rtype::client::gui {
    // Global pointer to current lobby state (for network callbacks)
    PrivateServerLobbyState* g_lobbyState = nullptr;
    PrivateServerLobbyState::~PrivateServerLobbyState() = default;

    PrivateServerLobbyState::PrivateServerLobbyState(StateManager& stateManager, const std::string& username, 
                                                     const std::string& serverCode, bool isAdmin)
        : stateManager(stateManager), username(username), serverCode(serverCode), isAdmin(isAdmin) {
        setupUI();
        m_overlay.setFillColor(sf::Color(0,0,0,150));
        g_lobbyState = this; // Register this instance globally for network callbacks
    }
    
    void PrivateServerLobbyState::setupUI() {
        const sf::Font& font = GUIHelper::getFont();
        
        // Waiting text setup (for non-admin players)
        playersWaitingText.setFont(font);
        playersWaitingText.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE - 28);
        playersWaitingText.setFillColor(GUIHelper::Colors::TEXT);
        
        // Action button sprite (admin only)
        if (isAdmin) {
            actionSpriteLoaded = actionTexture.loadFromFile(rtype::client::assets::ui::READY_BUTTON);
            if (!actionSpriteLoaded) {
                GUIHelper::setupButton(actionButton, actionButtonRect, "", GUIHelper::Sizes::BUTTON_FONT_SIZE);
            } else {
                actionTexture.setSmooth(true);
                actionSprite.setTexture(actionTexture);
                sf::Vector2u sz = actionTexture.getSize();
                actionSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
            }
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
        
        // Server code display setup
        serverCodeDisplay.setFont(font);
        serverCodeDisplay.setString("Server Code " + serverCode);
        serverCodeDisplay.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
        serverCodeDisplay.setFillColor(sf::Color::Yellow);
        serverCodeDisplay.setStyle(sf::Text::Bold);
        
        // Settings gear (admin only)
        if (isAdmin) {
            m_settingsSpriteLoaded = m_settingsTexture.loadFromFile(rtype::client::assets::ui::SETTINGS_GEAR);
            if (m_settingsSpriteLoaded) {
                m_settingsTexture.setSmooth(true);
                m_settingsSprite.setTexture(m_settingsTexture);
                sf::Vector2u sz = m_settingsTexture.getSize();
                m_settingsSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
            } else {
                m_settingsRect.setFillColor(sf::Color(60, 60, 60, 200));
                m_settingsRect.setOutlineColor(sf::Color::White);
                m_settingsRect.setOutlineThickness(1.5f);
            }

            // Settings panel texts (titles and rows)
            m_gameplayTitle.setFont(font);
            m_gameplayTitle.setString("Gameplay");
            m_gameplayTitle.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
            m_gameplayTitle.setFillColor(GUIHelper::Colors::TEXT);

            m_aiTitle.setFont(font);
            m_aiTitle.setString("AI");
            m_aiTitle.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
            m_aiTitle.setFillColor(GUIHelper::Colors::TEXT);

            m_cheatsTitle.setFont(font);
            m_cheatsTitle.setString("Cheats");
            m_cheatsTitle.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
            m_cheatsTitle.setFillColor(GUIHelper::Colors::TEXT);

            m_difficultyLabel.setFont(font);
            m_difficultyLabel.setString("Difficulty");
            m_difficultyLabel.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
            m_difficultyLabel.setFillColor(GUIHelper::Colors::TEXT);

            m_difficultyValue.setFont(font);
            m_difficultyValue.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
            m_difficultyValue.setFillColor(sf::Color(200, 220, 255));

            m_friendlyFireLabel.setFont(font);
            m_friendlyFireLabel.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
            m_friendlyFireLabel.setFillColor(GUIHelper::Colors::TEXT);
            // Squares (checkbox visuals)
            auto setupSquare = [](sf::RectangleShape& sq) {
                sq.setSize(sf::Vector2f(18.f, 18.f));
                sq.setFillColor(sf::Color(0, 0, 0, 0));
                sq.setOutlineColor(sf::Color(220, 220, 220));
                sq.setOutlineThickness(2.f);
            };
            setupSquare(m_sqFriendlyFire);

            m_aiAssistLabel.setFont(font);
            m_aiAssistLabel.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
            m_aiAssistLabel.setFillColor(GUIHelper::Colors::TEXT);
            setupSquare(m_sqAIAssist);

            m_megaDamageLabel.setFont(font);
            m_megaDamageLabel.setCharacterSize(GUIHelper::Sizes::INPUT_FONT_SIZE);
            m_megaDamageLabel.setFillColor(GUIHelper::Colors::TEXT);
            setupSquare(m_sqMegaDamage);

            // Panel background
            m_settingsPanelRect.setFillColor(sf::Color(20, 20, 20, 210));
            m_settingsPanelRect.setOutlineColor(sf::Color(255, 255, 255, 180));
            m_settingsPanelRect.setOutlineThickness(2.f);

            updateSettingsTexts();
        }

        updateWaitingText();
        if (isAdmin) {
            updateActionButton();
        }
    }
    
    void PrivateServerLobbyState::onEnter() {
        std::cout << "Entered Private Server Lobby:" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Server Code " << serverCode << std::endl;
        std::cout << "Is Admin: " << (isAdmin ? "Yes" : "No") << std::endl;
    }
    
    void PrivateServerLobbyState::updateLayout(const sf::Vector2u& windowSize) {
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        
        // Waiting text positioning (center)
        GUIHelper::centerText(playersWaitingText, centerX, centerY - 50.0f);
        
        // Action button positioning (admin only, below the text)
        if (isAdmin) {
            float buttonWidth = 520.0f;
            float buttonHeight = 180.0f;
            float buttonY = centerY + 50.0f;
            if (actionSpriteLoaded) {
                sf::Vector2u tex = actionTexture.getSize();
                if (tex.x > 0 && tex.y > 0) {
                    float scale = std::min(buttonWidth / static_cast<float>(tex.x),
                                           buttonHeight / static_cast<float>(tex.y));
                    actionSprite.setScale(scale, scale);
                    float scaledW = static_cast<float>(tex.x) * scale;
                    float scaledH = static_cast<float>(tex.y) * scale;
                    actionSprite.setPosition(centerX, buttonY + scaledH * 0.5f);
                    actionButtonRect.setSize(sf::Vector2f(scaledW, scaledH));
                    actionButtonRect.setPosition(centerX - scaledW * 0.5f, buttonY);
                }
            } else {
                actionButtonRect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
                actionButtonRect.setPosition(centerX - buttonWidth / 2, buttonY);
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
            }
        } else {
            returnButtonRect.setSize(sf::Vector2f(returnButtonWidth, returnButtonHeight));
            returnButtonRect.setPosition(leftMargin, topMargin);
            GUIHelper::centerText(returnButton,
                      returnButtonRect.getPosition().x + returnButtonWidth / 2,
                      returnButtonRect.getPosition().y + returnButtonHeight / 2);
        }
        
        // Server code display positioning (top right, moved more to the left)
        sf::FloatRect codeTextBounds = serverCodeDisplay.getLocalBounds();
        serverCodeDisplay.setPosition(windowSize.x - codeTextBounds.width - 30.0f, 30.0f);

        // Settings gear bottom-left (admin only)
        if (isAdmin) {
            float gearWidth = 110.0f;
            float gearHeight = 110.0f;
            float leftMargin = 10.0f;
            float bottomMargin = 12.0f;

            if (m_settingsSpriteLoaded) {
                sf::Vector2u tex = m_settingsTexture.getSize();
                if (tex.x > 0 && tex.y > 0) {
                    float scale = std::min(gearWidth / static_cast<float>(tex.x),
                                           gearHeight / static_cast<float>(tex.y));
                    m_settingsSprite.setScale(scale, scale);
                    float scaledW = static_cast<float>(tex.x) * scale;
                    float scaledH = static_cast<float>(tex.y) * scale;
                    m_settingsSprite.setPosition(leftMargin + scaledW * 0.5f,
                                                 windowSize.y - bottomMargin - scaledH * 0.5f);
                    m_settingsRect.setSize(sf::Vector2f(scaledW, scaledH));
                    m_settingsRect.setPosition(leftMargin, windowSize.y - bottomMargin - scaledH);
                }
            } else {
                m_settingsRect.setSize(sf::Vector2f(gearWidth, gearHeight));
                m_settingsRect.setPosition(leftMargin, windowSize.y - bottomMargin - gearHeight);
            }
        }

        // Settings panel layout (admin only)
        if (isAdmin) {
            updateSettingsLayout(windowSize);
        }
    }
    
    void PrivateServerLobbyState::handleEvent(const sf::Event& event) {
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
            case sf::Event::MouseButtonReleased:
                if (isAdmin) actionPressed = false;
                break;
            case sf::Event::MouseMoved:
                handleMouseMoveEvent(event);
                break;
            default:
                break;
        }
    }
    
    void PrivateServerLobbyState::handleResizeEvent(const sf::Event& event) {
        updateLayout(sf::Vector2u(event.size.width, event.size.height));
    }
    
    void PrivateServerLobbyState::handleKeyboardEvent(const sf::Event& event) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
        }
    }
    
    void PrivateServerLobbyState::handleMouseButtonEvent(const sf::Event& event) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Settings icon toggle (admin only)
            if (isAdmin) {
                if (GUIHelper::isPointInRect(mousePos, m_settingsRect)) {
                    m_showSettings = !m_showSettings;
                    return; // consume click
                }
            }

            // Settings panel interactions (admin only)
            if (isAdmin && m_showSettings) {
                if (GUIHelper::isPointInRect(mousePos, m_rectDifficulty)) {
                    m_difficultyIndex = (m_difficultyIndex + 1) % 3; // cycle Easy/Normal/Hard
                    updateSettingsTexts();
                    // Notify server of settings change (admin only)
                    rtype::client::network::senders::send_lobby_settings_update(static_cast<uint8_t>(m_difficultyIndex), m_friendlyFire, m_aiAssist, m_megaDamage);
                    return;
                }
                if (GUIHelper::isPointInRect(mousePos, m_rectFriendlyFire)) {
                    m_friendlyFire = !m_friendlyFire;
                    updateSettingsTexts();
                    rtype::client::network::senders::send_lobby_settings_update(static_cast<uint8_t>(m_difficultyIndex), m_friendlyFire, m_aiAssist, m_megaDamage);
                    return;
                }
                // AI assist only when exactly 1 player in lobby
                if (m_totalPlayersInLobby == 1 && GUIHelper::isPointInRect(mousePos, m_rectAIAssist)) {
                    m_aiAssist = !m_aiAssist;
                    updateSettingsTexts();
                    rtype::client::network::senders::send_lobby_settings_update(static_cast<uint8_t>(m_difficultyIndex), m_friendlyFire, m_aiAssist, m_megaDamage);
                    return;
                }
                if (GUIHelper::isPointInRect(mousePos, m_rectMegaDamage)) {
                    m_megaDamage = !m_megaDamage;
                    updateSettingsTexts();
                    rtype::client::network::senders::send_lobby_settings_update(static_cast<uint8_t>(m_difficultyIndex), m_friendlyFire, m_aiAssist, m_megaDamage);
                    return;
                }
            }

            // Check action button click (admin only)
            if (isAdmin && GUIHelper::isPointInRect(mousePos, actionButtonRect)) {
                actionPressed = true;
                startGame();
            }
            // Check return button click
            else if (GUIHelper::isPointInRect(mousePos, returnButtonRect)) {
                stateManager.changeState(std::make_unique<MainMenuState>(stateManager));
            }
        }
    }
    
    void PrivateServerLobbyState::handleMouseMoveEvent(const sf::Event& event) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        // Button hover effects for admin start button
        if (isAdmin) {
            actionHovered = GUIHelper::isPointInRect(mousePos, actionButtonRect);
        }
        
    // Return hover flag only (visuals applied at render)
    returnHovered = GUIHelper::isPointInRect(mousePos, returnButtonRect);

        // Settings gear hover
        if (isAdmin) {
            m_settingsHovered = GUIHelper::isPointInRect(mousePos, m_settingsRect);
        }
    }
    
    void PrivateServerLobbyState::update(float deltaTime) {
        // Update parallax if present
        if (m_parallaxSystem) {
            m_parallaxSystem->update(deltaTime);
        }
    }
    
    void PrivateServerLobbyState::render(sf::RenderWindow& window) {
        // Update layout if needed
        updateLayout(window.getSize());
        // Ensure and render parallax behind UI
        ensureParallaxInitialized(window);
        if (m_parallaxSystem) {
            m_parallaxSystem->render(window);
        }
        window.draw(m_overlay);
        
        // Render waiting text
        window.draw(playersWaitingText);
        
        // Render action button (admin only)
        if (isAdmin) {
            if (actionSpriteLoaded) {
                if (actionPressed) {
                    sf::Sprite glow = actionSprite;
                    sf::Vector2f os = glow.getScale();
                    glow.setScale(os.x * 1.12f, os.y * 1.12f);
                    glow.setColor(sf::Color(255, 255, 0, 120));
                    sf::RenderStates states;
                    states.blendMode = sf::BlendAdd;
                    window.draw(glow, states);
                }
                window.draw(actionSprite);
            } else {
                // Fallback: do not draw text per requirement
            }
        }
        
        // Render return button
        if (returnSpriteLoaded) {
            sf::Vector2f originalScale = returnSprite.getScale();
            if (returnHovered) returnSprite.setScale(originalScale.x * 0.94f, originalScale.y * 0.94f);
            window.draw(returnSprite);
            if (returnHovered) returnSprite.setScale(originalScale);
        } else {
            window.draw(returnButtonRect);
            window.draw(returnButton);
        }
        
        // Render server code display
        window.draw(serverCodeDisplay);

        // Render settings gear (admin only)
        if (isAdmin) {
            if (m_settingsSpriteLoaded) {
                sf::Vector2f base = m_settingsSprite.getScale();
                if (m_settingsHovered) m_settingsSprite.setScale(base.x * 1.06f, base.y * 1.06f);
                window.draw(m_settingsSprite);
                if (m_settingsHovered) m_settingsSprite.setScale(base);
            } else {
                window.draw(m_settingsRect);
            }
        }

        // Render settings panel if open (admin only)
        if (isAdmin && m_showSettings) {
            renderSettingsPanel(window);
        }
    }
    
    void PrivateServerLobbyState::startGame() {
        if (isAdmin) {
            std::cout << "Admin " << username << " is starting the game!" << std::endl;
            std::cout << "Sending GAME_START_REQUEST to server in room " << serverCode << std::endl;
            
            rtype::client::network::senders::send_game_start_request();
        }
    }
    
    void PrivateServerLobbyState::updateWaitingText() {
        if (isAdmin) {
            playersWaitingText.setString("Waiting for players...");
        } else {
            playersWaitingText.setString("Waiting for room host");
        }
    }
    
    void PrivateServerLobbyState::updateActionButton() {
        if (isAdmin) {
            actionButton.setString("Start Game");
            actionButtonRect.setFillColor(sf::Color(50, 100, 50, 200)); // Green for start
        }
    }

    void PrivateServerLobbyState::ensureParallaxInitialized(const sf::RenderWindow& window) {
        if (m_parallaxInitialized) return;
        m_parallaxSystem = std::make_unique<ParallaxSystem>(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        );
        if (g_gameState) {
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(g_gameState->getLevelIndex()), true);
        } else {
            // Use persisted last level when returning from a session
            m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(stateManager.getLastLevelIndex()), true);
        }
        m_overlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        m_parallaxInitialized = true;
    }
    
    void PrivateServerLobbyState::updateFromServer(uint32_t totalPlayers) {
        std::cout << "Lobby updated from server: " << totalPlayers << " total players" << std::endl;
        
        // Track for AI assist visibility
        m_totalPlayersInLobby = totalPlayers;

        // Update the display text
        if (isAdmin) {
            playersWaitingText.setString("Waiting for players... " + std::to_string(totalPlayers) + " in lobby");
        }
        // Non-admin players keep the static "Waiting for room host" text
    }

    void PrivateServerLobbyState::updateSettingsTexts() {
        static const char* DIFFS[] = {"Easy", "Normal", "Hard"};
        m_difficultyValue.setString(DIFFS[m_difficultyIndex]);
        // Plain labels without [X]/[ ]
        m_friendlyFireLabel.setString("Friendly fire");
        m_aiAssistLabel.setString("AI assist");
        m_megaDamageLabel.setString("Mega dmg");

        // Update square colors
        auto setSquare = [](sf::RectangleShape& sq, bool on) {
            if (on) {
                sq.setFillColor(sf::Color(60, 200, 80)); // green
            } else {
                sq.setFillColor(sf::Color(0, 0, 0, 0));
            }
        };
        setSquare(m_sqFriendlyFire, m_friendlyFire);
        setSquare(m_sqAIAssist, m_aiAssist);
        setSquare(m_sqMegaDamage, m_megaDamage);
    }

    void PrivateServerLobbyState::updateSettingsLayout(const sf::Vector2u& windowSize) {
        // Panel dims
    // Make the settings square (panel) bigger to better fit everything
    const float panelW = std::min(1040.f, windowSize.x * 0.96f);
    const float panelH = 440.f;
        const float panelX = (windowSize.x - panelW) * 0.5f;
        const float panelY = (windowSize.y - panelH) * 0.5f;

        m_settingsPanelRect.setSize(sf::Vector2f(panelW, panelH));
        m_settingsPanelRect.setPosition(panelX, panelY);

        // Columns
        const float padding = 24.f;
        const float colW = (panelW - padding * 4.f) / 3.f;
        const float col1X = panelX + padding;
        const float col2X = col1X + colW + padding;
        const float col3X = col2X + colW + padding;
        const float titleY = panelY + padding;
    const float rowSpacing = 48.f;
        const float rowStartY = titleY + 48.f;

        // Titles
        m_gameplayTitle.setPosition(col1X, titleY);
        m_aiTitle.setPosition(col2X, titleY);
        m_cheatsTitle.setPosition(col3X, titleY);

        // Gameplay rows
    // Difficulty: keep label on first row, move value one row below and align X with label
    m_difficultyLabel.setPosition(col1X, rowStartY);
    m_difficultyValue.setPosition(col1X, rowStartY + rowSpacing);
    m_rectDifficulty.setSize(sf::Vector2f(colW, rowSpacing + 36.f));
    m_rectDifficulty.setPosition(col1X, rowStartY - 6.f);
    m_rectDifficulty.setFillColor(sf::Color(0, 0, 0, 0)); // invisible hit area covering label+value

        // Friendly fire (moved further down)
        const float sq = 18.f;
        const float labelIndent = sq + 10.f;
        m_sqFriendlyFire.setPosition(col1X, rowStartY + 2.f * rowSpacing);
        m_friendlyFireLabel.setPosition(col1X + labelIndent, rowStartY + 2.f * rowSpacing - 2.f);
        m_rectFriendlyFire.setSize(sf::Vector2f(colW, 36.f));
        m_rectFriendlyFire.setPosition(col1X, rowStartY + 2.f * rowSpacing - 6.f);
        m_rectFriendlyFire.setFillColor(sf::Color(0, 0, 0, 0));

    // AI rows (visible only when exactly one player)
    m_sqAIAssist.setPosition(col2X, rowStartY);
    m_aiAssistLabel.setPosition(col2X + labelIndent, rowStartY - 2.f);
        m_rectAIAssist.setSize(sf::Vector2f(colW, 36.f));
        m_rectAIAssist.setPosition(col2X, rowStartY - 6.f);
        m_rectAIAssist.setFillColor(sf::Color(0, 0, 0, 0));

        // Cheats rows (only Mega damage)
        m_sqMegaDamage.setPosition(col3X, rowStartY);
        m_megaDamageLabel.setPosition(col3X + labelIndent, rowStartY - 2.f);
        m_rectMegaDamage.setSize(sf::Vector2f(colW, 36.f));
        m_rectMegaDamage.setPosition(col3X, rowStartY - 6.f);
        m_rectMegaDamage.setFillColor(sf::Color(0, 0, 0, 0));
    }

    void PrivateServerLobbyState::renderSettingsPanel(sf::RenderWindow& window) {
        window.draw(m_settingsPanelRect);
        window.draw(m_gameplayTitle);
        window.draw(m_aiTitle);
        window.draw(m_cheatsTitle);

    // Gameplay
        window.draw(m_difficultyLabel);
        window.draw(m_difficultyValue);
    window.draw(m_sqFriendlyFire);
    window.draw(m_friendlyFireLabel);

        // AI (only show when exactly 1 player is in lobby)
        if (m_totalPlayersInLobby == 1) {
            window.draw(m_sqAIAssist);
            window.draw(m_aiAssistLabel);
        }

        // Cheats
        window.draw(m_sqMegaDamage);
        window.draw(m_megaDamageLabel);
    }
}
