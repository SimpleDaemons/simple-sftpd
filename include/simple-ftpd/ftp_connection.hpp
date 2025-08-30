#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include "simple-ftpd/platform.hpp"

namespace simple_ftpd {

// Forward declarations
class FTPUser;
class FTPVirtualHost;
class Logger;

/**
 * @brief FTP Connection states
 */
enum class FTPConnectionState {
    CONNECTED,      // Initial connection established
    AUTHENTICATED,  // User authenticated
    AUTHORIZED,     // User authorized for specific operations
    TRANSFER_MODE,  // In file transfer mode
    CLOSING,        // Connection closing
    CLOSED         // Connection closed
};

/**
 * @brief FTP Transfer types
 */
enum class FTPTransferType {
    ASCII,         // ASCII text transfer
    BINARY,        // Binary transfer
    IMAGE          // Image transfer (same as binary)
};

/**
 * @brief FTP Transfer modes
 */
enum class FTPTransferMode {
    STREAM,        // Stream mode
    BLOCK,         // Block mode
    COMPRESSED     // Compressed mode
};

/**
 * @brief FTP Connection class
 * 
 * This class handles individual FTP connections including:
 * - Command parsing and execution
 * - File transfer operations
 * - User authentication
 * - Session management
 * - Data connection handling
 */
class FTPConnection {
public:
    /**
     * @brief Constructor
     * @param client_socket Client socket
     * @param client_addr Client address
     * @param virtual_host Virtual host for this connection
     */
    FTPConnection(socket_t client_socket, 
                  const std::string& client_addr,
                  std::shared_ptr<FTPVirtualHost> virtual_host);
    
    /**
     * @brief Destructor
     */
    ~FTPConnection();
    
    /**
     * @brief Start connection handling
     */
    void start();
    
    /**
     * @brief Stop connection handling
     */
    void stop();
    
    /**
     * @brief Check if connection is active
     * @return true if active, false otherwise
     */
    bool isActive() const { return active_.load(); }
    
    /**
     * @brief Get connection state
     * @return Current connection state
     */
    FTPConnectionState getState() const { return state_; }
    
    /**
     * @brief Get client address
     * @return Client address string
     */
    std::string getClientAddress() const { return client_addr_; }
    
    /**
     * @brief Get authenticated user
     * @return User if authenticated, nullptr otherwise
     */
    std::shared_ptr<FTPUser> getAuthenticatedUser() const { return authenticated_user_; }
    
    /**
     * @brief Get virtual host
     * @return Virtual host for this connection
     */
    std::shared_ptr<FTPVirtualHost> getVirtualHost() const { return virtual_host_; }
    
    /**
     * @brief Get current working directory
     * @return Current working directory
     */
    std::string getCurrentDirectory() const { return current_directory_; }
    
    /**
     * @brief Set current working directory
     * @param directory New working directory
     */
    void setCurrentDirectory(const std::string& directory);
    
    /**
     * @brief Send response to client
     * @param code Response code
     * @param message Response message
     */
    void sendResponse(int code, const std::string& message);
    
    /**
     * @brief Send data to client
     * @param data Data to send
     * @param length Length of data
     * @return Number of bytes sent
     */
    ssize_t sendData(const char* data, size_t length);
    
    /**
     * @brief Receive data from client
     * @param buffer Buffer to receive data
     * @param max_length Maximum length to receive
     * @return Number of bytes received
     */
    ssize_t receiveData(char* buffer, size_t max_length);
    
    /**
     * @brief Get transfer type
     * @return Current transfer type
     */
    FTPTransferType getTransferType() const { return transfer_type_; }
    
    /**
     * @brief Set transfer type
     * @param type New transfer type
     */
    void setTransferType(FTPTransferType type) { transfer_type_ = type; }
    
    /**
     * @brief Get transfer mode
     * @return Current transfer mode
     */
    FTPTransferMode getTransferMode() const { return transfer_mode_; }
    
    /**
     * @brief Set transfer mode
     * @param mode New transfer mode
     */
    void setTransferMode(FTPTransferMode mode) { transfer_mode_ = mode; }
    
    /**
     * @brief Check if passive mode is enabled
     * @return true if passive mode, false otherwise
     */
    bool isPassiveMode() const { return passive_mode_; }
    
    /**
     * @brief Set passive mode
     * @param passive true for passive mode, false for active mode
     */
    void setPassiveMode(bool passive) { passive_mode_ = passive; }
    
    /**
     * @brief Get data connection port
     * @return Data connection port
     */
    uint16_t getDataPort() const { return data_port_; }
    
    /**
     * @brief Set data connection port
     * @param port Data connection port
     */
    void setDataPort(uint16_t port) { data_port_ = port; }
    
    /**
     * @brief Get data connection address
     * @return Data connection address
     */
    std::string getDataAddress() const { return data_addr_; }
    
    /**
     * @brief Set data connection address
     * @param addr Data connection address
     */
    void setDataAddress(const std::string& addr) { data_addr_ = addr; }
    
    /**
     * @brief Get connection start time
     * @return Connection start time as string
     */
    std::string getStartTime() const { return start_time_; }
    
    /**
     * @brief Get last activity time
     * @return Last activity time as string
     */
    std::string getLastActivityTime() const { return last_activity_time_; }
    
    /**
     * @brief Update last activity time
     */
    void updateActivityTime();
    
    /**
     * @brief Get connection statistics
     * @return Statistics string
     */
    std::string getStatistics() const;

private:
    /**
     * @brief Main connection loop
     */
    void connectionLoop();
    
    /**
     * @brief Handle FTP command
     * @param command Command string
     */
    void handleCommand(const std::string& command);
    
    /**
     * @brief Parse command line
     * @param line Command line
     * @return Vector of command and arguments
     */
    std::vector<std::string> parseCommandLine(const std::string& line);
    
    /**
     * @brief Handle USER command
     * @param args Command arguments
     */
    void handleUSER(const std::vector<std::string>& args);
    
    /**
     * @brief Handle PASS command
     * @param args Command arguments
     */
    void handlePASS(const std::vector<std::string>& args);
    
    /**
     * @brief Handle QUIT command
     * @param args Command arguments
     */
    void handleQUIT(const std::vector<std::string>& args);
    
    /**
     * @brief Handle PWD command
     * @param args Command arguments
     */
    void handlePWD(const std::vector<std::string>& args);
    
    /**
     * @brief Handle CWD command
     * @param args Command arguments
     */
    void handleCWD(const std::vector<std::string>& args);
    
    /**
     * @brief Handle CDUP command
     * @param args Command arguments
     */
    void handleCDUP(const std::vector<std::string>& args);
    
    /**
     * @brief Handle LIST command
     * @param args Command arguments
     */
    void handleLIST(const std::vector<std::string>& args);
    
    /**
     * @brief Handle NLST command
     * @param args Command arguments
     */
    void handleNLST(const std::vector<std::string>& args);
    
    /**
     * @brief Handle RETR command
     * @param args Command arguments
     */
    void handleRETR(const std::vector<std::string>& args);
    
    /**
     * @brief Handle STOR command
     * @param args Command arguments
     */
    void handleSTOR(const std::vector<std::string>& args);
    
    /**
     * @brief Handle APPE command
     * @param args Command arguments
     */
    void handleAPPE(const std::vector<std::string>& args);
    
    /**
     * @brief Handle DELE command
     * @param args Command arguments
     */
    void handleDELE(const std::vector<std::string>& args);
    
    /**
     * @brief Handle MKD command
     * @param args Command arguments
     */
    void handleMKD(const std::vector<std::string>& args);
    
    /**
     * @brief Handle RMD command
     * @param args Command arguments
     */
    void handleRMD(const std::vector<std::string>& args);
    
    /**
     * @brief Handle RNFR command
     * @param args Command arguments
     */
    void handleRNFR(const std::vector<std::string>& args);
    
    /**
     * @brief Handle RNTO command
     * @param args Command arguments
     */
    void handleRNTO(const std::vector<std::string>& args);
    
    /**
     * @brief Handle TYPE command
     * @param args Command arguments
     */
    void handleTYPE(const std::vector<std::string>& args);
    
    /**
     * @brief Handle MODE command
     * @param args Command arguments
     */
    void handleMODE(const std::vector<std::string>& args);
    
    /**
     * @brief Handle PASV command
     * @param args Command arguments
     */
    void handlePASV(const std::vector<std::string>& args);
    
    /**
     * @brief Handle PORT command
     * @param args Command arguments
     */
    void handlePORT(const std::vector<std::string>& args);
    
    /**
     * @brief Handle NOOP command
     * @param args Command arguments
     */
    void handleNOOP(const std::vector<std::string>& args);
    
    /**
     * @brief Handle SYST command
     * @param args Command arguments
     */
    void handleSYST(const std::vector<std::string>& args);
    
    /**
     * @brief Handle FEAT command
     * @param args Command arguments
     */
    void handleFEAT(const std::vector<std::string>& args);
    
    /**
     * @brief Handle HELP command
     * @param args Command arguments
     */
    void handleHELP(const std::vector<std::string>& args);
    
    /**
     * @brief Handle SITE command
     * @param args Command arguments
     */
    void handleSITE(const std::vector<std::string>& args);
    
    /**
     * @brief Setup data connection
     * @return true if setup successfully, false otherwise
     */
    bool setupDataConnection();
    
    /**
     * @brief Close data connection
     */
    void closeDataConnection();
    
    /**
     * @brief Send file listing
     * @param path Path to list
     * @param long_format true for long format, false for short
     */
    void sendFileListing(const std::string& path, bool long_format = true);
    
    /**
     * @brief Send file
     * @param filepath Path to file
     */
    void sendFile(const std::string& filepath);
    
    /**
     * @brief Receive file
     * @param filepath Path to file
     */
    void receiveFile(const std::string& filepath);
    
    /**
     * @brief Check file permissions
     * @param filepath Path to file
     * @param operation Operation to check (read, write, delete)
     * @return true if allowed, false otherwise
     */
    bool checkFilePermissions(const std::string& filepath, const std::string& operation);
    
    /**
     * @brief Log command
     * @param command Command string
     */
    void logCommand(const std::string& command);
    
    /**
     * @brief Log error
     * @param error Error message
     */
    void logError(const std::string& error);
    
    // Connection properties
    socket_t client_socket_;
    std::string client_addr_;
    std::shared_ptr<FTPVirtualHost> virtual_host_;
    
    // Connection state
    std::atomic<bool> active_;
    FTPConnectionState state_;
    std::shared_ptr<FTPUser> authenticated_user_;
    
    // Session state
    std::string current_directory_;
    FTPTransferType transfer_type_;
    FTPTransferMode transfer_mode_;
    bool passive_mode_;
    uint16_t data_port_;
    std::string data_addr_;
    
    // Data connection
    socket_t data_socket_;
    std::string data_socket_addr_;
    uint16_t data_socket_port_;
    
    // Timing
    std::string start_time_;
    std::string last_activity_time_;
    
    // Statistics
    std::atomic<uint64_t> bytes_sent_;
    std::atomic<uint64_t> bytes_received_;
    std::atomic<uint64_t> files_sent_;
    std::atomic<uint64_t> files_received_;
    
    // Command handling
    std::string username_buffer_;
    std::string rename_from_path_;
    
    // Connection thread
    std::thread connection_thread_;
    
    // Logger
    std::shared_ptr<Logger> logger_;
    
    // Mutex for thread safety
    mutable std::mutex connection_mutex_;
};

} // namespace simple_ftpd
