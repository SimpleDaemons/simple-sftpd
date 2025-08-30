#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <set>
#include <atomic> // Added for atomic types

namespace simple_ftpd {

/**
 * @brief User permission types
 */
enum class UserPermission {
    READ,           // Read files and directories
    WRITE,          // Write/create files and directories
    DELETE,         // Delete files and directories
    RENAME,         // Rename files and directories
    MKDIR,          // Create directories
    RMDIR,          // Remove directories
    LIST,           // List directory contents
    UPLOAD,         // Upload files
    DOWNLOAD,       // Download files
    APPEND,         // Append to files
    CHMOD,          // Change file permissions
    CHOWN,          // Change file ownership
    SYMLINK,        // Create symbolic links
    EXECUTE,        // Execute files
    ADMIN           // Administrative operations
};

/**
 * @brief User authentication methods
 */
enum class AuthMethod {
    PASSWORD,       // Plain text password
    HASH,           // Hashed password
    PAM,            // PAM authentication
    LDAP,           // LDAP authentication
    RADIUS,         // RADIUS authentication
    OAUTH2,         // OAuth2 authentication
    CERTIFICATE,    // SSL certificate authentication
    KEY             // SSH key authentication
};

/**
 * @brief User account status
 */
enum class UserStatus {
    ACTIVE,         // Account is active
    INACTIVE,       // Account is inactive
    LOCKED,         // Account is locked
    EXPIRED,        // Account has expired
    SUSPENDED       // Account is suspended
};

/**
 * @brief FTP User class
 * 
 * This class represents an FTP user with:
 * - Authentication credentials
 * - Access permissions
 * - Directory restrictions
 * - Rate limiting
 * - Session management
 */
class FTPUser {
public:
    /**
     * @brief Constructor
     * @param username Username
     */
    explicit FTPUser(const std::string& username);
    
    /**
     * @brief Destructor
     */
    ~FTPUser();
    
    /**
     * @brief Get username
     * @return Username
     */
    std::string getUsername() const { return username_; }
    
    /**
     * @brief Set username
     * @param username New username
     */
    void setUsername(const std::string& username) { username_ = username; }
    
    /**
     * @brief Get password hash
     * @return Password hash
     */
    std::string getPasswordHash() const { return password_hash_; }
    
    /**
     * @brief Set password hash
     * @param hash New password hash
     */
    void setPasswordHash(const std::string& hash) { password_hash_ = hash; }
    
    /**
     * @brief Set plain text password (will be hashed)
     * @param password Plain text password
     */
    void setPassword(const std::string& password);
    
    /**
     * @brief Verify password
     * @param password Password to verify
     * @return true if password matches, false otherwise
     */
    bool verifyPassword(const std::string& password) const;
    
    /**
     * @brief Get home directory
     * @return Home directory path
     */
    std::string getHomeDirectory() const { return home_directory_; }
    
    /**
     * @brief Set home directory
     * @param directory New home directory
     */
    void setHomeDirectory(const std::string& directory) { home_directory_ = directory; }
    
    /**
     * @brief Get shell
     * @return Shell path
     */
    std::string getShell() const { return shell_; }
    
    /**
     * @brief Set shell
     * @param shell New shell path
     */
    void setShell(const std::string& shell) { shell_ = shell; }
    
    /**
     * @brief Get group
     * @return Group name
     */
    std::string getGroup() const { return group_; }
    
    /**
     * @brief Set group
     * @param group New group name
     */
    void setGroup(const std::string& group) { group_ = group; }
    
    /**
     * @brief Check if user is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return status_ == UserStatus::ACTIVE; }
    
    /**
     * @brief Set user status
     * @param status New status
     */
    void setStatus(UserStatus status) { status_ = status; }
    
    /**
     * @brief Get user status
     * @return Current status
     */
    UserStatus getStatus() const { return status_; }
    
    /**
     * @brief Check if user is anonymous
     * @return true if anonymous, false otherwise
     */
    bool isAnonymous() const { return anonymous_; }
    
    /**
     * @brief Set anonymous flag
     * @param anonymous true for anonymous, false otherwise
     */
    void setAnonymous(bool anonymous) { anonymous_ = anonymous; }
    
    /**
     * @brief Check if user is guest
     * @return true if guest, false otherwise
     */
    bool isGuest() const { return guest_; }
    
    /**
     * @brief Set guest flag
     * @param guest true for guest, false otherwise
     */
    void setGuest(bool guest) { guest_ = guest; }
    
    /**
     * @brief Check if user has permission
     * @param permission Permission to check
     * @return true if has permission, false otherwise
     */
    bool hasPermission(UserPermission permission) const;
    
    /**
     * @brief Grant permission
     * @param permission Permission to grant
     */
    void grantPermission(UserPermission permission);
    
    /**
     * @brief Revoke permission
     * @param permission Permission to revoke
     */
    void revokePermission(UserPermission permission);
    
    /**
     * @brief Get all permissions
     * @return Set of permissions
     */
    std::set<UserPermission> getPermissions() const { return permissions_; }
    
    /**
     * @brief Check if operation is denied
     * @param operation Operation to check
     * @return true if denied, false otherwise
     */
    bool isOperationDenied(const std::string& operation) const;
    
    /**
     * @brief Deny operation
     * @param operation Operation to deny
     */
    void denyOperation(const std::string& operation);
    
    /**
     * @brief Allow operation
     * @param operation Operation to allow
     */
    void allowOperation(const std::string& operation);
    
    /**
     * @brief Get denied operations
     * @return Set of denied operations
     */
    std::set<std::string> getDeniedOperations() const { return denied_operations_; }
    
    /**
     * @brief Get maximum connections
     * @return Maximum number of connections
     */
    size_t getMaxConnections() const { return max_connections_; }
    
    /**
     * @brief Set maximum connections
     * @param max_connections New maximum
     */
    void setMaxConnections(size_t max_connections) { max_connections_ = max_connections; }
    
    /**
     * @brief Get current connection count
     * @return Current number of connections
     */
    size_t getCurrentConnections() const { return current_connections_; }
    
    /**
     * @brief Increment connection count
     * @return true if successful, false if limit exceeded
     */
    bool incrementConnections();
    
    /**
     * @brief Decrement connection count
     */
    void decrementConnections();
    
    /**
     * @brief Get maximum transfer rate
     * @return Maximum transfer rate in bytes per second
     */
    size_t getMaxTransferRate() const { return max_transfer_rate_; }
    
    /**
     * @brief Set maximum transfer rate
     * @param rate New rate in bytes per second
     */
    void setMaxTransferRate(size_t rate) { max_transfer_rate_ = rate; }
    
    /**
     * @brief Get maximum file size
     * @return Maximum file size in bytes
     */
    size_t getMaxFileSize() const { return max_file_size_; }
    
    /**
     * @brief Set maximum file size
     * @param size New size in bytes
     */
    void setMaxFileSize(size_t size) { max_file_size_ = size; }
    
    /**
     * @brief Get session timeout
     * @return Session timeout duration
     */
    std::chrono::seconds getSessionTimeout() const { return session_timeout_; }
    
    /**
     * @brief Set session timeout
     * @param timeout New timeout duration
     */
    void setSessionTimeout(std::chrono::seconds timeout) { session_timeout_ = timeout; }
    
    /**
     * @brief Get authentication method
     * @return Authentication method
     */
    AuthMethod getAuthMethod() const { return auth_method_; }
    
    /**
     * @brief Set authentication method
     * @param method New authentication method
     */
    void setAuthMethod(AuthMethod method) { auth_method_ = method; }
    
    /**
     * @brief Get custom setting
     * @param key Setting key
     * @return Setting value or empty string if not found
     */
    std::string getCustomSetting(const std::string& key) const;
    
    /**
     * @brief Set custom setting
     * @param key Setting key
     * @param value Setting value
     */
    void setCustomSetting(const std::string& key, const std::string& value);
    
    /**
     * @brief Get all custom settings
     * @return Map of custom settings
     */
    std::map<std::string, std::string> getCustomSettings() const { return custom_settings_; }
    
    /**
     * @brief Check if user can access path
     * @param path Path to check
     * @param operation Operation to perform
     * @return true if access allowed, false otherwise
     */
    bool canAccessPath(const std::string& path, const std::string& operation) const;
    
    /**
     * @brief Add allowed path
     * @param path Path to allow
     */
    void addAllowedPath(const std::string& path);
    
    /**
     * @brief Remove allowed path
     * @param path Path to remove
     */
    void removeAllowedPath(const std::string& path);
    
    /**
     * @brief Get allowed paths
     * @return Set of allowed paths
     */
    std::set<std::string> getAllowedPaths() const { return allowed_paths_; }
    
    /**
     * @brief Add denied path
     * @param path Path to deny
     */
    void addDeniedPath(const std::string& path);
    
    /**
     * @brief Remove denied path
     * @param path Path to remove
     */
    void removeDeniedPath(const std::string& path);
    
    /**
     * @brief Get denied paths
     * @return Set of denied paths
     */
    std::set<std::string> getDeniedPaths() const { return denied_paths_; }
    
    /**
     * @brief Get user statistics
     * @return Statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Reset user statistics
     */
    void resetStatistics();
    
    /**
     * @brief Update last login time
     */
    void updateLastLogin();
    
    /**
     * @brief Get last login time
     * @return Last login time as string
     */
    std::string getLastLoginTime() const { return last_login_time_; }
    
    /**
     * @brief Check if account is expired
     * @return true if expired, false otherwise
     */
    bool isExpired() const;
    
    /**
     * @brief Set expiration date
     * @param expiration_date Expiration date
     */
    void setExpirationDate(const std::string& expiration_date);
    
    /**
     * @brief Get expiration date
     * @return Expiration date as string
     */
    std::string getExpirationDate() const { return expiration_date_; }

private:
    // Basic user information
    std::string username_;
    std::string password_hash_;
    std::string home_directory_;
    std::string shell_;
    std::string group_;
    
    // Account status
    UserStatus status_;
    bool anonymous_;
    bool guest_;
    
    // Permissions and restrictions
    std::set<UserPermission> permissions_;
    std::set<std::string> denied_operations_;
    std::set<std::string> allowed_paths_;
    std::set<std::string> denied_paths_;
    
    // Connection limits
    size_t max_connections_;
    size_t current_connections_;
    
    // Transfer limits
    size_t max_transfer_rate_;
    size_t max_file_size_;
    
    // Timing
    std::chrono::seconds session_timeout_;
    std::string last_login_time_;
    std::string expiration_date_;
    
    // Authentication
    AuthMethod auth_method_;
    
    // Custom settings
    std::map<std::string, std::string> custom_settings_;
    
    // Statistics
    std::atomic<uint64_t> total_uploads_;
    std::atomic<uint64_t> total_downloads_;
    std::atomic<uint64_t> total_bytes_uploaded_;
    std::atomic<uint64_t> total_bytes_downloaded_;
    std::atomic<uint64_t> total_connections_;
    std::atomic<uint64_t> failed_logins_;
    
    /**
     * @brief Hash password
     * @param password Plain text password
     * @return Hashed password
     */
    std::string hashPassword(const std::string& password) const;
    
    /**
     * @brief Check if path is in allowed paths
     * @param path Path to check
     * @return true if allowed, false otherwise
     */
    bool isPathAllowed(const std::string& path) const;
    
    /**
     * @brief Check if path is in denied paths
     * @param path Path to check
     * @return true if denied, false otherwise
     */
    bool isPathDenied(const std::string& path) const;
};

} // namespace simple_ftpd
