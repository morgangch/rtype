/**
 * @file StateManager.h
 * @brief State management system for the R-TYPE client GUI
 * 
 * This file contains the StateManager class which implements a stack-based
 * state management system. It handles state transitions, maintains the state
 * stack, and delegates events, updates, and rendering to the active state.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#ifndef CLIENT_STATE_MANAGER_HPP
#define CLIENT_STATE_MANAGER_HPP

#include "State.h"
#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>

// Forward declaration to avoid circular dependency
namespace rtype::client::gui {
    class NetworkManager;
    
    /**
     * @brief Custom deleter for NetworkManager to handle incomplete type
     */
    struct NetworkManagerDeleter {
        void operator()(NetworkManager* ptr);
    };
    
    /**
     * @class StateManager
     * @brief Manages a stack of GUI states for the application
     * 
     * The StateManager class implements a stack-based state management system
     * that allows for clean transitions between different GUI screens. It maintains
     * a stack of states where only the top state is active and receives events.
     * 
     * Key features:
     * - Stack-based state management (LIFO - Last In, First Out)
     * - Automatic state lifecycle management (onEnter/onExit)
     * - Event delegation to the active state
     * - Support for state overlays (multiple states can be active)
     * - Memory management using smart pointers
     * 
     * Common usage patterns:
     * - pushState(): Add a new state on top (e.g., opening a settings menu)
     * - popState(): Remove the top state (e.g., closing a dialog)
     * - changeState(): Replace the current state (e.g., going from menu to game)
     * 
     * Example:
     * @code
     * StateManager manager(window);
     * manager.pushState(std::make_unique<MainMenuState>(manager));
     * // In main loop:
     * manager.handleEvent(event);
     * manager.update(deltaTime);
     * manager.render();
     * @endcode
     */
    class StateManager {
    public:
        /**
         * @brief Construct a new StateManager
         * @param window Reference to the SFML render window
         */
        explicit StateManager(sf::RenderWindow& window);
        
        /**
         * @brief Destructor - ensures proper cleanup of all states
         */
        ~StateManager();
        
        /**
         * @brief Get access to the network manager
         * @return Reference to the NetworkManager instance
         */
        NetworkManager& getNetworkManager();
        
        /**
         * @brief Push a new state onto the stack
         * @param state Unique pointer to the new state to add
         * 
         * The new state becomes the active state and receives all events.
         * The previous state remains in the stack but becomes inactive.
         * Calls onEnter() on the new state.
         */
        void pushState(std::unique_ptr<State> state);
        
        /**
         * @brief Remove the top state from the stack
         * 
         * Calls onExit() on the current state before removing it.
         * The previous state (if any) becomes active again.
         * If this was the last state, the application should exit.
         */
        void popState();
        
        /**
         * @brief Replace the current state with a new one
         * @param state Unique pointer to the new state
         * 
         * Equivalent to popState() followed by pushState().
         * Calls onExit() on the old state and onEnter() on the new state.
         */
        void changeState(std::unique_ptr<State> state);
        
        /**
         * @brief Forward an event to the active state
         * @param event The SFML event to process
         */
        void handleEvent(const sf::Event& event);
        
        /**
         * @brief Update the active state
         * @param deltaTime Time elapsed since last update (in seconds)
         */
        void update(float deltaTime);
        
        /**
         * @brief Render the active state to the window
         */
        void render();
        
        /**
         * @brief Check if the state stack is empty
         * @return True if no states are active, false otherwise
         */
        bool isEmpty() const;
        
    private:
        sf::RenderWindow& window;           ///< Reference to the render window
        std::stack<std::unique_ptr<State>> states;  ///< Stack of states (top = active)
        std::unique_ptr<NetworkManager, NetworkManagerDeleter> networkManager;  ///< Network management system
    };
}

#endif // CLIENT_STATE_MANAGER_HPP
