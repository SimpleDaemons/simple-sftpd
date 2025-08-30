# API Reference

This document provides the complete API reference for Simple-Secure FTP Daemon, including all classes, methods, and data structures.

## 📚 Core Classes

### FTPServer

The main server class that orchestrates all FTP operations.

#### Constructor
```cpp
explicit FTPServer(std::shared_ptr<FTPServerConfig> config);
```

#### Public Methods
```cpp
bool start();                    // Start the server
void stop();                     // Stop the server
bool isRunning() const;          // Check if server is running
std::string getStatus() const;   // Get server status
size_t getActiveConnectionCount() const;  // Get connection count
bool reloadConfig();             // Reload configuration
```

#### Virtual Host Management
```cpp
std::shared_ptr<FTPVirtualHost> getVirtualHost(const std::string& hostname) const;
std::shared_ptr<FTPVirtualHost> getDefaultVirtualHost() const;
void addVirtualHost(std::shared_ptr<FTPVirtualHost> virtual_host);
void removeVirtualHost(const std::string& hostname);
```

#### User Management
```cpp
std::shared_ptr<FTPUser> getUser(const std::string& username) const;
void addUser(std::shared_ptr<FTPUser> user);
void removeUser(const std::string& username);
```

### FTPConnection

Handles individual FTP connections and command processing.

#### Constructor
```cpp
FTPConnection(socket_t client_socket, 
              const std::string& client_addr,
              std::shared_ptr<FTPVirtualHost> virtual_host);
```

#### Public Methods
```cpp
void start();                    // Start connection handling
void stop();                     // Stop connection handling
bool isActive() const;           // Check if connection is active
FTPConnectionState getState() const;  // Get connection state
std::string getClientAddress() const; // Get client address
```

#### File Operations
```cpp
void sendFile(const std::string& filepath);
void receiveFile(const std::string& filepath);
void sendFileListing(const std::string& path, bool long_format = true);
bool checkFilePermissions(const std::string& filepath, const std::string& operation);
```

### FTPUser

Represents an FTP user with authentication and permissions.

#### Constructor
```cpp
explicit FTPUser(const std::string& username);
```

#### Authentication
```cpp
bool verifyPassword(const std::string& password) const;
void setPassword(const std::string& password);
std::string getPasswordHash() const;
```

#### Permissions
```cpp
bool hasPermission(UserPermission permission) const;
void grantPermission(UserPermission permission);
void revokePermission(UserPermission permission);
std::set<UserPermission> getPermissions() const;
```

#### User Properties
```cpp
std::string getHomeDirectory() const;
void setHomeDirectory(const std::string& directory);
bool isEnabled() const;
void setStatus(UserStatus status);
```

### FTPVirtualHost

Manages virtual host configuration and isolation.

#### Constructor
```cpp
explicit FTPVirtualHost(const std::string& hostname);
```

#### Configuration
```cpp
void setDocumentRoot(const std::string& root);
std::string getDocumentRoot() const;
void setSSLConfig(const SSLConfig& config);
const SSLConfig& getSSLConfig() const;
```

#### Access Control
```cpp
bool canUserAccess(std::shared_ptr<FTPUser> user) const;
void addAllowedUser(const std::string& username);
void removeAllowedUser(const std::string& username);
bool isUserAllowed(const std::string& username) const;
```

### FTPServerConfig

Manages server configuration and settings.

#### Constructor
```cpp
FTPServerConfig();
```

#### Configuration Loading
```cpp
bool loadFromFile(const std::string& config_file);
bool loadFromJSON(const std::string& json_config);
bool saveToFile(const std::string& config_file) const;
std::string saveToJSON() const;
```

#### Validation
```cpp
bool validate() const;
std::vector<std::string> getErrors() const;
void reset();                    // Reset to defaults
```

### Logger

Provides comprehensive logging functionality.

#### Constructor
```cpp
Logger(const std::string& log_file = "",
       LogLevel log_level = LogLevel::INFO,
       bool log_to_console = true,
       bool log_to_file = true);
```

#### Logging Methods
```cpp
void trace(const std::string& message, const std::string& file = "", int line = 0);
void debug(const std::string& message, const std::string& file = "", int line = 0);
void info(const std::string& message, const std::string& file = "", int line = 0);
void warn(const std::string& message, const std::string& file = "", int line = 0);
void error(const std::string& message, const std::string& file = "", int line = 0);
void fatal(const std::string& message, const std::string& file = "", int line = 0);
```

#### Configuration
```cpp
void setLogLevel(LogLevel level);
void setLogFormat(LogFormat format);
void setLogFile(const std::string& log_file);
void setMaxLogSize(size_t max_size);
```

## 🔧 Data Structures

### SSLConfig
```cpp
struct SSLConfig {
    bool enabled = false;
    std::string certificate_file;
    std::string private_key_file;
    std::string ca_certificate_file;
    std::string cipher_suite;
    bool require_client_cert = false;
    bool verify_peer = false;
    int min_tls_version = 0x0301;
    int max_tls_version = 0x0304;
};
```

### LoggingConfig
```cpp
struct LoggingConfig {
    std::string log_file;
    std::string log_level = "INFO";
    bool log_to_console = true;
    bool log_to_file = true;
    bool log_commands = true;
    bool log_transfers = true;
    bool log_errors = true;
    std::string log_format = "default";
    size_t max_log_size = 10 * 1024 * 1024;
    size_t max_log_files = 5;
};
```

### SecurityConfig
```cpp
struct SecurityConfig {
    bool chroot_enabled = false;
    std::string chroot_directory;
    bool drop_privileges = true;
    std::string run_as_user;
    std::string run_as_group;
    bool allow_anonymous = false;
    bool allow_guest = false;
    bool require_ssl = false;
    std::vector<std::string> allowed_commands;
    std::vector<std::string> denied_commands;
    size_t max_login_attempts = 3;
    std::chrono::seconds login_timeout{30};
    std::chrono::seconds session_timeout{3600};
};
```

### ConnectionConfig
```cpp
struct ConnectionConfig {
    std::string bind_address = "0.0.0.0";
    uint16_t bind_port = 21;
    size_t max_connections = 100;
    size_t max_connections_per_ip = 10;
    std::chrono::seconds connection_timeout{300};
    std::chrono::seconds data_timeout{300};
    std::chrono::seconds idle_timeout{600};
    bool keep_alive = true;
    size_t keep_alive_interval = 60;
    size_t keep_alive_probes = 3;
    bool tcp_nodelay = true;
    bool reuse_address = true;
    size_t backlog = 50;
};
```

## 📊 Enumerations

### LogLevel
```cpp
enum class LogLevel {
    TRACE = 0,      // Most verbose
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5       // Least verbose
};
```

### LogFormat
```cpp
enum class LogFormat {
    SIMPLE,         // [LEVEL] message
    STANDARD,       // [TIMESTAMP] [LEVEL] [THREAD] message
    EXTENDED,       // [TIMESTAMP] [LEVEL] [THREAD] [FILE:LINE] message
    JSON,           // {"timestamp": "...", "level": "...", "message": "..."}
    CUSTOM          // Custom format
};
```

### UserPermission
```cpp
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
```

### UserStatus
```cpp
enum class UserStatus {
    ACTIVE,         // Account is active
    INACTIVE,       // Account is inactive
    LOCKED,         // Account is locked
    EXPIRED,        // Account has expired
    SUSPENDED       // Account is suspended
};
```

### FTPConnectionState
```cpp
enum class FTPConnectionState {
    CONNECTED,      // Initial connection established
    AUTHENTICATED,  // User authenticated
    AUTHORIZED,     // User authorized for specific operations
    TRANSFER_MODE,  // In file transfer mode
    CLOSING,        // Connection closing
    CLOSED          // Connection closed
};
```

## 🔌 Platform Abstraction

### Platform Functions
```cpp
// Socket operations
socket_t createSocket(int domain, int type, int protocol);
bool bindSocket(socket_t socket, const struct sockaddr* addr, socklen_t addrlen);
bool listenSocket(socket_t socket, int backlog);
socket_t acceptSocket(socket_t socket, struct sockaddr* addr, socklen_t* addrlen);
ssize_t sendData(socket_t socket, const char* data, size_t length);
ssize_t receiveData(socket_t socket, char* buffer, size_t max_length);
void closeSocket(socket_t socket);

// File system operations
bool createDirectory(const std::string& path, mode_t mode = 0755);
bool directoryExists(const std::string& path);
bool fileExists(const std::string& path);
bool setFilePermissions(const std::string& path, mode_t mode);
bool setFileOwner(const std::string& path, uid_t uid, gid_t gid);

// Process management
bool changeToUser(const std::string& username);
bool changeToGroup(const std::string& groupname);
bool dropPrivileges();
bool isRoot();
std::string getCurrentUser();
std::string getCurrentGroup();

// Path utilities
std::string getPathSeparator();
std::string normalizePath(const std::string& path);
std::string joinPath(const std::string& path1, const std::string& path2);
std::vector<std::string> splitPath(const std::string& path);
std::string getDirName(const std::string& path);
std::string getBaseName(const std::string& path);
bool isAbsolutePath(const std::string& path);
std::string resolvePath(const std::string& path);
```

## 🚨 Error Handling

### Error Codes
```cpp
// Socket error codes
#define SOCKET_ERROR_CODE errno
#define SOCKET_ERROR_MSG strerror(errno)

// Common error scenarios
class FTPException : public std::runtime_error {
public:
    explicit FTPException(const std::string& message);
    explicit FTPException(const std::string& message, int error_code);
    int getErrorCode() const;
private:
    int error_code_;
};
```

### Exception Handling
```cpp
try {
    server.start();
} catch (const FTPException& e) {
    logger->error("Failed to start server: " + std::string(e.what()));
    return false;
} catch (const std::exception& e) {
    logger->error("Unexpected error: " + std::string(e.what()));
    return false;
}
```

## 📝 Usage Examples

### Basic Server Setup
```cpp
#include "ssftpd/ftp_server.hpp"
#include "ssftpd/ftp_server_config.hpp"
#include "ssftpd/logger.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<simple_ftpd::FTPServerConfig>();
    config->loadFromFile("/etc/ssftpd/ssftpd.conf");
    
    // Create logger
    auto logger = std::make_shared<simple_ftpd::Logger>(
        "/var/log/ssftpd/ssftpd.log",
        simple_ftpd::LogLevel::INFO
    );
    
    // Create and start server
    simple_ftpd::FTPServer server(config);
    if (server.start()) {
        logger->info("Server started successfully");
        // Keep server running
        while (server.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    return 0;
}
```

### User Management
```cpp
#include "ssftpd/ftp_user.hpp"

// Create user
auto user = std::make_shared<simple_ftpd::FTPUser>("john");
user->setPassword("secret123");
user->setHomeDirectory("/var/ftp/john");
user->grantPermission(simple_ftpd::UserPermission::READ);
user->grantPermission(simple_ftpd::UserPermission::WRITE);
user->grantPermission(simple_ftpd::UserPermission::LIST);
user->grantPermission(simple_ftpd::UserPermission::UPLOAD);
user->grantPermission(simple_ftpd::UserPermission::DOWNLOAD);

// Verify user
if (user->verifyPassword("secret123")) {
    if (user->hasPermission(simple_ftpd::UserPermission::UPLOAD)) {
        // Allow upload
    }
}
```

### Virtual Host Configuration
```cpp
#include "ssftpd/ftp_virtual_host.hpp"

// Create virtual host
auto vhost = std::make_shared<simple_ftpd::FTPVirtualHost>("ftp.example.com");
vhost->setDocumentRoot("/var/ftp/example");
vhost->setWelcomeMessage("Welcome to Example.com FTP Server");
vhost->setBannerMessage("Example.com FTP Server Ready");

// Configure SSL
simple_ftpd::SSLConfig ssl_config;
ssl_config.enabled = true;
ssl_config.certificate_file = "/etc/ssl/certs/example.com.crt";
ssl_config.private_key_file = "/etc/ssl/private/example.com.key";
vhost->setSSLConfig(ssl_config);

// Configure security
simple_ftpd::SecurityConfig security_config;
security_config.chroot_enabled = true;
security_config.chroot_directory = "/var/ftp/example";
security_config.drop_privileges = true;
security_config.run_as_user = "ftp";
vhost->setSecurityConfig(security_config);
```

## 🔍 Debugging and Development

### Debug Logging
```cpp
// Enable debug logging
logger->setLogLevel(simple_ftpd::LogLevel::DEBUG);
logger->setLogFormat(simple_ftpd::LogFormat::EXTENDED);

// Add debug statements
logger->debug("Function: " + std::string(__FUNCTION__));
logger->debug("File: " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
logger->debug("Variable: " + std::to_string(value));
```

### Performance Monitoring
```cpp
// Enable performance monitoring
logger->setPerformanceMonitoring(true);

// Get performance metrics
std::string metrics = logger->getPerformanceMetrics();
logger->info("Performance metrics: " + metrics);
```

## 📚 Additional Resources

- **Getting Started**: [Getting Started Guide](../getting-started/README.md)
- **Configuration**: [Configuration Guide](../configuration/README.md)
- **User Guide**: [User Guide](../user-guide/README.md)
- **Development**: [Development Guide](../development/README.md)

For more detailed examples and use cases, see the [Examples](../examples/README.md) section.
