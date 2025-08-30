# Simple FTP Daemon (simple-ftpd)

A secure, configurable, and feature-rich FTP server written in C++ for Linux, macOS, and Windows.

## Features

### Core FTP Functionality
- **RFC 959 Compliant**: Full FTP protocol implementation
- **Active/Passive Mode**: Support for both connection modes
- **File Transfer**: Upload, download, append, and resume capabilities
- **Directory Operations**: List, create, remove, and navigate directories
- **File Management**: Rename, delete, and modify file permissions

### Security Features
- **SSL/TLS Support**: Secure file transfers with OpenSSL
- **User Authentication**: Multiple authentication methods (password, hash, PAM, LDAP)
- **Access Control**: Granular permissions and path restrictions
- **Chroot Support**: Isolated file system access
- **Privilege Dropping**: Security hardening for production use
- **Rate Limiting**: Protection against abuse and DoS attacks

### Virtual Hosting
- **Multi-Domain Support**: Host multiple FTP sites on one server
- **Per-Host Configuration**: Individual settings for each virtual host
- **SSL Certificate Management**: Separate certificates per domain
- **Access Control**: User restrictions per virtual host

### User Management
- **Flexible Permissions**: Granular control over user capabilities
- **Anonymous Access**: Configurable anonymous user support
- **Guest Accounts**: Limited access for temporary users
- **Connection Limits**: Per-user connection and transfer restrictions
- **Session Management**: Timeout and activity tracking

### Performance & Monitoring
- **Multi-threaded**: Efficient handling of multiple connections
- **Connection Pooling**: Optimized resource management
- **Transfer Optimization**: Sendfile and memory-mapped I/O support
- **Statistics**: Comprehensive usage and performance metrics
- **Logging**: Advanced logging with rotation and filtering

### Platform Support
- **Cross-Platform**: Linux, macOS, and Windows
- **Native Builds**: Optimized for each platform
- **Package Management**: DEB, RPM, PKG, and MSI packages
- **Service Integration**: systemd, launchd, and Windows services

## Quick Start

### Prerequisites

- **C++17 Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake 3.16+**: Build system
- **OpenSSL**: SSL/TLS support
- **jsoncpp**: JSON configuration parsing

### Installation

#### From Source

```bash
# Clone the repository
git clone https://github.com/simple-ftpd/simple-ftpd.git
cd simple-ftpd

# Build the project
make install-dev  # Install development dependencies
make build        # Build the application
make install      # Install system-wide
```

#### From Packages

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install simple-ftpd
```

**CentOS/RHEL:**
```bash
sudo yum install simple-ftpd
# or
sudo dnf install simple-ftpd
```

**macOS:**
```bash
brew install simple-ftpd
```

### Configuration

1. **Copy the example configuration:**
```bash
sudo cp /etc/simple-ftpd/simple-ftpd.conf.example /etc/simple-ftpd/simple-ftpd.conf
```

2. **Edit the configuration file:**
```bash
sudo nano /etc/simple-ftpd/simple-ftpd.conf
```

3. **Create necessary directories:**
```bash
sudo mkdir -p /var/ftp /var/log/simple-ftpd
sudo chown ftp:ftp /var/ftp
```

### Running the Server

#### Basic Usage

```bash
# Start in foreground (for testing)
sudo simple-ftpd start --config /etc/simple-ftpd/simple-ftpd.conf

# Start as daemon
sudo simple-ftpd --daemon start

# Test configuration
simple-ftpd --test-config --config /etc/simple-ftpd/simple-ftpd.conf
```

#### Service Management

**Linux (systemd):**
```bash
sudo systemctl enable simple-ftpd
sudo systemctl start simple-ftpd
sudo systemctl status simple-ftpd
```

**macOS (launchd):**
```bash
sudo launchctl load /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
sudo launchctl start com.blburns.simple-ftpd
```

**Windows:**
```cmd
sc create simple-ftpd binPath= "C:\Program Files\simple-ftpd\bin\simple-ftpd.exe"
sc start simple-ftpd
```

## Configuration

### Main Configuration File

The main configuration file (`simple-ftpd.conf`) supports both INI and JSON formats. Here's an example INI configuration:

```ini
# Global server settings
server_name = "Simple FTP Daemon"
server_version = "0.1.0"
enable_ssl = true
enable_virtual_hosts = true

# SSL Configuration
[ssl]
enabled = true
certificate_file = "/etc/simple-ftpd/ssl/server.crt"
private_key_file = "/etc/simple-ftpd/ssl/server.key"

# Connection settings
[connection]
bind_address = "0.0.0.0"
bind_port = 21
max_connections = 100

# Virtual hosts
[virtual_hosts.default]
hostname = "default"
document_root = "/var/ftp"
enabled = true

# Users
[users.admin]
username = "admin"
password_hash = "$2y$10$hashed_password"
home_directory = "/var/ftp/admin"
permissions = ["READ", "WRITE", "LIST", "UPLOAD", "DOWNLOAD"]
```

### User Management

#### Adding Users

```bash
# Add a new user
sudo simple-ftpd user add \
  --username john \
  --password secret \
  --home /var/ftp/john \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD

# Add anonymous user
sudo simple-ftpd user add \
  --username anonymous \
  --home /var/ftp/public \
  --anonymous \
  --permissions READ,LIST,DOWNLOAD
```

#### User Permissions

Available permissions:
- `READ`: Read files and directories
- `WRITE`: Write/create files and directories
- `DELETE`: Delete files and directories
- `RENAME`: Rename files and directories
- `MKDIR`: Create directories
- `RMDIR`: Remove directories
- `LIST`: List directory contents
- `UPLOAD`: Upload files
- `DOWNLOAD`: Download files
- `APPEND`: Append to files
- `ADMIN`: Administrative operations

### Virtual Hosts

#### Adding Virtual Hosts

```bash
# Add a new virtual host
sudo simple-ftpd virtual add \
  --hostname ftp.example.com \
  --root /var/ftp/example \
  --ssl \
  --certificate /etc/ssl/certs/example.com.crt \
  --private-key /etc/ssl/private/example.com.key
```

#### Virtual Host Configuration

Each virtual host can have:
- Separate document root
- Individual SSL certificates
- Custom security settings
- User access restrictions
- Transfer rate limits

### SSL/TLS Configuration

#### Generating Self-Signed Certificates

```bash
# Generate certificate for a domain
sudo simple-ftpd ssl generate \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city San Francisco \
  --organization "Example Corp" \
  --email admin@example.com
```

#### Installing Certificates

```bash
# Install existing certificates
sudo simple-ftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key \
  --ca-certificate /path/to/ca.crt
```

## Command Line Interface

### Server Commands

```bash
# Start the server
simple-ftpd start [--config FILE] [--daemon] [--foreground]

# Stop the server
simple-ftpd stop

# Restart the server
simple-ftpd restart

# Show server status
simple-ftpd status

# Reload configuration
simple-ftpd reload
```

### User Management Commands

```bash
# List all users
simple-ftpd user list

# Add user
simple-ftpd user add --username NAME --password PASS --home DIR

# Remove user
simple-ftpd user remove --username NAME

# Modify user
simple-ftpd user modify --username NAME --permissions READ,WRITE

# Change password
simple-ftpd user password --username NAME --password NEW_PASS
```

### Virtual Host Commands

```bash
# List virtual hosts
simple-ftpd virtual list

# Add virtual host
simple-ftpd virtual add --hostname DOMAIN --root DIR

# Remove virtual host
simple-ftpd virtual remove --hostname DOMAIN

# Enable/disable virtual host
simple-ftpd virtual enable --hostname DOMAIN
simple-ftpd virtual disable --hostname DOMAIN
```

### SSL Management Commands

```bash
# Generate certificate
simple-ftpd ssl generate --hostname DOMAIN

# Install certificate
simple-ftpd ssl install --hostname DOMAIN --cert FILE --key FILE

# Show SSL status
simple-ftpd ssl status

# Renew certificate
simple-ftpd ssl renew --hostname DOMAIN
```

## Development

### Building from Source

```bash
# Clone and setup
git clone https://github.com/simple-ftpd/simple-ftpd.git
cd simple-ftpd

# Install dependencies
make install-dev

# Build options
make debug      # Debug build
make release    # Release build
make test       # Run tests
make clean      # Clean build artifacts

# Package creation
make package    # Create packages for current platform
```

### Project Structure

```
simple-ftpd/
├── include/simple-ftpd/     # Header files
│   ├── ftp_server.hpp       # Main server class
│   ├── ftp_connection.hpp   # Connection handling
│   ├── ftp_user.hpp         # User management
│   ├── ftp_virtual_host.hpp # Virtual host support
│   ├── ftp_server_config.hpp # Configuration
│   ├── logger.hpp           # Logging system
│   └── platform.hpp         # Platform abstraction
├── src/                     # Source files
│   ├── core/               # Core implementation
│   ├── utils/              # Utility functions
│   └── main.cpp            # Main application
├── config/                  # Configuration files
├── tools/                   # Management tools
├── docs/                    # Documentation
├── scripts/                 # Build and deployment scripts
├── CMakeLists.txt          # CMake build configuration
└── Makefile                # Make build system
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass
6. Submit a pull request

### Testing

```bash
# Run all tests
make test

# Run specific test suites
cd build && ctest -R "unit_tests"
cd build && ctest -R "integration_tests"

# Run with coverage (Linux only)
make coverage

# Run with memory checking (Linux only)
make memcheck
```

## Troubleshooting

### Common Issues

#### Server Won't Start

1. **Check configuration:**
```bash
simple-ftpd --test-config --config /etc/simple-ftpd/simple-ftpd.conf
```

2. **Check permissions:**
```bash
sudo chown -R ftp:ftp /var/ftp
sudo chmod 755 /var/ftp
```

3. **Check ports:**
```bash
sudo netstat -tlnp | grep :21
```

#### SSL Connection Issues

1. **Verify certificate files:**
```bash
openssl x509 -in /etc/simple-ftpd/ssl/server.crt -text -noout
```

2. **Check certificate permissions:**
```bash
sudo chmod 600 /etc/simple-ftpd/ssl/server.key
sudo chown ftp:ftp /etc/simple-ftpd/ssl/server.key
```

#### Performance Issues

1. **Check system resources:**
```bash
top
iostat
netstat -i
```

2. **Adjust configuration:**
```ini
[connection]
max_connections = 50
thread_pool_size = 4

[transfer]
buffer_size = 16384
use_sendfile = true
```

### Log Files

- **Main log**: `/var/log/simple-ftpd/simple-ftpd.log`
- **Access log**: `/var/log/simple-ftpd/access.log`
- **Error log**: `/var/log/simple-ftpd/error.log`

### Debug Mode

Enable debug logging:

```ini
[logging]
log_level = "DEBUG"
log_commands = true
log_transfers = true

# Development settings
debug_mode = true
verbose_logging = true
trace_commands = true
```

## Security Considerations

### Production Deployment

1. **Use strong SSL certificates**
2. **Enable chroot for users**
3. **Drop privileges to non-root user**
4. **Implement rate limiting**
5. **Restrict allowed commands**
6. **Use firewall rules**
7. **Regular security updates**

### Network Security

```bash
# Firewall rules (iptables)
sudo iptables -A INPUT -p tcp --dport 21 -j ACCEPT
sudo iptables -A INPUT -p tcp --dport 1024:65535 -j ACCEPT

# Or with ufw (Ubuntu)
sudo ufw allow 21/tcp
sudo ufw allow 1024:65535/tcp
```

### User Security

1. **Strong password policies**
2. **Limited permissions**
3. **Path restrictions**
4. **Connection limits**
5. **Session timeouts**

## Performance Tuning

### System Tuning

```bash
# Increase file descriptor limits
echo "* soft nofile 65536" >> /etc/security/limits.conf
echo "* hard nofile 65536" >> /etc/security/limits.conf

# Kernel parameters
echo "net.core.somaxconn = 65536" >> /etc/sysctl.conf
echo "net.ipv4.tcp_max_syn_backlog = 65536" >> /etc/sysctl.conf
sysctl -p
```

### Application Tuning

```ini
[connection]
max_connections = 200
backlog = 100
keep_alive = true
tcp_nodelay = true

[transfer]
buffer_size = 32768
use_sendfile = true
use_mmap = true

# Performance settings
thread_pool_size = 16
enable_compression = true
cache_size = 100MB
```

## Monitoring and Metrics

### Built-in Monitoring

```bash
# Show server statistics
simple-ftpd status

# Show performance metrics
simple-ftpd metrics

# Show connection information
simple-ftpd connections
```

### External Monitoring

- **Prometheus**: Metrics endpoint at `/metrics`
- **Grafana**: Dashboard templates included
- **Log aggregation**: Structured logging support
- **Health checks**: HTTP health endpoint

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **Documentation**: [docs/](docs/)
- **Issues**: [GitHub Issues](https://github.com/simple-ftpd/simple-ftpd/issues)
- **Discussions**: [GitHub Discussions](https://github.com/simple-ftpd/simple-ftpd/discussions)
- **Email**: SimpleDaemons

## Acknowledgments

- **OpenSSL**: SSL/TLS implementation
- **jsoncpp**: JSON parsing library
- **CMake**: Build system
- **FTP RFC 959**: Protocol specification

## Changelog

### v0.1.0 (Current)
- Initial release
- Core FTP server functionality
- SSL/TLS support
- Virtual hosting
- User management
- Multi-platform support
- Comprehensive configuration system
