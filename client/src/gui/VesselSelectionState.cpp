/**
 * @file VesselSelectionState.cpp
 * @brief Implementation of the vessel selection interface
 */

#include "gui/VesselSelectionState.h"
#include "gui/AssetPaths.h"
#include "gui/TextureCache.h"
#include "gui/FontManager.h"
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
        hoveredVesselIndex(-1),
        animationTime(0.0f),
        confirmHovered(false),
        backHovered(false)
    {
        // Load font
        font = FontManager::getInstance().getDefaultFont();
        
        // Initialize parallax background (will be sized on first render)
        // ParallaxSystem requires window dimensions, so we'll initialize it later
        parallax = nullptr;
        
        // Setup title
        titleText.setFont(font);
        titleText.setString("SELECT YOUR VESSEL");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(2.0f);
        
        // Setup confirm button
        confirmButtonText.setFont(font);
        confirmButtonText.setString("CONFIRM");
        confirmButtonText.setCharacterSize(32);
        confirmButtonText.setFillColor(sf::Color::White);
        
        confirmButton.setSize(sf::Vector2f(200.0f, 60.0f));
        confirmButton.setFillColor(sf::Color(0, 150, 0));
        confirmButton.setOutlineColor(sf::Color::White);
        confirmButton.setOutlineThickness(2.0f);
        
        // Setup back button
        backButtonText.setFont(font);
        backButtonText.setString("BACK");
        backButtonText.setCharacterSize(32);
        backButtonText.setFillColor(sf::Color::White);
        
        backButton.setSize(sf::Vector2f(150.0f, 60.0f));
        backButton.setFillColor(sf::Color(150, 0, 0));
        backButton.setOutlineColor(sf::Color::White);
        backButton.setOutlineThickness(2.0f);
        
        // Setup vessel card positions (2x2 grid)
        vesselCardPositions[0] = sf::Vector2f(300.0f, 300.0f);  // Top-left
        vesselCardPositions[1] = sf::Vector2f(700.0f, 300.0f);  // Top-right
        vesselCardPositions[2] = sf::Vector2f(300.0f, 550.0f);  // Bottom-left
        vesselCardPositions[3] = sf::Vector2f(700.0f, 550.0f);  // Bottom-right
        
        // Load vessel sprites from single sprite sheet
        // PLAYER.gif is 166x86 pixels: 5 frames × 5 rows (each frame is 33x17)
        // Each vessel class is on a different row
        using namespace rtype::client::assets::player;
        const std::array<int, 4> vesselRows = {
            0,   // Row 0, y=0: Crimson Striker (red)
            17,  // Row 1, y=17: Azure Phantom (blue)
            34,  // Row 2, y=34: Emerald Titan (green)
            51   // Row 3, y=51: Solar Guardian (yellow)
        };
        
        // Load the single shared texture once (all vessels use PLAYER.gif)
        if (!vesselTextures[0].loadFromFile(PLAYER_SPRITE)) {
            std::cerr << "[VesselSelectionState] Failed to load vessel sprite sheet: " 
                      << PLAYER_SPRITE << std::endl;
            return;
        }
        
        // Setup each vessel sprite with correct row from sprite sheet
        for (size_t i = 0; i < 4; ++i) {
            vesselTextures[i] = vesselTextures[0];  // Share the same texture
            vesselSprites[i].setTexture(vesselTextures[i]);
            vesselSprites[i].setTextureRect(sf::IntRect(0, vesselRows[i], 33, 17));  // Select correct row
            vesselSprites[i].setScale(2.0f, 2.0f);
            
            // Center sprite
            vesselSprites[i].setOrigin(16.5f, 8.5f);  // Half of 33x17
        }
    }

    void VesselSelectionState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                static_cast<float>(event.mouseButton.y));
            
            // Check vessel card clicks
            for (size_t i = 0; i < 4; ++i) {
                sf::FloatRect cardBounds(
                    vesselCardPositions[i].x - 150.0f,
                    vesselCardPositions[i].y - 100.0f,
                    300.0f,
                    200.0f
                );
                
                if (cardBounds.contains(mousePos)) {
                    selectedVessel = static_cast<rtype::common::components::VesselType>(i);
                    std::cout << "[VesselSelectionState] Selected vessel: " << i << std::endl;
                    return;
                }
            }
            
            // Check confirm button click
            if (confirmButton.getGlobalBounds().contains(mousePos)) {
                confirmSelection();
                return;
            }
            
            // Check back button click
            if (backButton.getGlobalBounds().contains(mousePos)) {
                goBack();
                return;
            }
        }
        
        // ESC key to go back
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            goBack();
        }
    }

    void VesselSelectionState::update(float deltaTime) {
        // Update parallax background
        if (parallax) {
            parallax->update(deltaTime);
        }
        
        // Update animation time
        animationTime += deltaTime;
        
        // Hover states are calculated during handleEvent for simplicity
        hoveredVesselIndex = -1;
        confirmHovered = false;
        backHovered = false;
    }

    void VesselSelectionState::render(sf::RenderWindow& window) {
        // Initialize parallax on first render
        if (!parallax) {
            parallax = std::make_unique<ParallaxSystem>(
                static_cast<float>(window.getSize().x),
                static_cast<float>(window.getSize().y)
            );
        }
        
        // Draw parallax background
        if (parallax) {
            parallax->render(window);
        }
        
        // Center title
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(
            (window.getSize().x - titleBounds.width) / 2.0f - titleBounds.left,
            50.0f
        );
        window.draw(titleText);
        
        // Draw vessel cards
        for (size_t i = 0; i < 4; ++i) {
            bool isSelected = (static_cast<int>(selectedVessel) == static_cast<int>(i));
            bool isHovered = (hoveredVesselIndex == static_cast<int>(i));
            
            renderVesselCard(
                window,
                static_cast<rtype::common::components::VesselType>(i),
                vesselCardPositions[i],
                isSelected,
                isHovered
            );
        }
        
        // Position and draw confirm button
        confirmButton.setPosition(
            window.getSize().x / 2.0f - confirmButton.getSize().x / 2.0f,
            window.getSize().y - 100.0f
        );
        
        if (confirmHovered) {
            confirmButton.setFillColor(sf::Color(0, 200, 0));
        } else {
            confirmButton.setFillColor(sf::Color(0, 150, 0));
        }
        
        window.draw(confirmButton);
        
        sf::FloatRect confirmTextBounds = confirmButtonText.getLocalBounds();
        confirmButtonText.setPosition(
            confirmButton.getPosition().x + (confirmButton.getSize().x - confirmTextBounds.width) / 2.0f - confirmTextBounds.left,
            confirmButton.getPosition().y + (confirmButton.getSize().y - confirmTextBounds.height) / 2.0f - confirmTextBounds.top
        );
        window.draw(confirmButtonText);
        
        // Position and draw back button
        backButton.setPosition(50.0f, window.getSize().y - 100.0f);
        
        if (backHovered) {
            backButton.setFillColor(sf::Color(200, 0, 0));
        } else {
            backButton.setFillColor(sf::Color(150, 0, 0));
        }
        
        window.draw(backButton);
        
        sf::FloatRect backTextBounds = backButtonText.getLocalBounds();
        backButtonText.setPosition(
            backButton.getPosition().x + (backButton.getSize().x - backTextBounds.width) / 2.0f - backTextBounds.left,
            backButton.getPosition().y + (backButton.getSize().y - backTextBounds.height) / 2.0f - backTextBounds.top
        );
        window.draw(backButtonText);
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

    void VesselSelectionState::renderVesselCard(
        sf::RenderWindow& window,
        rtype::common::components::VesselType vesselType,
        const sf::Vector2f& position,
        bool isSelected,
        bool isHovered
    ) {
        // Card background
        sf::RectangleShape cardBackground(sf::Vector2f(300.0f, 200.0f));
        cardBackground.setPosition(position.x - 150.0f, position.y - 100.0f);
        
        if (isSelected) {
            cardBackground.setFillColor(sf::Color(50, 50, 150, 200));
            cardBackground.setOutlineColor(sf::Color::Cyan);
            cardBackground.setOutlineThickness(4.0f);
        } else if (isHovered) {
            cardBackground.setFillColor(sf::Color(70, 70, 70, 200));
            cardBackground.setOutlineColor(sf::Color::White);
            cardBackground.setOutlineThickness(3.0f);
        } else {
            cardBackground.setFillColor(sf::Color(40, 40, 40, 200));
            cardBackground.setOutlineColor(sf::Color(100, 100, 100));
            cardBackground.setOutlineThickness(2.0f);
        }
        
        window.draw(cardBackground);
        
        // Vessel sprite
        int vesselIndex = static_cast<int>(vesselType);
        if (vesselIndex >= 0 && vesselIndex < 4) {
            vesselSprites[vesselIndex].setPosition(position.x, position.y - 30.0f);
            
            // Animate selected vessel
            if (isSelected) {
                float scale = 2.0f + 0.1f * std::sin(animationTime * 3.0f);
                vesselSprites[vesselIndex].setScale(scale, scale);
            } else {
                vesselSprites[vesselIndex].setScale(2.0f, 2.0f);
            }
            
            window.draw(vesselSprites[vesselIndex]);
        }
        
        // Vessel name
        const std::array<std::string, 4> vesselNames = {
            "Crimson Striker",
            "Azure Phantom",
            "Emerald Titan",
            "Solar Guardian"
        };
        
        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(vesselNames[vesselIndex]);
        nameText.setCharacterSize(20);
        nameText.setFillColor(sf::Color::White);
        
        sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setPosition(
            position.x - nameBounds.width / 2.0f - nameBounds.left,
            position.y + 50.0f
        );
        window.draw(nameText);
        
        // Vessel type label
        const std::array<std::string, 4> vesselTypes = {
            "Balanced",
            "Speed",
            "Power",
            "Defense"
        };
        
        sf::Text typeText;
        typeText.setFont(font);
        typeText.setString(vesselTypes[vesselIndex]);
        typeText.setCharacterSize(16);
        typeText.setFillColor(sf::Color(200, 200, 200));
        
        sf::FloatRect typeBounds = typeText.getLocalBounds();
        typeText.setPosition(
            position.x - typeBounds.width / 2.0f - typeBounds.left,
            position.y + 75.0f
        );
        window.draw(typeText);
    }

    void VesselSelectionState::renderStatBar(
        sf::RenderWindow& window,
        const sf::Vector2f& position,
        const std::string& label,
        float value,
        const sf::Color& color
    ) {
        // Label
        sf::Text labelText;
        labelText.setFont(font);
        labelText.setString(label);
        labelText.setCharacterSize(14);
        labelText.setFillColor(sf::Color::White);
        labelText.setPosition(position);
        window.draw(labelText);
        
        // Background bar
        sf::RectangleShape bgBar(sf::Vector2f(100.0f, 10.0f));
        bgBar.setPosition(position.x + 80.0f, position.y);
        bgBar.setFillColor(sf::Color(50, 50, 50));
        window.draw(bgBar);
        
        // Value bar
        float barWidth = (value / 2.0f) * 100.0f; // Normalize to 0-100
        sf::RectangleShape valueBar(sf::Vector2f(barWidth, 10.0f));
        valueBar.setPosition(position.x + 80.0f, position.y);
        valueBar.setFillColor(color);
        window.draw(valueBar);
    }

} // namespace rtype::client::gui
