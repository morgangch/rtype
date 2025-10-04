/**
 * @file NetworkManager.cpp
 * @brief Implementation of the network management system for the R-TYPE client GUI
 * 
 * This file contains the implementation of the NetworkManager class, providing
 * thread-safe network operations and integration between the GUI system and
 * the underlying network communication layer.
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#include "gui/NetworkManager.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

namespace rtype::client::gui {
    
    NetworkManager::NetworkManager() 
        : connected(false), networkRunning(false), networkThread(nullptr),
          currentServerPort(0), currentRoomId(0) {
    }
    
    NetworkManager::~NetworkManager() {
        disconnect();
        stopNetworkLoop();
    }
    
    bool NetworkManager::connectToServer(const std::string& serverIP, int serverPort, 
                                       const std::string& playerName, uint32_t roomId) {
        std::lock_guard<std::mutex> lock(networkMutex);
        
        // Disconnect if already connected
        if (connected.load()) {
            disconnect();
        }
        
        std::cout << "[NetworkManager] Attempting to connect to " << serverIP 
                  << ":" << serverPort << " as '" << playerName << "'" << std::endl;
        
        // Use the existing network system to establish connection
        int result = rtype::client::network::start_room_connection(serverIP, serverPort, playerName, roomId);
        
        if (result == 0 && rtype::client::network::udp_fd > 0) {
            // Store connection details
            currentServerIP = serverIP;
            currentServerPort = serverPort;
            currentPlayerName = playerName;
            currentRoomId = roomId;
            
            connected.store(true);
            
            // Start the background network loop
            startNetworkLoop();
            
            std::cout << "[NetworkManager] Successfully connected to server" << std::endl;
            return true;
        } else {
            std::cerr << "[NetworkManager] Failed to connect to server" << std::endl;
            return false;
        }
    }
    
    void NetworkManager::disconnect() {
        if (connected.load()) {
            std::cout << "[NetworkManager] Disconnecting from server" << std::endl;
            
            stopNetworkLoop();
            
            // Close the UDP socket if it's open
            if (rtype::client::network::udp_fd > 0) {
                close(rtype::client::network::udp_fd);
                rtype::client::network::udp_fd = 0;
            }
            
            connected.store(false);
            
            std::cout << "[NetworkManager] Disconnected from server" << std::endl;
        }
    }
    
    void NetworkManager::update() {
        // This method can be used for GUI-side network state updates
        // The actual network processing happens in the background thread
        
        if (!connected.load() && networkRunning.load()) {
            // Connection lost, stop network loop
            stopNetworkLoop();
        }
    }
    
    bool NetworkManager::isConnected() const {
        return connected.load();
    }
    
    std::string NetworkManager::getConnectionStatus() const {
        if (connected.load()) {
            return "Connected to " + currentServerIP + ":" + std::to_string(currentServerPort) 
                   + " (Room: " + std::to_string(currentRoomId) + ")";
        } else {
            return "Not connected";
        }
    }
    
    void NetworkManager::startNetworkLoop() {
        if (networkRunning.load()) {
            return; // Already running
        }
        
        networkRunning.store(true);
        networkThread = std::make_unique<std::thread>(&NetworkManager::networkLoop, this);
        
        std::cout << "[NetworkManager] Started background network loop" << std::endl;
    }
    
    void NetworkManager::stopNetworkLoop() {
        if (networkRunning.load()) {
            std::cout << "[NetworkManager] Stopping background network loop" << std::endl;
            
            networkRunning.store(false);
            
            if (networkThread && networkThread->joinable()) {
                networkThread->join();
            }
            networkThread.reset();
            
            std::cout << "[NetworkManager] Background network loop stopped" << std::endl;
        }
    }
    
    void NetworkManager::networkLoop() {
        std::cout << "[NetworkManager] Network loop started" << std::endl;
        
        while (networkRunning.load() && connected.load()) {
            // Process network operations using the existing network system
            if (rtype::client::network::udp_fd > 0) {
                try {
                    // Receive packets
                    rtype::client::network::loop_recv();
                    
                    // Send packets
                    rtype::client::network::loop_send();
                } catch (const std::exception& e) {
                    std::cerr << "[NetworkManager] Network loop error: " << e.what() << std::endl;
                    connected.store(false);
                    break;
                }
            } else {
                // Connection lost
                std::cerr << "[NetworkManager] UDP socket invalid, connection lost" << std::endl;
                connected.store(false);
                break;
            }
            
            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        std::cout << "[NetworkManager] Network loop ended" << std::endl;
    }
    
} // namespace rtype::client::gui
