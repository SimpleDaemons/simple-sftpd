#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "simple-ftpd/ftp_server_config.hpp"

namespace simple_ftpd {

// Forward declarations
class FTPUser;
class Logger;

/**
 * @brief Virtual host access control
 */
enum class VirtualHostAccess {
    ALLOW_ALL,      // Allow all users
    ALLOW_LISTED,   // Allow only listed users
    DENY_LISTED,    // Deny listed users
    DENY_ALL        // Deny all users
};

/**
 * @brief Virtual host security level
 */
enum class VirtualHostSecurity {
    LOW,            // Low security (minimal restrictions)
    MEDIUM,         // Medium security (balanced restrictions)
    HIGH,           // High security (strict restrictions)
    MAXIMUM         // Maximum security (very strict restrictions)
};

/**
 * @brief FTP Virtual Host class
 * 
 * This class represents a virtual FTP host with:
 * - Document root and access control
 * - SSL/TLS configuration
 * - Security settings
 * - Transfer restrictions
 * - User management
 */
class FTPVirtualHost {
public:
    /**
     * @brief Constructor
     * @param hostname Virtual host hostname
     */
    explicit FTPVirtualHost(const std::string& hostname);
    
    /**
     * @brief Destructor
     */
    ~FTPVirtualHost();
    
    /**
     * @brief Get hostname
     * @return Virtual host hostname
     */
    std::string getHostname() const { return hostname_; }
    
    /**
     * @brief Set hostname
     * @param hostname New hostname
     */
    void setHostname(const std::string& hostname) { hostname_ = hostname; }
    
    /**
     * @brief Get document root
     * @return Document root path
     */
    std::string getDocumentRoot() const { return document_root_; }
    
    /**
     * @brief Set document root
     * @param root New document root path
     */
    void setDocumentRoot(const std::string& root) { document_root_ = root; }
    
    /**
     * @brief Get welcome message
     * @return Welcome message
     */
    std::string getWelcomeMessage() const { return welcome_message_; }
    
    /**
     * @brief Set welcome message
     * @param message New welcome message
     */
    void setWelcomeMessage(const std::string& message) { welcome_message_ = message; }
    
    /**
     * @brief Get banner message
     * @return Banner message
     */
    std::string getBannerMessage() const { return banner_message_; }
    
    /**
     * @brief Set banner message
     * @param message New banner message
     */
    void setBannerMessage(const std::string& message) { banner_message_ = message; }
    
    /**
     * @brief Check if virtual host is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Set enabled flag
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * @brief Get SSL configuration
     * @return SSL configuration
     */
    const SSLConfig& getSSLConfig() const { return ssl_config_; }
    
    /**
     * @brief Set SSL configuration
     * @param config New SSL configuration
     */
    void setSSLConfig(const SSLConfig& config) { ssl_config_ = config; }
    
    /**
     * @brief Check if SSL is enabled
     * @return true if SSL enabled, false otherwise
     */
    bool isSSLEnabled() const { return ssl_config_.enabled; }
    
    /**
     * @brief Get security configuration
     * @return Security configuration
     */
    const SecurityConfig& getSecurityConfig() const { return security_config_; }
    
    /**
     * @brief Set security configuration
     * @param config New security configuration
     */
    void setSecurityConfig(const SecurityConfig& config) { security_config_ = config; }
    
    /**
     * @brief Get transfer configuration
     * @return Transfer configuration
     */
    const TransferConfig& getTransferConfig() const { return transfer_config_; }
    
    /**
     * @brief Set transfer configuration
     * @param config New transfer configuration
     */
    void setTransferConfig(const TransferConfig& config) { transfer_config_ = config; }
    
    /**
     * @brief Get access control mode
     * @return Access control mode
     */
    VirtualHostAccess getAccessControl() const { return access_control_; }
    
    /**
     * @brief Set access control mode
     * @param mode New access control mode
     */
    void setAccessControl(VirtualHostAccess mode) { access_control_ = mode; }
    
    /**
     * @brief Get security level
     * @return Security level
     */
    VirtualHostSecurity getSecurityLevel() const { return security_level_; }
    
    /**
     * @brief Set security level
     * @param level New security level
     */
    void setSecurityLevel(VirtualHostSecurity level) { security_level_ = level; }
    
    /**
     * @brief Add alias
     * @param alias Alias to add
     */
    void addAlias(const std::string& alias);
    
    /**
     * @brief Remove alias
     * @param alias Alias to remove
     */
    void removeAlias(const std::string& alias);
    
    /**
     * @brief Check if hostname matches
     * @param hostname Hostname to check
     * @return true if matches, false otherwise
     */
    bool matchesHostname(const std::string& hostname) const;
    
    /**
     * @brief Get all aliases
     * @return Vector of aliases
     */
    std::vector<std::string> getAliases() const;
    
    /**
     * @brief Add allowed user
     * @param username Username to allow
     */
    void addAllowedUser(const std::string& username);
    
    /**
     * @brief Remove allowed user
     * @param username Username to remove
     */
    void removeAllowedUser(const std::string& username);
    
    /**
     * @brief Check if user is allowed
     * @param username Username to check
     * @return true if allowed, false otherwise
     */
    bool isUserAllowed(const std::string& username) const;
    
    /**
     * @brief Get allowed users
     * @return Set of allowed usernames
     */
    std::set<std::string> getAllowedUsers() const { return allowed_users_; }
    
    /**
     * @brief Add denied user
     * @param username Username to deny
     */
    void addDeniedUser(const std::string& username);
    
    /**
     * @brief Remove denied user
     * @param username Username to remove
     */
    void removeDeniedUser(const std::string& username);
    
    /**
     * @brief Check if user is denied
     * @param username Username to check
     * @return true if denied, false otherwise
     */
    bool isUserDenied(const std::string& username) const;
    
    /**
     * @brief Get denied users
     * @return Set of denied usernames
     */
    std::set<std::string> getDeniedUsers() const { return denied_users_; }
    
    /**
     * @brief Check if user can access virtual host
     * @param user User to check
     * @return true if access allowed, false otherwise
     */
    bool canUserAccess(std::shared_ptr<FTPUser> user) const;
    
    /**
     * @brief Get virtual host statistics
     * @return Statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Reset virtual host statistics
     */
    void resetStatistics();
    
    /**
     * @brief Update access statistics
     * @param username Username that accessed
     * @param operation Operation performed
     */
    void updateAccessStats(const std::string& username, const std::string& operation);
    
    /**
     * @brief Get access statistics
     * @return Map of access statistics
     */
    std::map<std::string, size_t> getAccessStats() const { return access_stats_; }
    
    /**
     * @brief Get virtual host configuration
     * @return Virtual host configuration
     */
    VirtualHostConfig getConfig() const;
    
    /**
     * @brief Set virtual host configuration
     * @param config New configuration
     */
    void setConfig(const VirtualHostConfig& config);
    
    /**
     * @brief Validate virtual host configuration
     * @return true if valid, false otherwise
     */
    bool validate() const;
    
    /**
     * @brief Get validation errors
     * @return Vector of error messages
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief Load configuration from file
     * @param config_file Configuration file path
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& config_file);
    
    /**
     * @brief Save configuration to file
     * @param config_file Configuration file path
     * @return true if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& config_file) const;
    
    /**
     * @brief Get virtual host information
     * @return Information string
     */
    std::string getInfo() const;
    
    /**
     * @brief Check if virtual host is default
     * @return true if default, false otherwise
     */
    bool isDefault() const { return default_; }
    
    /**
     * @brief Set default flag
     * @param default_vhost true for default, false otherwise
     */
    void setDefault(bool default_vhost) { default_ = default_vhost; }
    
    /**
     * @brief Get creation time
     * @return Creation time as string
     */
    std::string getCreationTime() const { return creation_time_; }
    
    /**
     * @brief Get last modification time
     * @return Last modification time as string
     */
    std::string getLastModificationTime() const { return last_modification_time_; }
    
    /**
     * @brief Update modification time
     */
    void updateModificationTime();

private:
    // Basic virtual host information
    std::string hostname_;
    std::string document_root_;
    std::string welcome_message_;
    std::string banner_message_;
    bool enabled_;
    bool default_;
    
    // Configuration sections
    SSLConfig ssl_config_;
    SecurityConfig security_config_;
    TransferConfig transfer_config_;
    
    // Access control
    VirtualHostAccess access_control_;
    VirtualHostSecurity security_level_;
    std::vector<std::string> aliases_;
    std::set<std::string> allowed_users_;
    std::set<std::string> denied_users_;
    
    // Statistics
    std::atomic<uint64_t> total_connections_;
    std::atomic<uint64_t> total_transfers_;
    std::atomic<uint64_t> total_bytes_transferred_;
    std::map<std::string, size_t> access_stats_;
    
    // Timing
    std::string creation_time_;
    std::string last_modification_time_;
    
    // Logger
    std::shared_ptr<Logger> logger_;
    
    // Validation state
    mutable std::vector<std::string> validation_errors_;
    
    /**
     * @brief Initialize default values
     */
    void initializeDefaults();
    
    /**
     * @brief Update security configuration based on security level
     */
    void updateSecurityConfig();
    
    /**
     * @brief Validate document root
     * @return true if valid, false otherwise
     */
    bool validateDocumentRoot() const;
    
    /**
     * @brief Validate SSL configuration
     * @return true if valid, false otherwise
     */
    bool validateSSLConfig() const;
    
    /**
     * @brief Validate security configuration
     * @return true if valid, false otherwise
     */
    bool validateSecurityConfig() const;
    
    /**
     * @brief Validate transfer configuration
     * @return true if valid, false otherwise
     */
    bool validateTransferConfig() const;
    
    /**
     * @brief Check if path is accessible
     * @param path Path to check
     * @return true if accessible, false otherwise
     */
    bool isPathAccessible(const std::string& path) const;
    
    /**
     * @brief Get current timestamp
     * @return Current timestamp as string
     */
    std::string getCurrentTimestamp() const;
};

} // namespace simple_ftpd
