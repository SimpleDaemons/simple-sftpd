#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <functional>
#include "simple-ftpd/platform.hpp"

namespace simple_ftpd {

// Forward declarations
class FTPConnection;
class FTPUser;
class FTPVirtualHost;
class FTPServerConfig;
class Logger;

/**
 * @brief FTP Server main class
 * 
 * This class manages the main FTP server functionality including:
 * - Listening for incoming connections
 * - Managing multiple FTP connections
 * - User authentication and authorization
 * - Virtual host support
 * - SSL/TLS support
 */
class FTPServer {
public:
    /**
     * @brief Constructor
     * @param config Server configuration
     */
    explicit FTPServer(std::shared_ptr<FTPServerConfig> config);
    
    /**
     * @brief Destructor
     */
    ~FTPServer();
    
    /**
     * @brief Start the FTP server
     * @return true if started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stop the FTP server
     */
    void stop();
    
    /**
     * @brief Check if server is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_.load(); }
    
    /**
     * @brief Get server status information
     * @return Status string
     */
    std::string getStatus() const;
    
    /**
     * @brief Get active connection count
     * @return Number of active connections
     */
    size_t getActiveConnectionCount() const;
    
    /**
     * @brief Get server configuration
     * @return Server configuration
     */
    std::shared_ptr<FTPServerConfig> getConfig() const { return config_; }
    
    /**
     * @brief Reload configuration
     * @return true if reloaded successfully, false otherwise
     */
    bool reloadConfig();
    
    /**
     * @brief Get virtual host by name
     * @param hostname Hostname to look up
     * @return Virtual host if found, nullptr otherwise
     */
    std::shared_ptr<FTPVirtualHost> getVirtualHost(const std::string& hostname) const;
    
    /**
     * @brief Get default virtual host
     * @return Default virtual host
     */
    std::shared_ptr<FTPVirtualHost> getDefaultVirtualHost() const;
    
    /**
     * @brief Add virtual host
     * @param virtual_host Virtual host to add
     */
    void addVirtualHost(std::shared_ptr<FTPVirtualHost> virtual_host);
    
    /**
     * @brief Remove virtual host
     * @param hostname Hostname to remove
     */
    void removeVirtualHost(const std::string& hostname);
    
    /**
     * @brief Get user by username
     * @param username Username to look up
     * @return User if found, nullptr otherwise
     */
    std::shared_ptr<FTPUser> getUser(const std::string& username) const;
    
    /**
     * @brief Add user
     * @param user User to add
     */
    void addUser(std::shared_ptr<FTPUser> user);
    
    /**
     * @brief Remove user
     * @param username Username to remove
     */
    void removeUser(const std::string& username);
    
    /**
     * @brief Set connection callback
     * @param callback Function to call when new connection is established
     */
    void setConnectionCallback(std::function<void(std::shared_ptr<FTPConnection>)> callback);
    
    /**
     * @brief Set disconnection callback
     * @param callback Function to call when connection is closed
     */
    void setDisconnectionCallback(std::function<void(std::shared_ptr<FTPConnection>)> callback);
    
    /**
     * @brief Set authentication callback
     * @param callback Function to call for user authentication
     */
    void setAuthenticationCallback(std::function<bool(const std::string&, const std::string&)> callback);
    
    /**
     * @brief Set authorization callback
     * @param callback Function to call for access authorization
     */
    void setAuthorizationCallback(std::function<bool(const std::string&, const std::string&, const std::string&)> callback);

private:
    /**
     * @brief Main server loop
     */
    void serverLoop();
    
    /**
     * @brief Accept new connections
     */
    void acceptConnections();
    
    /**
     * @brief Handle new connection
     * @param client_socket Client socket
     * @param client_addr Client address
     */
    void handleNewConnection(socket_t client_socket, const std::string& client_addr);
    
    /**
     * @brief Clean up closed connections
     */
    void cleanupConnections();
    
    /**
     * @brief Load configuration
     * @return true if loaded successfully, false otherwise
     */
    bool loadConfiguration();
    
    /**
     * @brief Load virtual hosts
     * @return true if loaded successfully, false otherwise
     */
    bool loadVirtualHosts();
    
    /**
     * @brief Load users
     * @return true if loaded successfully, false otherwise
     */
    bool loadUsers();
    
    /**
     * @brief Setup SSL context
     * @return true if setup successfully, false otherwise
     */
    bool setupSSL();
    
    /**
     * @brief Setup signal handlers
     */
    void setupSignalHandlers();
    
    /**
     * @brief Signal handler for graceful shutdown
     * @param signal Signal number
     */
    static void signalHandler(int signal);
    
    // Server configuration
    std::shared_ptr<FTPServerConfig> config_;
    
    // Server state
    std::atomic<bool> running_;
    std::atomic<bool> shutdown_requested_;
    
    // Network components
    socket_t listen_socket_;
    std::string bind_address_;
    uint16_t bind_port_;
    
    // SSL components
    bool ssl_enabled_;
    void* ssl_ctx_; // OpenSSL context
    
    // Virtual hosts and users
    std::map<std::string, std::shared_ptr<FTPVirtualHost>> virtual_hosts_;
    std::shared_ptr<FTPVirtualHost> default_virtual_host_;
    std::map<std::string, std::shared_ptr<FTPUser>> users_;
    
    // Active connections
    std::vector<std::shared_ptr<FTPConnection>> connections_;
    mutable std::mutex connections_mutex_;
    
    // Server thread
    std::thread server_thread_;
    
    // Callbacks
    std::function<void(std::shared_ptr<FTPConnection>)> connection_callback_;
    std::function<void(std::shared_ptr<FTPConnection>)> disconnection_callback_;
    std::function<bool(const std::string&, const std::string&)> authentication_callback_;
    std::function<bool(const std::string&, const std::string&, const std::string&)> authorization_callback_;
    
    // Logger
    std::shared_ptr<Logger> logger_;
    
    // Static instance for signal handling
    static FTPServer* instance_;
};

} // namespace simple_ftpd
