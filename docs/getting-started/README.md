# Getting Started with Simple-Secure FTP Daemon

Welcome to Simple-Secure FTP Daemon! This guide will help you get up and running with your first FTP server in minutes.

## 🎯 What You'll Learn

By the end of this guide, you'll have:
- ✅ A working FTP server running on your system
- ✅ Basic configuration set up
- ✅ Your first user account created
- ✅ Understanding of essential commands

## 📋 Prerequisites

Before starting, ensure you have:

- **Operating System**: Linux, macOS, or Windows
- **Administrative Access**: Root/sudo privileges for installation
- **Network Access**: Ability to configure firewall rules
- **Basic Knowledge**: Familiarity with command line operations

## 🚀 Quick Start (5 Minutes)

### Step 1: Installation

Choose your installation method:

#### Option A: Package Installation (Recommended)
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install simple-sftpd

# CentOS/RHEL
sudo yum install simple-sftpd
# or
sudo dnf install simple-sftpd

# macOS
brew install simple-sftpd
```

#### Option B: Build from Source
```bash
# Clone the repository
git clone https://github.com/simple-sftpd/simple-sftpd.git
cd simple-sftpd

# Install dependencies and build
make install-dev
make build
sudo make install
```

### Step 2: Basic Configuration

```bash
# Copy example configuration
sudo cp /etc/simple-sftpd/simple-sftpd.conf.example /etc/simple-sftpd/simple-sftpd.conf

# Edit configuration
sudo nano /etc/simple-sftpd/simple-sftpd.conf
```

**Essential settings to configure:**
```ini
# Basic server settings
server_name = "My FTP Server"
bind_address = "0.0.0.0"  # Listen on all interfaces
bind_port = 21             # Standard FTP port

# Create directories
working_directory = "/var/ftp"
log_file = "/var/log/simple-sftpd/simple-sftpd.log"

# Enable basic features
enable_ssl = false         # Start without SSL for simplicity
enable_user_management = true
```

### Step 3: Create Required Directories

```bash
# Create FTP directory structure
sudo mkdir -p /var/ftp /var/log/simple-sftpd
sudo chown ftp:ftp /var/ftp
sudo chmod 755 /var/ftp

# Create user configuration directory
sudo mkdir -p /etc/simple-sftpd/users
```

### Step 4: Add Your First User

```bash
# Add a user account
sudo simple-sftpd user add \
  --username myuser \
  --password mypassword \
  --home /var/ftp/myuser \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD

# Create user directory
sudo mkdir -p /var/ftp/myuser
sudo chown myuser:ftp /var/ftp/myuser
```

### Step 5: Start the Server

```bash
# Test configuration first
simple-sftpd --test-config --config /etc/simple-sftpd/simple-sftpd.conf

# Start in foreground (for testing)
sudo simple-sftpd start --config /etc/simple-sftpd/simple-sftpd.conf
```

### Step 6: Test Your Connection

Open a new terminal and test with an FTP client:

```bash
# Using command line FTP client
ftp localhost

# Or use a GUI client like FileZilla
# Host: localhost
# Port: 21
# Username: myuser
# Password: mypassword
```

## 🔧 Basic Configuration Explained

### Configuration File Structure

The configuration file uses an INI format with sections:

```ini
# Global settings (no section)
server_name = "My FTP Server"
enable_ssl = false

# SSL configuration section
[ssl]
enabled = false
certificate_file = "/path/to/cert.crt"

# Connection settings
[connection]
bind_address = "0.0.0.0"
bind_port = 21
max_connections = 100

# User settings
[users.myuser]
username = "myuser"
password_hash = "hashed_password"
home_directory = "/var/ftp/myuser"
permissions = ["READ", "WRITE", "LIST", "UPLOAD", "DOWNLOAD"]
```

### Essential Configuration Options

| Option | Description | Default | Recommended |
|--------|-------------|---------|-------------|
| `bind_address` | IP address to bind to | `0.0.0.0` | `0.0.0.0` (all interfaces) |
| `bind_port` | Port to listen on | `21` | `21` (standard FTP) |
| `working_directory` | Base FTP directory | `/var/ftp` | `/var/ftp` |
| `enable_ssl` | Enable SSL/TLS | `false` | `true` (production) |
| `max_connections` | Maximum concurrent connections | `100` | `50-200` |

## 👤 User Management Basics

### User Types

1. **Regular Users**: Full access to their home directory
2. **Anonymous Users**: Public access (read-only)
3. **Guest Users**: Limited access with restrictions
4. **Admin Users**: Administrative privileges

### Permission System

Available permissions:
- `READ`: Read files and directories
- `WRITE`: Create and modify files
- `DELETE`: Remove files and directories
- `LIST`: List directory contents
- `UPLOAD`: Upload files
- `DOWNLOAD`: Download files
- `ADMIN`: Administrative operations

### Example User Configurations

```bash
# Regular user with full access
sudo simple-sftpd user add \
  --username developer \
  --password devpass123 \
  --home /var/ftp/developer \
  --permissions READ,WRITE,DELETE,LIST,UPLOAD,DOWNLOAD

# Anonymous user (public access)
sudo simple-sftpd user add \
  --username anonymous \
  --home /var/ftp/public \
  --anonymous \
  --permissions READ,LIST,DOWNLOAD

# Restricted user
sudo simple-sftpd user add \
  --username guest \
  --password guest123 \
  --home /var/ftp/guest \
  --permissions READ,LIST,DOWNLOAD \
  --max-connections 1 \
  --max-transfer-rate 512KB
```

## 🚀 Running as a Service

### Linux (systemd)

```bash
# Enable and start service
sudo systemctl enable simple-sftpd
sudo systemctl start simple-sftpd

# Check status
sudo systemctl status simple-sftpd

# View logs
sudo journalctl -u simple-sftpd -f
```

### macOS (launchd)

```bash
# Load service
sudo launchctl load /Library/LaunchDaemons/com.blburns.simple-sftpd.plist

# Start service
sudo launchctl start com.blburns.simple-sftpd

# Check status
sudo launchctl list | grep simple-sftpd
```

### Windows

```cmd
# Install service
sc create simple-sftpd binPath= "C:\Program Files\simple-sftpd\bin\simple-sftpd.exe"

# Start service
sc start simple-sftpd

# Check status
sc query simple-sftpd
```

## 🔒 Basic Security Setup

### Firewall Configuration

```bash
# Ubuntu/Debian (ufw)
sudo ufw allow 21/tcp
sudo ufw allow 1024:65535/tcp  # Passive mode ports

# CentOS/RHEL (firewalld)
sudo firewall-cmd --permanent --add-service=ftp
sudo firewall-cmd --permanent --add-port=1024-65535/tcp
sudo firewall-cmd --reload

# macOS
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --add /usr/local/bin/simple-sftpd
```

### User Isolation

```bash
# Enable chroot for users
sudo mkdir -p /var/ftp/chroot
sudo chown root:root /var/ftp/chroot
sudo chmod 755 /var/ftp/chroot

# Edit configuration
sudo nano /etc/simple-sftpd/simple-sftpd.conf

# Add to [security] section:
chroot_enabled = true
chroot_directory = "/var/ftp/chroot"
```

## 📊 Monitoring Your Server

### Basic Commands

```bash
# Check server status
simple-sftpd status

# View active connections
simple-sftpd connections

# Show server statistics
simple-sftpd stats

# View logs
tail -f /var/log/simple-sftpd/simple-sftpd.log
```

### Log Analysis

```bash
# View recent connections
grep "CONNECT" /var/log/simple-sftpd/simple-sftpd.log

# Check for errors
grep "ERROR" /var/log/simple-sftpd/simple-sftpd.log

# Monitor uploads/downloads
grep "TRANSFER" /var/log/simple-sftpd/simple-sftpd.log
```

## 🚨 Troubleshooting

### Common Issues

#### Server Won't Start
```bash
# Check configuration
simple-sftpd --test-config --config /etc/simple-sftpd/simple-sftpd.conf

# Check permissions
ls -la /var/ftp
ls -la /etc/simple-sftpd/

# Check ports
sudo netstat -tlnp | grep :21
```

#### Connection Refused
```bash
# Check firewall
sudo ufw status
sudo firewall-cmd --list-all

# Check service status
sudo systemctl status simple-sftpd
```

#### Permission Denied
```bash
# Check user permissions
sudo simple-sftpd user list

# Check directory permissions
ls -la /var/ftp/myuser/

# Verify user home directory
sudo simple-sftpd user show --username myuser
```

## 📚 Next Steps

Now that you have a basic FTP server running:

1. **Configure SSL/TLS**: See [SSL Configuration Guide](../configuration/ssl-configuration.md)
2. **Set up Virtual Hosts**: See [Virtual Host Configuration](../configuration/virtual-hosts.md)
3. **Advanced Security**: See [Security Hardening](../configuration/security.md)
4. **Performance Tuning**: See [Performance Optimization](../configuration/performance.md)

## 🆘 Need Help?

- **Documentation**: Browse the full [documentation index](../README.md)
- **Examples**: Check [configuration examples](../examples/README.md)
- **Issues**: Report problems on [GitHub Issues](https://github.com/simple-sftpd/simple-sftpd/issues)
- **Discussions**: Join [GitHub Discussions](https://github.com/simple-sftpd/simple-sftpd/discussions)

---

**Congratulations!** You now have a working FTP server. Continue exploring the documentation to unlock more advanced features and configurations.
