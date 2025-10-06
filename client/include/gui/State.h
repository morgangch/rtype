/**
 * @file @file State.h
 * @brief Abstract base class for the state pattern implementation in R-TYPE GUI
 * 
 * This file defines the State interface which serves as the base class for all
 * GUI states in the R-TYPE client application. It implements the state pattern
 * to manage different screens and user interfaces in a clean, organized manner.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP

#include <SFML/Graphics.hpp>

namespace rtype::client::gui {
    
    /**
     * @class State
     * @brief Abstract base class for all GUI states
     * 
     * The State class defines the interface that all concrete GUI states must implement.
     * It follows the state pattern to allow the application to change behavior based
     * on the current state (e.g., main menu, game lobby, settings, etc.).
     * 
     * Each state is responsible for:
     * - Handling user input events
     * - Updating its internal state and animations
     * - Rendering itself to the screen
     * - Managing state transitions (entry/exit)
     * 
     * Concrete implementations include:
     * - MainMenuState: The main game menu
     * - PublicServerState: Public server selection screen
     * - PrivateServerState: Private server creation/joining screen
     * - PrivateServerLobbyState: Lobby for private servers
     * 
     * @see StateManager for state management and transitions
     */
    class State {
    public:
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes
         */
        virtual ~State() = default;
        
        /**
         * @brief Handle input events for this state
         * @param event The SFML event to process (keyboard, mouse, window events)
         */
        virtual void handleEvent(const sf::Event& event) = 0;
        
        /**
         * @brief Update the state's internal logic and animations
         * @param deltaTime Time elapsed since last update (in seconds)
         */
        virtual void update(float deltaTime) = 0;
        
        /**
         * @brief Render the state to the screen
         * @param window The SFML render window to draw to
         */
        virtual void render(sf::RenderWindow& window) = 0;
        
        /**
         * @brief Called when this state becomes active
         * Override this method to perform initialization when the state is entered
         */
        virtual void onEnter() {}
        
        /**
         * @brief Called when this state becomes inactive
         * Override this method to perform cleanup when the state is exited
         */
        virtual void onExit() {}
    };
}

#endif // CLIENT_STATE_HPP
