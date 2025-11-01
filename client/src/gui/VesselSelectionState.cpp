/**
 * @file VesselSelectionState.cpp
 * @brief Implementation of the vessel selection interface
 */

#include "gui/VesselSelectionState.h"
#include "gui/AssetPaths.h"
#include "gui/TextureCache.h"
#include "gui/FontManager.h"
#include "gui/GUIHelper.h"
#include "network/network.h"
#include <iostream>
#include <sstream>
#include <cmath>

namespace rtype::client::gui {

    VesselSelectionState::VesselSelectionState(
        StateManager& stateManager,
        const std::string& username,
        const std::string& serverIp,
        int serverPort,
        uint32_t roomCode
    ) : stateManager(stateManager),
        username(username),
        serverIp(serverIp),
        serverPort(serverPort),
        roomCode(roomCode),
        selectedVessel(rtype::common::components::VesselType::CrimsonStriker),
        hoveredRowIndex(-1),
        m_returnSpriteLoaded(false),
        m_returnHovered(false),
        animationTime(0.0f)
    {
        // Font and title
        font = FontManager::getInstance().getDefaultFont();
        titleText.setFont(font);
        titleText.setString("SELECT YOUR VESSEL");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(2.0f);

        // Overlay dim like other menus
        m_overlay.setFillColor(sf::Color(0, 0, 0, 160));

        // Return button sprite (top-left)
        m_returnSpriteLoaded = m_returnTexture.loadFromFile(rtype::client::assets::ui::RETURN_BUTTON);
        if (m_returnSpriteLoaded) {
            m_returnTexture.setSmooth(true);
            m_returnSprite.setTexture(m_returnTexture);
            auto sz = m_returnTexture.getSize();
            m_returnSprite.setOrigin(sz.x * 0.5f, sz.y * 0.5f);
        }

        // Use single shared sheet with colored rows, like before
        using namespace rtype::client::assets::player;
        if (!rowSharedTexture.loadFromFile(PLAYER_SPRITE)) {
            std::cerr << "[VesselSelectionState] Failed to load player sheet: " << PLAYER_SPRITE << std::endl;
        }
        rowSharedTexture.setSmooth(true);
        const int rowOffsets[4] = {0, 17, 34, 51};
        for (int i = 0; i < 4; ++i) {
            rowSprites[i].setTexture(rowSharedTexture);
            rowSprites[i].setTextureRect(sf::IntRect(0, rowOffsets[i], 33, 17));
            rowSprites[i].setScale(2.6f, 2.6f);
            rowSprites[i].setOrigin(16.5f, 8.5f);
        }

        // Ready button texture (like lobby Ready)
        m_readyTexture.loadFromFile(rtype::client::assets::ui::READY_BUTTON);
        m_readyTexture.setSmooth(true);

        // Prepare row shapes and per-row UI
        for (int i = 0; i < 4; ++i) {
            rowBackgrounds[i].setFillColor(sf::Color(30, 30, 30, 190));
            rowBackgrounds[i].setOutlineColor(sf::Color(100, 100, 100));
            rowBackgrounds[i].setOutlineThickness(2.0f);

            // Ready button sprite (drawn only for selected row)
            rowReadySprites[i].setTexture(m_readyTexture);

            rowName[i].setFont(font);
            rowName[i].setCharacterSize(28);
            rowName[i].setFillColor(sf::Color::White);

            rowRole[i].setFont(font);
            rowRole[i].setCharacterSize(18);
            rowRole[i].setFillColor(sf::Color(200, 200, 200));

            rowShoot[i].setFont(font);
            rowShoot[i].setCharacterSize(18);
            rowShoot[i].setFillColor(sf::Color::White);

            rowCharged[i].setFont(font);
            rowCharged[i].setCharacterSize(18);
            rowCharged[i].setFillColor(sf::Color::White);

            rowSpeed[i].setFont(font);
            rowSpeed[i].setCharacterSize(18);
            rowSpeed[i].setFillColor(sf::Color(180, 220, 255));

            rowDefense[i].setFont(font);
            rowDefense[i].setCharacterSize(18);
            rowDefense[i].setFillColor(sf::Color(255, 220, 180));

            rowFireRate[i].setFont(font);
            rowFireRate[i].setCharacterSize(18);
            rowFireRate[i].setFillColor(sf::Color(180, 255, 180));
        }

        // Content per design brief
        rowName[0].setString("Crimson Striker");
        rowRole[0].setString("Balanced");
        rowShoot[0].setString("Shoot 1 dmg single");
        rowCharged[0].setString("Charged 2 dmg piercing");
        rowSpeed[0].setString("Speed 100");
        rowDefense[0].setString("Defense 3 HP");
        rowFireRate[0].setString("Fire rate 100");

        rowName[1].setString("Azure Phantom");
        rowRole[1].setString("Speed");
        rowShoot[1].setString("Shoot 0.5 dmg rapid");
        rowCharged[1].setString("Charged 1 dmg burst homing");
        rowSpeed[1].setString("Speed 120");
        rowDefense[1].setString("Defense 2 HP");
        rowFireRate[1].setString("Fire rate 150");

        rowName[2].setString("Emerald Titan");
        rowRole[2].setString("Power");
        rowShoot[2].setString("Shoot 2 dmg slow");
        rowCharged[2].setString("Charged 4 dmg AoE");
        rowSpeed[2].setString("Speed 80");
        rowDefense[2].setString("Defense 4 HP");
        rowFireRate[2].setString("Fire rate 70");

        rowName[3].setString("Solar Guardian");
        rowRole[3].setString("Defense");
        rowShoot[3].setString("Shoot 0.5 dmg shotgun");
        rowCharged[3].setString("Charged Shield");
        rowSpeed[3].setString("Speed 90");
        rowDefense[3].setString("Defense 5 HP");
        rowFireRate[3].setString("Fire rate 100");
    }

    void VesselSelectionState::handleEvent(const sf::Event& event) {
        // Hover updates
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mouse(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            m_returnHovered = m_returnRect.contains(mouse);
            hoveredRowIndex = -1;
            for (int i = 0; i < 4; ++i) {
                if (rowBackgrounds[i].getGlobalBounds().contains(mouse)) {
                    hoveredRowIndex = i;
                }
                rowReadyHovered[i] = rowReadySprites[i].getGlobalBounds().contains(mouse);
            }
            return;
        }

        // Clicks
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouse(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            // Return top-left
            if (m_returnRect.contains(mouse)) {
                goBack();
                return;
            }
            // Select a row
            for (int i = 0; i < 4; ++i) {
                if (rowBackgrounds[i].getGlobalBounds().contains(mouse)) {
                    selectedVessel = static_cast<rtype::common::components::VesselType>(i);
                    return;
                }
            }
            // Ready only on selected row
            int sel = static_cast<int>(selectedVessel);
            if (sel >= 0 && sel < 4 && rowReadySprites[sel].getGlobalBounds().contains(mouse)) {
                confirmSelection();
                return;
            }
        }

        // ESC to return
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            goBack();
        }
    }

    void VesselSelectionState::update(float deltaTime) {
        if (parallax) parallax->update(deltaTime);
        animationTime += deltaTime;

        // Pulsate the selected vessel sprite slightly
        const float baseScale = 2.6f;
        for (int i = 0; i < 4; ++i) {
            if (static_cast<int>(selectedVessel) == i) {
                float pulse = 1.0f + 0.06f * std::sin(animationTime * 3.0f);
                rowSprites[i].setScale(baseScale * pulse, baseScale * pulse);
            } else {
                rowSprites[i].setScale(baseScale, baseScale);
            }
        }

        // Outline colors based on selection/hover
        for (int i = 0; i < 4; ++i) {
            if (static_cast<int>(selectedVessel) == i) {
                rowBackgrounds[i].setOutlineColor(sf::Color::Cyan);
                rowBackgrounds[i].setOutlineThickness(3.0f);
            } else if (hoveredRowIndex == i) {
                rowBackgrounds[i].setOutlineColor(sf::Color::White);
                rowBackgrounds[i].setOutlineThickness(2.5f);
            } else {
                rowBackgrounds[i].setOutlineColor(sf::Color(100, 100, 100));
                rowBackgrounds[i].setOutlineThickness(2.0f);
            }
        }
    }

    void VesselSelectionState::render(sf::RenderWindow& window) {
        // Ensure parallax exists (match menu behavior)
        if (!parallax) {
            parallax = std::make_unique<ParallaxSystem>(
                static_cast<float>(window.getSize().x),
                static_cast<float>(window.getSize().y)
            );
        }

        // Compute layout each frame for responsiveness
        layout(window.getSize());

        // Background parallax + overlay
        if (parallax) parallax->render(window);
        window.draw(m_overlay);

        // Title centered near top
        window.draw(titleText);

        // Return button (hover scale)
        if (m_returnSpriteLoaded) {
            const float mul = m_returnHovered ? 0.96f : 1.0f;
            auto s = m_returnSprite.getScale();
            m_returnSprite.setScale(s.x * mul, s.y * mul);
            window.draw(m_returnSprite);
            m_returnSprite.setScale(s);
        }

        // Draw rows
        for (int i = 0; i < 4; ++i) {
            renderRow(window, i);
        }
    }

    void VesselSelectionState::onEnter() {
        std::cout << "[VesselSelectionState] Entered vessel selection" << std::endl;
    }

    void VesselSelectionState::onExit() {
        std::cout << "[VesselSelectionState] Exiting vessel selection" << std::endl;
    }

    void VesselSelectionState::confirmSelection() {
        std::cout << "[VesselSelectionState] Confirming vessel: " 
                  << static_cast<int>(selectedVessel) << std::endl;
        
        // Connect to server with selected vessel type
        rtype::client::network::start_room_connection(
            serverIp, 
            serverPort, 
            username, 
            roomCode,
            static_cast<uint8_t>(selectedVessel)
        );
    }

    void VesselSelectionState::goBack() {
        std::cout << "[VesselSelectionState] Going back to previous state" << std::endl;
        stateManager.popState();
    }

    void VesselSelectionState::layout(const sf::Vector2u& size) {
    // Overlay covers full screen
        m_overlay.setSize(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));

        // Title centered
    GUIHelper::centerText(titleText, size.x * 0.5f, 28.0f);

        // Return button in top-left
        const float btnW = 160.0f;
        const float btnH = 90.0f;
        const float x = 12.0f + btnW * 0.5f;
        const float y = 12.0f + btnH * 0.5f;
        if (m_returnSpriteLoaded) {
            auto sz = m_returnTexture.getSize();
            if (sz.y > 0) {
                float scale = (btnH * 1.2f) / static_cast<float>(sz.y);
                m_returnSprite.setScale(scale, scale);
            }
            m_returnSprite.setPosition(x, y);
        }
        m_returnRect = sf::FloatRect(12.0f, 12.0f, btnW, btnH);

        // Rows area
        float top = 100.0f;
        float bottom = static_cast<float>(size.y) - 24.0f;
        float usable = bottom - top;
        float rowH = std::min(170.0f, usable / 5.0f);
        float gap = rowH * 0.25f;
        float startY = top + gap * 0.5f;
        float rowW = std::min(1000.0f, static_cast<float>(size.x) * 0.9f);
        float rowX = (static_cast<float>(size.x) - rowW) * 0.5f;

        for (int i = 0; i < 4; ++i) {
            float yRow = startY + static_cast<float>(i) * (rowH + gap);
            rowBackgrounds[i].setSize(sf::Vector2f(rowW, rowH));
            rowBackgrounds[i].setPosition(rowX, yRow);

            // Sprite at left
            rowSprites[i].setPosition(rowX + 60.0f, yRow + rowH * 0.5f);

            // Text columns
            float textX = rowX + 120.0f;
            float textY = yRow + 16.0f;
            rowName[i].setPosition(textX, textY - 8.0f);
            rowRole[i].setPosition(textX, textY + 26.0f);
            rowShoot[i].setPosition(textX, textY + 26.0f + 24.0f);
            rowCharged[i].setPosition(textX, textY + 26.0f + 24.0f + 22.0f);

            // Keep X as before (right side), but vertically center the three stat labels
            float statsX = rowX + rowW * 0.62f;
            float centerY = yRow + rowH * 0.5f;
            rowSpeed[i].setPosition(statsX, centerY - 26.0f);
            rowDefense[i].setPosition(statsX, centerY);
            rowFireRate[i].setPosition(statsX, centerY + 26.0f);

            // Ready button to the right, outside the square
            if (m_readyTexture.getSize().x > 0) {
                float btnX = rowX + rowW + 50.0f;
                float btnY = yRow + rowH * 0.5f;
                auto sz = m_readyTexture.getSize();
                rowReadySprites[i].setOrigin(sz.x * 0.5f, sz.y * 0.5f);
                // Scale down gently to fit
                float targetH = std::min(96.0f, rowH * 0.8f);
                if (sz.y > 0) {
                    float scale = targetH / static_cast<float>(sz.y);
                    rowReadySprites[i].setScale(scale, scale);
                }
                rowReadySprites[i].setPosition(btnX, btnY);
            }
        }
    }

    void VesselSelectionState::renderRow(sf::RenderWindow& window, int i) {
        window.draw(rowBackgrounds[i]);
        window.draw(rowSprites[i]);
        window.draw(rowName[i]);
        window.draw(rowRole[i]);
        window.draw(rowShoot[i]);
        window.draw(rowCharged[i]);
        window.draw(rowSpeed[i]);
        window.draw(rowDefense[i]);
        window.draw(rowFireRate[i]);

        if (static_cast<int>(selectedVessel) == i && m_readyTexture.getSize().x > 0) {
            // Apply slight hover scale when hovered
            float mul = rowReadyHovered[i] ? 1.06f : 1.0f;
            auto s = rowReadySprites[i].getScale();
            rowReadySprites[i].setScale(s.x * mul, s.y * mul);
            window.draw(rowReadySprites[i]);
            rowReadySprites[i].setScale(s);
        }
    }

} // namespace rtype::client::gui
