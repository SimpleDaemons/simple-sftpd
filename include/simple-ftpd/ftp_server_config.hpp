#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace simple_ftpd {

/**
 * @brief SSL/TLS configuration
 */
struct SSLConfig {
    bool enabled = false;
    std::string certificate_file;
    std::string private_key_file;
    std::string ca_certificate_file;
    std::string cipher_suite;
    bool require_client_cert = false;
    bool verify_peer = false;
    int min_tls_version = 0x0301; // TLS 1.0
    int max_tls_version = 0x0304; // TLS 1.3
};

/**
 * @brief Logging configuration
 */
struct LoggingConfig {
    std::string log_file;
    std::string log_level = "INFO";
    bool log_to_console = true;
    bool log_to_file = true;
    bool log_commands = true;
    bool log_transfers = true;
    bool log_errors = true;
    std::string log_format = "default";
    size_t max_log_size = 10 * 1024 * 1024; // 10MB
    size_t max_log_files = 5;
};

/**
 * @brief Security configuration
 */
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

/**
 * @brief Transfer configuration
 */
struct TransferConfig {
    size_t max_file_size = 0; // 0 = unlimited
    size_t max_transfer_rate = 0; // 0 = unlimited
    bool allow_overwrite = true;
    bool allow_resume = true;
    std::string temp_directory;
    size_t buffer_size = 8192;
    bool use_sendfile = true;
    bool use_mmap = false;
    std::vector<std::string> allowed_extensions;
    std::vector<std::string> denied_extensions;
};

/**
 * @brief Connection configuration
 */
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

/**
 * @brief Passive mode configuration
 */
struct PassiveConfig {
    bool enabled = true;
    std::string external_ip;
    std::string local_ip;
    uint16_t min_port = 1024;
    uint16_t max_port = 65535;
    bool use_external_ip = false;
    bool use_local_ip = false;
    std::vector<std::string> allowed_ports;
    std::vector<std::string> denied_ports;
};

/**
 * @brief Rate limiting configuration
 */
struct RateLimitConfig {
    bool enabled = false;
    size_t max_requests_per_minute = 1000;
    size_t max_connections_per_minute = 100;
    size_t max_transfers_per_minute = 50;
    size_t max_bytes_per_minute = 100 * 1024 * 1024; // 100MB
    std::chrono::seconds window_size{60};
    bool per_ip_limiting = true;
    bool per_user_limiting = true;
};

/**
 * @brief Virtual host configuration
 */
struct VirtualHostConfig {
    std::string hostname;
    std::string document_root;
    std::string welcome_message;
    std::string banner_message;
    bool enabled = true;
    SSLConfig ssl;
    SecurityConfig security;
    TransferConfig transfer;
    std::map<std::string, std::string> aliases;
    std::vector<std::string> allowed_users;
    std::vector<std::string> denied_users;
};

/**
 * @brief User configuration
 */
struct UserConfig {
    std::string username;
    std::string password_hash;
    std::string home_directory;
    std::string shell = "/bin/false";
    std::string group;
    bool enabled = true;
    bool anonymous = false;
    bool guest = false;
    std::vector<std::string> permissions;
    std::vector<std::string> denied_operations;
    size_t max_connections = 1;
    size_t max_transfer_rate = 0; // 0 = unlimited
    size_t max_file_size = 0; // 0 = unlimited
    std::chrono::seconds session_timeout{3600};
    std::map<std::string, std::string> custom_settings;
};

/**
 * @brief Main FTP server configuration
 */
class FTPServerConfig {
public:
    /**
     * @brief Constructor
     */
    FTPServerConfig();
    
    /**
     * @brief Destructor
     */
    ~FTPServerConfig();
    
    /**
     * @brief Load configuration from file
     * @param config_file Configuration file path
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& config_file);
    
    /**
     * @brief Load configuration from JSON string
     * @param json_config JSON configuration string
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromJSON(const std::string& json_config);
    
    /**
     * @brief Save configuration to file
     * @param config_file Configuration file path
     * @return true if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& config_file) const;
    
    /**
     * @brief Save configuration to JSON string
     * @return JSON configuration string
     */
    std::string saveToJSON() const;
    
    /**
     * @brief Validate configuration
     * @return true if valid, false otherwise
     */
    bool validate() const;
    
    /**
     * @brief Get configuration errors
     * @return Vector of error messages
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief Merge configuration from another config
     * @param other Configuration to merge
     */
    void merge(const FTPServerConfig& other);
    
    /**
     * @brief Reset to default values
     */
    void reset();
    
    // Configuration sections
    SSLConfig ssl;
    LoggingConfig logging;
    SecurityConfig security;
    TransferConfig transfer;
    ConnectionConfig connection;
    PassiveConfig passive;
    RateLimitConfig rate_limit;
    
    // Virtual hosts and users
    std::vector<VirtualHostConfig> virtual_hosts;
    std::vector<UserConfig> users;
    
    // Global settings
    std::string server_name = "Simple FTP Daemon";
    std::string server_version = "0.1.0";
    std::string server_banner = "Welcome to Simple FTP Daemon";
    std::string config_file;
    std::string pid_file;
    bool daemon_mode = false;
    bool foreground_mode = false;
    std::string working_directory;
    std::string user_config_dir;
    std::string system_config_dir;
    
    // Feature flags
    bool enable_ssl = false;
    bool enable_virtual_hosts = false;
    bool enable_user_management = true;
    bool enable_rate_limiting = false;
    bool enable_logging = true;
    bool enable_statistics = true;
    bool enable_monitoring = false;
    
    // Performance settings
    size_t thread_pool_size = 4;
    size_t max_memory_usage = 100 * 1024 * 1024; // 100MB
    bool enable_compression = false;
    bool enable_caching = true;
    size_t cache_size = 10 * 1024 * 1024; // 10MB
    
    // Monitoring settings
    bool enable_metrics = false;
    std::string metrics_endpoint = "/metrics";
    uint16_t metrics_port = 8080;
    std::chrono::seconds metrics_interval{60};
    
    // Backup and recovery
    bool enable_backup = false;
    std::string backup_directory;
    std::chrono::seconds backup_interval{86400}; // 24 hours
    size_t max_backups = 7;
    
    // Development and debugging
    bool debug_mode = false;
    bool verbose_logging = false;
    bool trace_commands = false;
    bool profile_performance = false;
    std::string log_socket_events = "none";

private:
    /**
     * @brief Parse configuration file
     * @param config_file Configuration file path
     * @return true if parsed successfully, false otherwise
     */
    bool parseConfigFile(const std::string& config_file);
    
    /**
     * @brief Parse JSON configuration
     * @param json_config JSON configuration string
     * @return true if parsed successfully, false otherwise
     */
    bool parseJSONConfig(const std::string& json_config);
    
    /**
     * @brief Parse INI configuration
     * @param config_file Configuration file path
     * @return true if parsed successfully, false otherwise
     */
    bool parseINIConfig(const std::string& config_file);
    
    /**
     * @brief Set default values
     */
    void setDefaults();
    
    /**
     * @brief Validate SSL configuration
     * @return true if valid, false otherwise
     */
    bool validateSSL() const;
    
    /**
     * @brief Validate security configuration
     * @return true if valid, false otherwise
     */
    bool validateSecurity() const;
    
    /**
     * @brief Validate connection configuration
     * @return true if valid, false otherwise
     */
    bool validateConnection() const;
    
    /**
     * @brief Validate virtual host configuration
     * @return true if valid, false otherwise
     */
    bool validateVirtualHosts() const;
    
    /**
     * @brief Validate user configuration
     * @return true if valid, false otherwise
     */
    bool validateUsers() const;
    
    // Configuration state
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    bool loaded_ = false;
    std::string last_modified_;
    std::string config_format_; // "json", "ini", "auto"
};

} // namespace simple_ftpd
