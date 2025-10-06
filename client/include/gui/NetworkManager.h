/**
 * @file NetworkManager.h
 * @brief Network management system for the R-TYPE client GUI
 * 
 * This file defines the NetworkManager class which provides a bridge between
 * the GUI system and the network communication layer. It manages network
 * connections, handles background networking operations, and provides
 * a clean interface for GUI states to interact with the network.
 * 
 * Key features:
 * - Non-blocking network operations
 * - Thread-safe network state management
 * - Integration with GUI state system
 * - Background network loop handling
 * - Connection status monitoring
 * 
 * @author R-Type Development Team
 * @date 2024
 */

#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include "network.h"

namespace rtype::client::gui {
    
    /**
     * @brief Manages network operations for the GUI system
     * 
     * The NetworkManager provides a high-level interface for GUI states to
     * interact with the network layer. It handles connection management,
     * background network operations, and thread-safe communication.
     * 
     * Usage example:
     * @code
     * NetworkManager networkManager;
     * networkManager.connectToServer("127.0.0.1", 8080, "PlayerName", 0);
     * 
     * // In update loop
     * networkManager.update();
     * 
     * if (networkManager.isConnected()) {
     *     // Handle connected state
     * }
     * @endcode
     */
    class NetworkManager {
    public:
        /**
         * @brief Constructs a new NetworkManager
         */
        NetworkManager();
        
        /**
         * @brief Destructor - ensures proper cleanup of network resources
         */
        ~NetworkManager();
        
        /**
         * @brief Starts a connection to the specified server
         * @param serverIP The IP address of the server to connect to
         * @param serverPort The port number of the server
         * @param playerName The name of the player connecting
         * @param roomId The ID of the room to join (0 for public servers)
         * @return True if connection initiation was successful, false otherwise
         */
        bool connectToServer(const std::string& serverIP, int serverPort, 
                           const std::string& playerName, uint32_t roomId);
        
        /**
         * @brief Disconnects from the current server
         */
        void disconnect();
        
        /**
         * @brief Updates the network manager - processes network operations
         * 
         * This method should be called regularly from the main update loop
         * to process incoming and outgoing network traffic.
         */
        void update();
        
        /**
         * @brief Checks if currently connected to a server
         * @return True if connected, false otherwise
         */
        bool isConnected() const;
        
        /**
         * @brief Gets the current connection status as a string
         * @return String describing the current connection state
         */
        std::string getConnectionStatus() const;
        
        /**
         * @brief Starts the background network processing
         * 
         * This starts a background thread that continuously processes
         * network send/receive operations when connected.
         */
        void startNetworkLoop();
        
        /**
         * @brief Stops the background network processing
         */
        void stopNetworkLoop();
        
    private:
        /**
         * @brief Background network processing function
         * 
         * This function runs in a separate thread and continuously
         * processes network operations while connected.
         */
        void networkLoop();
        
        std::atomic<bool> connected;           ///< Current connection status
        std::atomic<bool> networkRunning;     ///< Network loop running status
        std::unique_ptr<std::thread> networkThread; ///< Background network thread
        std::mutex networkMutex;              ///< Mutex for thread-safe operations
        
        std::string currentServerIP;          ///< Currently connected server IP
        int currentServerPort;                ///< Currently connected server port
        std::string currentPlayerName;        ///< Current player name
        uint32_t currentRoomId;              ///< Current room ID
    };
    
} // namespace rtype::client::gui
