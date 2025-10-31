/**
 * @file HighscoresState.h
 * @brief SFML state that renders the Highscores screen (top scores list).
 *
 * Purpose:
 * - Presents a read-only leaderboard of the best runs.
 * - Uses HighscoreManager to load entries and compute a Top-N view.
 * - Renders a parallax background similar to the main menu.
 * - Provides a "Return" button to go back to the previous state.
 *
 * Rendering & UI:
 * - Title text, N rows of score entries, and a semi-transparent overlay.
 * - Mouse hover feedback on the return button; click to navigate back.
 * - Layout adapts to the window size via layout(). Call on resize.
 *
 * Notes:
 * - This state assumes an active SFML window and font/resources are available.
 * - All operations are performed on the main thread (not thread-safe).
 */

#ifndef CLIENT_HIGHSCORES_STATE_HPP
#define CLIENT_HIGHSCORES_STATE_HPP

#include "State.h"
#include "StateManager.h"
#include "HighscoreManager.h"
#include "ParallaxSystem.h"
#include <SFML/Graphics.hpp>
#include <vector>

namespace rtype::client::gui {

/**
 * @brief State implementation responsible for displaying highscores.
 *
 * Lifecycle expectations:
 * - onEnter(): typically loads scores from disk and prepares text rows.
 * - handleEvent(): processes mouse/keyboard to handle return button.
 * - update(): advances background parallax and hover states.
 * - render(): draws background, overlay, title, rows, and return button.
 * - onExit(): releases transient UI resources if needed.
 */
class HighscoresState : public State {
public:
    /**
     * @brief Construct the highscores state.
     * @param sm Reference to the owning StateManager for navigation.
     */
    explicit HighscoresState(StateManager& sm);
    ~HighscoresState() override = default;

    /**
     * @brief Handle SFML window/input events (mouse, keyboard, resize).
     * @param event Incoming SFML event.
     *
     * Typical behaviors:
     * - Update hover for the return button on mouse move.
     * - Trigger navigation on mouse click within return area or on ESC.
     * - Recompute layout on resize events.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Update per-frame logic (parallax, hover animations, timers).
     * @param deltaTime Time elapsed since the last frame, in seconds.
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the highscores screen to the provided window.
     * @param window Render target window.
     */
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Called when the state becomes active.
     *
     * Expected actions:
     * - Load/pull highscores via HighscoreManager and compute top entries.
     * - Initialize UI elements (title, rows, overlay, return button).
     * - Ensure parallax background is ready.
     */
    void onEnter() override;

    /**
     * @brief Called when the state is about to be deactivated.
     *
     * May release textures or transient UI objects if needed.
     */
    void onExit() override;

private:
    StateManager& m_sm;
    HighscoreManager m_mgr;
    std::vector<HighscoreEntry> m_top;

    // Parallax background like the main menu
    std::unique_ptr<ParallaxSystem> m_parallaxSystem;
    bool m_parallaxInitialized{false};

    sf::Text m_title;
    std::vector<sf::Text> m_rows;
    sf::RectangleShape m_overlay;

    // Return button sprite
    sf::Texture m_returnTex;
    sf::Sprite m_returnSprite;
    sf::FloatRect m_returnRect; // clickable area
    bool m_returnHovered{false};

    /**
     * @brief Recompute positions/sizes of all UI elements for a window size.
     * @param size Window pixel size used for layout.
     */
    void layout(const sf::Vector2u& size);

    /**
     * @brief Lazily allocate and prepare the parallax background system.
     * @param window Reference window for viewport-dependent initialization.
     */
    void ensureParallaxInitialized(const sf::RenderWindow& window);
};

} // namespace rtype::client::gui

#endif // CLIENT_HIGHSCORES_STATE_HPP
