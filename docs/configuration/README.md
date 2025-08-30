# Configuration Guide

This guide covers all configuration options available in Simple-Secure FTP Daemon, from basic settings to advanced features.

## 📁 Configuration Files

Simple-Secure FTP Daemon supports multiple configuration formats and locations:

### File Locations

| Platform | Default Location | Alternative Locations |
|----------|------------------|----------------------|
| Linux | `/etc/ssftpd/ssftpd.conf` | `/usr/local/etc/ssftpd/` |
| macOS | `/usr/local/etc/ssftpd/ssftpd.conf` | `~/Library/Application Support/ssftpd/` |
| Windows | `C:\Program Files\ssftpd\config\` | `%APPDATA%\ssftpd\` |

### Configuration Formats

1. **INI Format** (Default): Human-readable, section-based configuration
2. **JSON Format**: Machine-readable, structured configuration
3. **Auto-detection**: Automatically detects format based on file extension

## ⚙️ Global Configuration

### Basic Server Settings

```ini
# Server identification
server_name = "Simple-Secure FTP Daemon"
server_version = "0.1.0"
server_banner = "Welcome to Simple-Secure FTP Daemon"

# Operation mode
daemon_mode = false          # Run as daemon (true) or foreground (false)
foreground_mode = true       # Force foreground mode
working_directory = "/var/ftp"  # Base directory for FTP operations

# Configuration directories
user_config_dir = "/etc/ssftpd/users"
system_config_dir = "/etc/ssftpd"
```

### Feature Flags

```ini
# Core features
enable_ssl = true            # Enable SSL/TLS support
enable_virtual_hosts = true  # Enable virtual hosting
enable_user_management = true # Enable user management
enable_rate_limiting = true  # Enable rate limiting
enable_logging = true        # Enable logging system
enable_statistics = true     # Enable statistics collection
enable_monitoring = false    # Enable monitoring endpoints
```

### Performance Settings

```ini
# Threading and performance
thread_pool_size = 8         # Number of worker threads
max_memory_usage = 256MB     # Maximum memory usage
enable_compression = false   # Enable file compression
enable_caching = true        # Enable file caching
cache_size = 50MB            # Cache size limit
```

## 🔐 SSL/TLS Configuration

### Basic SSL Settings

```ini
[ssl]
enabled = true               # Enable SSL/TLS
certificate_file = "/etc/ssftpd/ssl/server.crt"
private_key_file = "/etc/ssftpd/ssl/server.key"
ca_certificate_file = "/etc/ssftpd/ssl/ca.crt"
```

### Advanced SSL Options

```ini
[ssl]
# Cipher configuration
cipher_suite = "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256"

# Client certificate requirements
require_client_cert = false  # Require client certificates
verify_peer = false          # Verify peer certificates

# TLS version control
min_tls_version = 1.2        # Minimum TLS version (1.0, 1.1, 1.2, 1.3)
max_tls_version = 1.3        # Maximum TLS version
```

### SSL Certificate Management

```bash
# Generate self-signed certificate
sudo ssftpd ssl generate \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city "San Francisco" \
  --organization "Example Corp" \
  --email admin@example.com

# Install existing certificates
sudo ssftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key \
  --ca-certificate /path/to/ca.crt
```

## 📝 Logging Configuration

### Basic Logging

```ini
[logging]
log_file = "/var/log/ssftpd/ssftpd.log"
log_level = "INFO"           # TRACE, DEBUG, INFO, WARN, ERROR, FATAL
log_to_console = true        # Output to console
log_to_file = true           # Output to file
```

### Advanced Logging

```ini
[logging]
# Log content control
log_commands = true          # Log FTP commands
log_transfers = true         # Log file transfers
log_errors = true            # Log error messages

# Format and rotation
log_format = "STANDARD"      # SIMPLE, STANDARD, EXTENDED, JSON, CUSTOM
custom_format = "[{timestamp}] [{level}] {message}"  # Custom format string

# Log rotation
max_log_size = 10MB          # Maximum log file size
max_log_files = 5            # Number of rotated log files
```

### Log Format Examples

```ini
# Standard format
log_format = "STANDARD"
# Output: [2024-01-15 10:30:45] [INFO] [Thread-1] Server started

# Extended format
log_format = "EXTENDED"
# Output: [2024-01-15 10:30:45] [INFO] [Thread-1] [main.cpp:123] Server started

# JSON format
log_format = "JSON"
# Output: {"timestamp":"2024-01-15T10:30:45Z","level":"INFO","thread":"Thread-1","message":"Server started"}

# Custom format
log_format = "CUSTOM"
custom_format = "[{level}] {message} ({timestamp})"
# Output: [INFO] Server started (2024-01-15 10:30:45)
```

## 🔒 Security Configuration

### Basic Security

```ini
[security]
chroot_enabled = false       # Enable chroot jail
chroot_directory = "/var/ftp" # Chroot base directory
drop_privileges = true       # Drop root privileges after startup
run_as_user = "ftp"          # User to run as
run_as_group = "ftp"         # Group to run as
```

### Access Control

```ini
[security]
# Anonymous access
allow_anonymous = false       # Allow anonymous users
allow_guest = false          # Allow guest users

# SSL requirements
require_ssl = false          # Require SSL for all connections

# Command restrictions
allowed_commands = ["USER", "PASS", "QUIT", "PWD", "CWD", "LIST", "RETR", "STOR"]
denied_commands = ["SITE", "SYST", "HELP"]

# Authentication limits
max_login_attempts = 3       # Maximum failed login attempts
login_timeout = 30           # Login timeout in seconds
session_timeout = 3600       # Session timeout in seconds
```

### Advanced Security Features

```ini
[security]
# Path restrictions
restrict_paths = true        # Enable path restrictions
allowed_paths = ["/var/ftp/public", "/var/ftp/shared"]
denied_paths = ["/var/ftp/private", "/var/ftp/admin"]

# IP restrictions
allowed_ips = ["192.168.1.0/24", "10.0.0.0/8"]
denied_ips = ["192.168.1.100", "10.0.0.100"]

# Time restrictions
allowed_hours = ["09:00-17:00"]  # Business hours only
allowed_days = ["Monday-Friday"]  # Weekdays only
```

## 📡 Connection Configuration

### Network Settings

```ini
[connection]
bind_address = "0.0.0.0"     # IP address to bind to
bind_port = 21               # Port to listen on
max_connections = 100        # Maximum concurrent connections
max_connections_per_ip = 10  # Maximum connections per IP address
backlog = 50                 # Connection queue size
```

### Timeout Settings

```ini
[connection]
connection_timeout = 300      # Connection establishment timeout
data_timeout = 300           # Data transfer timeout
idle_timeout = 600           # Idle connection timeout

# Keep-alive settings
keep_alive = true            # Enable TCP keep-alive
keep_alive_interval = 60     # Keep-alive interval
keep_alive_probes = 3        # Keep-alive probe count
```

### Performance Options

```ini
[connection]
tcp_nodelay = true           # Enable TCP_NODELAY
reuse_address = true         # Enable SO_REUSEADDR
tcp_fastopen = false         # Enable TCP Fast Open
```

## 📤 Transfer Configuration

### File Transfer Settings

```ini
[transfer]
max_file_size = 0            # Maximum file size (0 = unlimited)
max_transfer_rate = 0        # Maximum transfer rate (0 = unlimited)
allow_overwrite = true       # Allow file overwrites
allow_resume = true          # Allow transfer resumption
temp_directory = "/tmp/ssftpd"  # Temporary file directory
```

### Transfer Optimization

```ini
[transfer]
buffer_size = 8192           # Transfer buffer size
use_sendfile = true          # Use sendfile() for optimization
use_mmap = false             # Use memory mapping
enable_compression = false   # Enable transfer compression

# File type restrictions
allowed_extensions = ["txt", "pdf", "doc", "docx", "jpg", "png", "gif"]
denied_extensions = ["exe", "bat", "sh", "com"]
```

### Advanced Transfer Features

```ini
[transfer]
# Bandwidth control
bandwidth_limit = 0          # Global bandwidth limit
per_user_bandwidth = true    # Apply bandwidth limits per user
per_connection_bandwidth = false # Apply bandwidth limits per connection

# Transfer validation
validate_checksums = false   # Validate file checksums
checksum_algorithm = "MD5"   # Checksum algorithm (MD5, SHA1, SHA256)
```

## 🌐 Passive Mode Configuration

### Basic Passive Settings

```ini
[passive]
enabled = true               # Enable passive mode
min_port = 1024              # Minimum passive port
max_port = 65535             # Maximum passive port
```

### Advanced Passive Configuration

```ini
[passive]
# IP address configuration
external_ip = ""             # External IP address for NAT
local_ip = ""                # Local IP address for binding
use_external_ip = false      # Use external IP for passive connections
use_local_ip = false         # Use local IP for passive connections

# Port restrictions
allowed_ports = []           # Specific allowed ports
denied_ports = []            # Specific denied ports

# NAT and firewall support
nat_enabled = false          # Enable NAT support
firewall_friendly = true     # Firewall-friendly mode
```

## 🚦 Rate Limiting Configuration

### Basic Rate Limiting

```ini
[rate_limit]
enabled = true               # Enable rate limiting
max_requests_per_minute = 1000
max_connections_per_minute = 100
max_transfers_per_minute = 50
max_bytes_per_minute = 100MB
window_size = 60             # Time window in seconds
```

### Advanced Rate Limiting

```ini
[rate_limit]
# Per-entity limiting
per_ip_limiting = true       # Apply limits per IP address
per_user_limiting = true     # Apply limits per user
per_virtual_host_limiting = false # Apply limits per virtual host

# Burst handling
burst_enabled = true         # Allow burst traffic
burst_multiplier = 2.0       # Burst multiplier
burst_duration = 10          # Burst duration in seconds

# Adaptive limiting
adaptive_limiting = false    # Enable adaptive rate limiting
adaptive_window = 300        # Adaptive window size
```

## 🏠 Virtual Host Configuration

### Basic Virtual Host

```ini
[virtual_hosts.default]
hostname = "default"         # Virtual host hostname
document_root = "/var/ftp"   # Document root directory
welcome_message = "Welcome to Simple-Secure FTP Daemon"
banner_message = "Simple-Secure FTP Daemon Ready"
enabled = true               # Enable this virtual host
default = true               # Set as default virtual host
```

### Virtual Host SSL

```ini
[virtual_hosts.example.ssl]
enabled = true               # Enable SSL for this virtual host
certificate_file = "/etc/ssl/certs/example.com.crt"
private_key_file = "/etc/ssl/private/example.com.key"
ca_certificate_file = "/etc/ssl/certs/ca.crt"
```

### Virtual Host Security

```ini
[virtual_hosts.example.security]
chroot_enabled = true        # Enable chroot for this virtual host
chroot_directory = "/var/ftp/example"
drop_privileges = true
run_as_user = "ftp"
run_as_group = "ftp"
allow_anonymous = false
allow_guest = false
require_ssl = true           # Require SSL for this virtual host
```

### Virtual Host Transfer Settings

```ini
[virtual_hosts.example.transfer]
max_file_size = 100MB        # Maximum file size for this virtual host
max_transfer_rate = 1MB      # Maximum transfer rate
allow_overwrite = false      # Disallow file overwrites
allow_resume = true          # Allow transfer resumption
```

## 👤 User Configuration

### Basic User

```ini
[users.myuser]
username = "myuser"          # Username
password_hash = "$2y$10$hashed_password"  # Hashed password
home_directory = "/var/ftp/myuser"  # User home directory
shell = "/bin/false"         # User shell
group = "ftp"                # User group
enabled = true               # Enable this user
```

### User Types

```ini
[users.anonymous]
username = "anonymous"       # Anonymous user
password_hash = ""           # No password for anonymous
home_directory = "/var/ftp/public"
anonymous = true             # Mark as anonymous user
guest = false                # Not a guest user

[users.guest]
username = "guest"           # Guest user
password_hash = ""
home_directory = "/var/ftp/guest"
anonymous = false
guest = true                 # Mark as guest user
```

### User Permissions

```ini
[users.admin]
# Full permissions
permissions = ["READ", "WRITE", "DELETE", "RENAME", "MKDIR", "RMDIR", 
               "LIST", "UPLOAD", "DOWNLOAD", "APPEND", "ADMIN"]

# No denied operations
denied_operations = []

[users.restricted]
# Limited permissions
permissions = ["READ", "LIST", "DOWNLOAD"]
denied_operations = ["WRITE", "DELETE", "UPLOAD", "ADMIN"]
```

### User Restrictions

```ini
[users.limited]
# Connection limits
max_connections = 2          # Maximum concurrent connections
max_transfer_rate = 512KB    # Maximum transfer rate
max_file_size = 50MB         # Maximum file size

# Session management
session_timeout = 1800       # 30 minutes session timeout

# Path restrictions
allowed_paths = ["/var/ftp/public", "/var/ftp/shared"]
denied_paths = ["/var/ftp/private", "/var/ftp/admin"]
```

## 🔧 Advanced Configuration

### Environment Variables

```bash
# Override configuration with environment variables
export SIMPLE_FTPD_CONFIG_FILE="/path/to/config.conf"
export SIMPLE_FTPD_LOG_LEVEL="DEBUG"
export SIMPLE_FTPD_BIND_ADDRESS="127.0.0.1"
export SIMPLE_FTPD_BIND_PORT="2121"
```

### Configuration Inheritance

```ini
# Global settings apply to all virtual hosts
enable_ssl = true
max_connections = 100

# Virtual host specific overrides
[virtual_hosts.secure]
enable_ssl = true            # Override global setting
max_connections = 50         # Override global setting

[virtual_hosts.public]
enable_ssl = false           # Override global setting
max_connections = 200        # Override global setting
```

### Configuration Validation

```bash
# Validate configuration file
ssftpd --test-config --config /etc/ssftpd/ssftpd.conf

# Validate and show detailed results
ssftpd --validate --config /etc/ssftpd/ssftpd.conf
```

## 📊 Configuration Examples

### Minimal Configuration

```ini
# Minimal working configuration
server_name = "My FTP Server"
bind_address = "0.0.0.0"
bind_port = 21
working_directory = "/var/ftp"
enable_ssl = false
enable_user_management = true

[users.admin]
username = "admin"
password_hash = "$2y$10$hashed_password"
home_directory = "/var/ftp/admin"
permissions = ["READ", "WRITE", "LIST", "UPLOAD", "DOWNLOAD"]
```

### Production Configuration

```ini
# Production-ready configuration
server_name = "Production FTP Server"
daemon_mode = true
working_directory = "/var/ftp"
enable_ssl = true
enable_virtual_hosts = true
enable_rate_limiting = true

[ssl]
enabled = true
certificate_file = "/etc/ssl/certs/ftp.example.com.crt"
private_key_file = "/etc/ssl/private/ftp.example.com.key"
min_tls_version = 1.2

[security]
chroot_enabled = true
chroot_directory = "/var/ftp/chroot"
drop_privileges = true
run_as_user = "ftp"
max_login_attempts = 3

[connection]
max_connections = 200
max_connections_per_ip = 20
connection_timeout = 300
idle_timeout = 600

[rate_limit]
enabled = true
max_requests_per_minute = 2000
max_connections_per_minute = 200
max_bytes_per_minute = 1GB
```

### Development Configuration

```ini
# Development configuration
server_name = "Development FTP Server"
foreground_mode = true
debug_mode = true
verbose_logging = true
trace_commands = true

[logging]
log_level = "DEBUG"
log_commands = true
log_transfers = true
log_format = "EXTENDED"

[development]
profile_performance = true
log_socket_events = "all"
```

## 🚨 Configuration Best Practices

### Security Best Practices

1. **Always use SSL/TLS** in production environments
2. **Enable chroot** for user isolation
3. **Drop privileges** after startup
4. **Restrict commands** to only necessary ones
5. **Use strong passwords** and hash them properly
6. **Limit connection rates** to prevent abuse
7. **Monitor logs** regularly for suspicious activity

### Performance Best Practices

1. **Tune buffer sizes** based on your network
2. **Enable sendfile** for large file transfers
3. **Use appropriate thread pool sizes** (CPU cores × 2)
4. **Monitor memory usage** and adjust limits
5. **Use connection pooling** for high-traffic scenarios

### Maintenance Best Practices

1. **Backup configurations** before major changes
2. **Test configurations** in staging environments
3. **Use version control** for configuration files
4. **Document customizations** and their purposes
5. **Regular security audits** of configurations

## 🔍 Troubleshooting Configuration

### Common Configuration Issues

#### SSL Certificate Problems
```bash
# Check certificate validity
openssl x509 -in /etc/ssftpd/ssl/server.crt -text -noout

# Verify certificate chain
openssl verify /etc/ssftpd/ssl/server.crt

# Check private key permissions
ls -la /etc/ssftpd/ssl/server.key
```

#### Permission Issues
```bash
# Check directory permissions
ls -la /var/ftp/
ls -la /etc/ssftpd/

# Verify user ownership
id ftp
groups ftp
```

#### Network Issues
```bash
# Check port binding
sudo netstat -tlnp | grep :21

# Test local connectivity
telnet localhost 21

# Check firewall rules
sudo ufw status
sudo firewall-cmd --list-all
```

### Configuration Validation

```bash
# Syntax check
ssftpd --test-config --config /etc/ssftpd/ssftpd.conf

# Full validation
ssftpd --validate --config /etc/ssftpd/ssftpd.conf

# Check specific sections
grep -n "\[ssl\]" /etc/ssftpd/ssftpd.conf
grep -n "\[users" /etc/ssftpd/ssftpd.conf
```

## 📚 Next Steps

Now that you understand the configuration system:

1. **Customize your configuration** based on your needs
2. **Set up SSL/TLS** for secure connections
3. **Configure virtual hosts** for multi-domain support
4. **Implement user management** with proper permissions
5. **Set up monitoring** and logging
6. **Test your configuration** thoroughly
7. **Deploy to production** with security best practices

For more advanced topics, see:
- [SSL/TLS Configuration](ssl-configuration.md)
- [Virtual Host Configuration](virtual-hosts.md)
- [Security Hardening](security.md)
- [Performance Tuning](performance.md)
