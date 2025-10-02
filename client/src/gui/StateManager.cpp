/**
 * @file StateManager.cpp
 * @brief Implementation of the StateManager class
 * 
 * This file implements the StateManager class methods for managing GUI states
 * in the R-TYPE client. It handles state transitions, lifecycle management,
 * and event delegation using a stack-based approach.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/StateManager.hpp"

namespace rtype::client::gui {
    
    StateManager::StateManager(sf::RenderWindow& window) : window(window) {}
    
    void StateManager::pushState(std::unique_ptr<State> state) {
        // Pause the current state by calling onExit()
        if (!states.empty()) {
            states.top()->onExit();
        }
        
        states.push(std::move(state));
        states.top()->onEnter();
    }
    
    void StateManager::popState() {
        if (!states.empty()) {
            states.top()->onExit();
            states.pop();
            
            if (!states.empty()) {
                states.top()->onEnter();
            }
        }
    }
    
    void StateManager::changeState(std::unique_ptr<State> state) {
        if (!states.empty()) {
            states.top()->onExit();
            states.pop();
        }
        
        states.push(std::move(state));
        states.top()->onEnter();
    }
    
    void StateManager::handleEvent(const sf::Event& event) {
        if (!states.empty()) {
            states.top()->handleEvent(event);
        }
    }
    
    void StateManager::update(float deltaTime) {
        if (!states.empty()) {
            states.top()->update(deltaTime);
        }
    }
    
    void StateManager::render() {
        if (!states.empty()) {
            states.top()->render(window);
        }
    }
    
    bool StateManager::isEmpty() const {
        return states.empty();
    }
}
