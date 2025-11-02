/**
 * @file HighscoresState.cpp
 * @brief Implementation of the SFML Highscores screen (top scores list).
 *
 * Responsibilities:
 * - Load highscores via HighscoreManager and prepare a read-only Top-N view.
 * - Render a parallax background consistent with the main menu aesthetic.
 * - Draw title, rows of entries, and a return button with hover feedback.
 * - Handle basic input: ESC/Backspace to return, click return button.
 *
 * Layout & rendering:
 * - UI is re-laid out every frame from the current window size to keep
 *   alignment responsive. For large windows, consider caching if needed.
 * - Title sits near the top center; rows are spaced uniformly below.
 * - A semi-transparent overlay darkens the background for readability.
 */

#include "gui/HighscoresState.h"
#include "gui/GUIHelper.h"
#include "gui/AssetPaths.h"
#include "gui/ParallaxSystem.h"
#include <sstream>

namespace rtype::client::gui {

/**
 * @brief Construct highscores state and set initial overlay appearance.
 * @param sm Owner state manager used to navigate back.
 */
HighscoresState::HighscoresState(StateManager& sm)
    : m_sm(sm), m_mgr("scores.json") {
    m_overlay.setFillColor(sf::Color(0, 0, 0, 160));
}

/**
 * @brief Prepare resources and UI when state becomes active.
 *
 * - Acquires the global UI font.
 * - Styles and sets the title text.
 * - Loads highscores from disk and computes the top-10 view.
 * - Creates one sf::Text per entry using the required format:
 *   NAME PLAYER <index> <score> (no separators).
 * - Loads the return button texture if available.
 */
void HighscoresState::onEnter() {
    const sf::Font& font = GUIHelper::getFont();
    m_title.setFont(font);
    m_title.setString("HIGHSCORES");
    m_title.setCharacterSize(GUIHelper::Sizes::TITLE_FONT_SIZE);
    m_title.setFillColor(GUIHelper::Colors::TEXT);
    m_title.setStyle(sf::Text::Bold);

    m_mgr.load();
    m_top = m_mgr.topN(10);

    m_rows.clear();
    for (std::size_t i = 0; i < m_top.size(); ++i) {
        const auto& e = m_top[i];
    std::ostringstream line;
    // Requested format: NAME PLAYER SCORE (no separators or dashes)
    line << e.name << " PLAYER " << e.playerIndex << " " << e.score;
        sf::Text t;
        t.setFont(font);
        t.setString(line.str());
        t.setCharacterSize(GUIHelper::Sizes::BUTTON_FONT_SIZE);
        t.setFillColor(GUIHelper::Colors::TEXT);
        m_rows.push_back(t);
    }

    // Load return button sprite (optional asset)
    if (m_returnTex.loadFromFile(rtype::client::assets::ui::RETURN_BUTTON)) {
        m_returnTex.setSmooth(true);
        m_returnSprite.setTexture(m_returnTex);
        auto sz = m_returnTex.getSize();
        m_returnSprite.setOrigin(sz.x * 0.5f, sz.y * 0.5f);
    }
}

/**
 * @brief Cleanup hook when leaving the state.
 * Currently a no-op; textures/UI are released implicitly.
 */
void HighscoresState::onExit() {
    // nothing for now
}

/**
 * @brief Compute positions and sizing of title, rows and return button.
 * @param size Current window size in pixels.
 */
void HighscoresState::layout(const sf::Vector2u& size) {
    // Overlay covers whole screen
    m_overlay.setSize(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));

    // Title centered at top
    GUIHelper::centerText(m_title, size.x * 0.5f, size.y * 0.18f);

    // Rows centered below
    float startY = size.y * 0.28f;
    float lineH = 40.0f;
    for (std::size_t i = 0; i < m_rows.size(); ++i) {
        GUIHelper::centerText(m_rows[i], size.x * 0.5f, startY + static_cast<float>(i) * lineH);
    }

    // Return button top-left area (clickable rect separate from sprite bounds)
    const float btnW = 160.0f;
    const float btnH = 90.0f;
    const float x = 12.0f + btnW * 0.5f;
    const float y = 12.0f + btnH * 0.5f;
    if (m_returnSprite.getTexture()) {
        // scale based on height
        auto sz = m_returnTex.getSize();
        if (sz.y > 0) {
            float scale = (btnH * 1.2f) / static_cast<float>(sz.y);
            m_returnSprite.setScale(scale, scale);
        }
        m_returnSprite.setPosition(x, y);
    }
    m_returnRect = sf::FloatRect(12.0f, 12.0f, btnW, btnH);
}

/**
 * @brief Lazy-initialize the parallax system using current window size.
 * @param window Render window (provides size for viewport setup).
 */
void HighscoresState::ensureParallaxInitialized(const sf::RenderWindow& window) {
    if (m_parallaxInitialized) return;
    m_parallaxSystem = std::make_unique<ParallaxSystem>(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    // Match main menu behavior: theme based on last level index persisted in state manager
    m_parallaxSystem->setTheme(ParallaxSystem::themeFromLevel(m_sm.getLastLevelIndex()), true);
    m_parallaxInitialized = true;
}

/**
 * @brief Handle basic input for navigation and hover feedback.
 *
 * - ESC/Backspace: pop the state and return to previous screen.
 * - Mouse move: update hover status for return button.
 * - Left click on return area: pop the state.
 */
void HighscoresState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::BackSpace) {
            m_sm.popState();
            return;
        }
    }
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        m_returnHovered = m_returnRect.contains(mouse);
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (m_returnRect.contains(mouse)) {
            m_sm.popState();
            return;
        }
    }
}

/**
 * @brief Advance parallax background state.
 * @param deltaTime Seconds since last frame.
 */
void HighscoresState::update(float deltaTime) {
    if (m_parallaxSystem) {
        m_parallaxSystem->update(deltaTime);
    }
}

/**
 * @brief Draw background, overlay, title, rows and return button.
 * @param window Render window.
 */
void HighscoresState::render(sf::RenderWindow& window) {
    layout(window.getSize());
    ensureParallaxInitialized(window);

    // Render menu-style parallax background, then dim overlay like menu
    if (m_parallaxSystem) {
        m_parallaxSystem->render(window);
    }
    window.draw(m_overlay);

    // Title and rows
    window.draw(m_title);
    for (auto& t : m_rows) window.draw(t);

    // Return button with hover scale
    if (m_returnSprite.getTexture()) {
        const float scaleMul = m_returnHovered ? 0.96f : 1.0f;
        auto s = m_returnSprite.getScale();
        m_returnSprite.setScale(s.x * scaleMul, s.y * scaleMul);
        window.draw(m_returnSprite);
        // Reset base scale so hover effect does not accumulate frame-to-frame
        m_returnSprite.setScale(s);
    }
}

} // namespace rtype::client::gui
