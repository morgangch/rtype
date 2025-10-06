/**
 * @file StateManager.cpp
 * @brief Implementation of the StateManager class
 * 
 * This file implements the StateManager class methods for managing GUI states
 * in the R-TYPE client. It handles state transitions, lifecycle management,
 * and event delegation using a stack-based approach. The StateManager also
 * integrates network functionality through the NetworkManager.
 * 
 * @author R-TYPE Development Team
 * @date 2025
 */

#include "gui/StateManager.hpp"
#include "gui/NetworkManager.hpp"

namespace rtype::client::gui {
    
    // Custom deleter implementation
    void NetworkManagerDeleter::operator()(NetworkManager* ptr) {
        delete ptr;
    }
    
    StateManager::StateManager(sf::RenderWindow& window) 
        : window(window), networkManager(new NetworkManager(), NetworkManagerDeleter()) {}
    
    StateManager::~StateManager() {
        // Clean up network connections
        if (networkManager) {
            networkManager->disconnect();
        }
    }
    
    NetworkManager& StateManager::getNetworkManager() {
        return *networkManager;
    }
    
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
        // Update network manager first
        if (networkManager) {
            networkManager->update();
        }
        
        // Then update the active state
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
