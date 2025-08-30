# User Guide

This guide covers all user operations and management tasks for Simple FTP Daemon, from basic server management to advanced user administration.

## 🎯 What You'll Learn

By the end of this guide, you'll be able to:
- ✅ Manage FTP server operations (start, stop, restart)
- ✅ Add, modify, and remove users
- ✅ Configure virtual hosts and domains
- ✅ Manage SSL certificates
- ✅ Monitor server performance and logs
- ✅ Troubleshoot common issues

## 🚀 Server Management

### Basic Server Operations

#### Starting the Server

```bash
# Start in foreground (for testing)
sudo simple-ftpd start --config /etc/simple-ftpd/simple-ftpd.conf

# Start as daemon
sudo simple-ftpd --daemon start --config /etc/simple-ftpd/simple-ftpd.conf

# Start with specific configuration
sudo simple-ftpd start --config /path/to/custom.conf
```

#### Stopping the Server

```bash
# Graceful shutdown
sudo simple-ftpd stop

# Force stop (if graceful fails)
sudo simple-ftpd stop --force

# Stop specific instance
sudo simple-ftpd stop --pid-file /var/run/simple-ftpd.pid
```

#### Restarting the Server

```bash
# Restart with current configuration
sudo simple-ftpd restart

# Restart with new configuration
sudo simple-ftpd restart --config /path/to/new.conf

# Reload configuration without restart
sudo simple-ftpd reload
```

#### Server Status

```bash
# Check server status
simple-ftpd status

# Show detailed status
simple-ftpd status --verbose

# Show status in JSON format
simple-ftpd status --format json

# Check specific components
simple-ftpd status --component ssl
simple-ftpd status --component users
simple-ftpd status --component virtual-hosts
```

### Service Management

#### Linux (systemd)

```bash
# Enable service at boot
sudo systemctl enable simple-ftpd

# Start service
sudo systemctl start simple-ftpd

# Stop service
sudo systemctl stop simple-ftpd

# Restart service
sudo systemctl restart simple-ftpd

# Check status
sudo systemctl status simple-ftpd

# View logs
sudo journalctl -u simple-ftpd -f
sudo journalctl -u simple-ftpd --since "1 hour ago"
```

#### macOS (launchd)

```bash
# Load service
sudo launchctl load /Library/LaunchDaemons/com.blburns.simple-ftpd.plist

# Start service
sudo launchctl start com.blburns.simple-ftpd

# Stop service
sudo launchctl stop com.blburns.simple-ftpd

# Unload service
sudo launchctl unload /Library/LaunchDaemons/com.blburns.simple-ftpd.plist

# Check status
sudo launchctl list | grep simple-ftpd
```

#### Windows

```cmd
# Install service
sc create simple-ftpd binPath= "C:\Program Files\simple-ftpd\bin\simple-ftpd.exe"

# Start service
sc start simple-ftpd

# Stop service
sc stop simple-ftpd

# Delete service
sc delete simple-ftpd

# Check status
sc query simple-ftpd
```

## 👤 User Management

### Adding Users

#### Basic User Creation

```bash
# Create a regular user
sudo simple-ftpd user add \
  --username john \
  --password secret123 \
  --home /var/ftp/john \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD

# Create user with specific group
sudo simple-ftpd user add \
  --username developer \
  --password devpass123 \
  --home /var/ftp/developer \
  --group developers \
  --permissions READ,WRITE,DELETE,LIST,UPLOAD,DOWNLOAD,ADMIN
```

#### Anonymous User

```bash
# Create anonymous user for public access
sudo simple-ftpd user add \
  --username anonymous \
  --home /var/ftp/public \
  --anonymous \
  --permissions READ,LIST,DOWNLOAD

# Create guest user with restrictions
sudo simple-ftpd user add \
  --username guest \
  --password guest123 \
  --home /var/ftp/guest \
  --guest \
  --permissions READ,LIST,DOWNLOAD \
  --max-connections 1 \
  --max-transfer-rate 256KB
```

#### Advanced User Configuration

```bash
# User with connection limits
sudo simple-ftpd user add \
  --username limited \
  --password limpass123 \
  --home /var/ftp/limited \
  --permissions READ,LIST,DOWNLOAD \
  --max-connections 2 \
  --max-transfer-rate 512KB \
  --max-file-size 25MB \
  --session-timeout 1800

# User with path restrictions
sudo simple-ftpd user add \
  --username restricted \
  --password res123 \
  --home /var/ftp/restricted \
  --permissions READ,LIST,DOWNLOAD \
  --allowed-paths /var/ftp/public,/var/ftp/shared \
  --denied-paths /var/ftp/private,/var/ftp/admin
```

### Modifying Users

#### Password Changes

```bash
# Change user password
sudo simple-ftpd user password \
  --username john \
  --password newpassword123

# Change password interactively
sudo simple-ftpd user password --username john
# Will prompt for new password
```

#### Permission Updates

```bash
# Grant additional permissions
sudo simple-ftpd user grant \
  --username john \
  --permissions DELETE,RENAME

# Revoke permissions
sudo simple-ftpd user revoke \
  --username john \
  --permissions DELETE

# Set complete permission set
sudo simple-ftpd user permissions \
  --username john \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD
```

#### User Properties

```bash
# Update home directory
sudo simple-ftpd user modify \
  --username john \
  --home /var/ftp/newhome

# Update connection limits
sudo simple-ftpd user modify \
  --username john \
  --max-connections 5 \
  --max-transfer-rate 1MB

# Update session timeout
sudo simple-ftpd user modify \
  --username john \
  --session-timeout 3600
```

### User Information

#### Listing Users

```bash
# List all users
sudo simple-ftpd user list

# List users with details
sudo simple-ftpd user list --verbose

# List users in specific format
sudo simple-ftpd user list --format json
sudo simple-ftpd user list --format table

# Filter users
sudo simple-ftpd user list --enabled-only
sudo simple-ftpd user list --anonymous-only
sudo simple-ftpd user list --guest-only
```

#### User Details

```bash
# Show user information
sudo simple-ftpd user show --username john

# Show user statistics
sudo simple-ftpd user stats --username john

# Show user permissions
sudo simple-ftpd user permissions --username john

# Show user connections
sudo simple-ftpd user connections --username john
```

#### User Status

```bash
# Enable user
sudo simple-ftpd user enable --username john

# Disable user
sudo simple-ftpd user disable --username john

# Lock user account
sudo simple-ftpd user lock --username john

# Unlock user account
sudo simple-ftpd user unlock --username john
```

### Removing Users

```bash
# Remove user (keeps files)
sudo simple-ftpd user remove --username john

# Remove user and files
sudo simple-ftpd user remove --username john --remove-files

# Remove user with confirmation
sudo simple-ftpd user remove --username john --confirm

# Force remove (if user is connected)
sudo simple-ftpd user remove --username john --force
```

## 🏠 Virtual Host Management

### Adding Virtual Hosts

#### Basic Virtual Host

```bash
# Create default virtual host
sudo simple-ftpd virtual add \
  --hostname default \
  --root /var/ftp \
  --welcome "Welcome to Default FTP Server" \
  --banner "Default FTP Server Ready"

# Create domain-specific virtual host
sudo simple-ftpd virtual add \
  --hostname ftp.example.com \
  --root /var/ftp/example \
  --welcome "Welcome to Example.com FTP Server" \
  --banner "Example.com FTP Server Ready"
```

#### Virtual Host with SSL

```bash
# Create SSL-enabled virtual host
sudo simple-ftpd virtual add \
  --hostname secure.example.com \
  --root /var/ftp/secure \
  --ssl \
  --certificate /etc/ssl/certs/secure.example.com.crt \
  --private-key /etc/ssl/private/secure.example.com.key \
  --require-ssl

# Create virtual host with custom SSL settings
sudo simple-ftpd virtual add \
  --hostname custom.example.com \
  --root /var/ftp/custom \
  --ssl \
  --certificate /etc/ssl/certs/custom.example.com.crt \
  --private-key /etc/ssl/private/custom.example.com.key \
  --min-tls-version 1.2 \
  --cipher-suite "ECDHE-RSA-AES256-GCM-SHA384"
```

#### Advanced Virtual Host Configuration

```bash
# Virtual host with security restrictions
sudo simple-ftpd virtual add \
  --hostname restricted.example.com \
  --root /var/ftp/restricted \
  --chroot \
  --chroot-directory /var/ftp/restricted \
  --drop-privileges \
  --run-as-user ftp \
  --run-as-group ftp

# Virtual host with transfer limits
sudo simple-ftpd virtual add \
  --hostname limited.example.com \
  --root /var/ftp/limited \
  --max-file-size 50MB \
  --max-transfer-rate 512KB \
  --allow-overwrite false \
  --allow-resume true
```

### Managing Virtual Hosts

#### Listing Virtual Hosts

```bash
# List all virtual hosts
sudo simple-ftpd virtual list

# List with details
sudo simple-ftpd virtual list --verbose

# List in specific format
sudo simple-ftpd virtual list --format json
sudo simple-ftpd virtual list --format table

# Filter virtual hosts
sudo simple-ftpd virtual list --enabled-only
sudo simple-ftpd virtual list --ssl-only
```

#### Virtual Host Information

```bash
# Show virtual host details
sudo simple-ftpd virtual show --hostname ftp.example.com

# Show virtual host statistics
sudo simple-ftpd virtual stats --hostname ftp.example.com

# Show virtual host configuration
sudo simple-ftpd virtual config --hostname ftp.example.com
```

#### Modifying Virtual Hosts

```bash
# Update welcome message
sudo simple-ftpd virtual modify \
  --hostname ftp.example.com \
  --welcome "Updated Welcome Message"

# Update document root
sudo simple-ftpd virtual modify \
  --hostname ftp.example.com \
  --root /var/ftp/newroot

# Update SSL settings
sudo simple-ftpd virtual modify \
  --hostname ftp.example.com \
  --certificate /path/to/new/cert.crt \
  --private-key /path/to/new/key.key
```

#### Virtual Host Status

```bash
# Enable virtual host
sudo simple-ftpd virtual enable --hostname ftp.example.com

# Disable virtual host
sudo simple-ftpd virtual disable --hostname ftp.example.com

# Set as default
sudo simple-ftpd virtual default --hostname ftp.example.com
```

#### Removing Virtual Hosts

```bash
# Remove virtual host (keeps files)
sudo simple-ftpd virtual remove --hostname ftp.example.com

# Remove virtual host and files
sudo simple-ftpd virtual remove --hostname ftp.example.com --remove-files

# Force remove
sudo simple-ftpd virtual remove --hostname ftp.example.com --force
```

## 🔐 SSL Certificate Management

### Generating Certificates

#### Self-Signed Certificates

```bash
# Generate basic self-signed certificate
sudo simple-ftpd ssl generate \
  --hostname ftp.example.com

# Generate with custom details
sudo simple-ftpd ssl generate \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city "San Francisco" \
  --organization "Example Corp" \
  --email admin@example.com \
  --validity 365

# Generate for multiple hostnames
sudo simple-ftpd ssl generate \
  --hostname ftp.example.com \
  --alt-names "ftp2.example.com,ftp3.example.com" \
  --country US \
  --state California
```

#### Certificate Signing Requests (CSR)

```bash
# Generate CSR for CA signing
sudo simple-ftpd ssl csr \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city "San Francisco" \
  --organization "Example Corp" \
  --email admin@example.com \
  --output /tmp/ftp.example.com.csr
```

### Installing Certificates

#### Installing Existing Certificates

```bash
# Install certificate and private key
sudo simple-ftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key

# Install with CA certificate
sudo simple-ftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key \
  --ca-certificate /path/to/ca.crt

# Install for specific virtual host
sudo simple-ftpd ssl install \
  --hostname ftp.example.com \
  --virtual-host secure.example.com \
  --certificate /path/to/secure.example.com.crt \
  --private-key /path/to/secure.example.com.key
```

#### Certificate Updates

```bash
# Update existing certificate
sudo simple-ftpd ssl update \
  --hostname ftp.example.com \
  --certificate /path/to/new/certificate.crt \
  --private-key /path/to/new/private.key

# Update CA certificate
sudo simple-ftpd ssl update \
  --hostname ftp.example.com \
  --ca-certificate /path/to/new/ca.crt
```

### Certificate Management

#### Certificate Information

```bash
# Show certificate details
sudo simple-ftpd ssl show --hostname ftp.example.com

# Show certificate validity
sudo simple-ftpd ssl validity --hostname ftp.example.com

# Show certificate chain
sudo simple-ftpd ssl chain --hostname ftp.example.com

# Show certificate fingerprint
sudo simple-ftpd ssl fingerprint --hostname ftp.example.com
```

#### Certificate Renewal

```bash
# Check certificate expiration
sudo simple-ftpd ssl check --hostname ftp.example.com

# Renew self-signed certificate
sudo simple-ftpd ssl renew \
  --hostname ftp.example.com \
  --validity 365

# Renew with new details
sudo simple-ftpd ssl renew \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city "San Francisco" \
  --organization "Example Corp" \
  --email admin@example.com \
  --validity 730
```

#### Certificate Removal

```bash
# Remove certificate
sudo simple-ftpd ssl remove --hostname ftp.example.com

# Remove from specific virtual host
sudo simple-ftpd ssl remove \
  --hostname ftp.example.com \
  --virtual-host secure.example.com

# Force remove
sudo simple-ftpd ssl remove --hostname ftp.example.com --force
```

## 📊 Monitoring and Statistics

### Server Statistics

#### Basic Statistics

```bash
# Show server statistics
simple-ftpd stats

# Show statistics in specific format
simple-ftpd stats --format json
simple-ftpd stats --format table

# Show specific statistics
simple-ftpd stats --component connections
simple-ftpd stats --component transfers
simple-ftpd stats --component users
```

#### Performance Metrics

```bash
# Show performance metrics
simple-ftpd performance

# Show memory usage
simple-ftpd memory

# Show CPU usage
simple-ftpd cpu

# Show network statistics
simple-ftpd network
```

### Connection Monitoring

#### Active Connections

```bash
# Show active connections
simple-ftpd connections

# Show connection details
simple-ftpd connections --verbose

# Filter connections
simple-ftpd connections --user john
simple-ftpd connections --virtual-host ftp.example.com
simple-ftpd connections --ip 192.168.1.100

# Show connection history
simple-ftpd connections --history
```

#### Connection Management

```bash
# Disconnect specific connection
sudo simple-ftpd disconnect --connection-id 12345

# Disconnect user connections
sudo simple-ftpd disconnect --user john

# Disconnect IP connections
sudo simple-ftpd disconnect --ip 192.168.1.100

# Force disconnect
sudo simple-ftpd disconnect --connection-id 12345 --force
```

### Log Management

#### Viewing Logs

```bash
# View main log
tail -f /var/log/simple-ftpd/simple-ftpd.log

# View access log
tail -f /var/log/simple-ftpd/access.log

# View error log
tail -f /var/log/simple-ftpd/error.log

# View logs with simple-ftpd command
simple-ftpd logs --follow
simple-ftpd logs --lines 100
simple-ftpd logs --level ERROR
```

#### Log Analysis

```bash
# Search logs
simple-ftpd logs --search "connection"
simple-ftpd logs --search "error"
simple-ftpd logs --search "user john"

# Filter by time
simple-ftpd logs --since "1 hour ago"
simple-ftpd logs --since "2024-01-15 10:00:00"
simple-ftpd logs --until "2024-01-15 11:00:00"

# Filter by component
simple-ftpd logs --component ssl
simple-ftpd logs --component authentication
simple-ftpd logs --component transfer
```

#### Log Management

```bash
# Rotate logs
sudo simple-ftpd logrotate

# Clear logs
sudo simple-ftpd logclear

# Archive logs
sudo simple-ftpd logarchive --output /backup/logs-$(date +%Y%m%d).tar.gz

# Set log level
sudo simple-ftpd loglevel --level DEBUG
sudo simple-ftpd loglevel --level INFO
```

## 🔧 Maintenance and Troubleshooting

### Configuration Management

#### Configuration Validation

```bash
# Test configuration
simple-ftpd --test-config --config /etc/simple-ftpd/simple-ftpd.conf

# Validate configuration
simple-ftpd --validate --config /etc/simple-ftpd/simple-ftpd.conf

# Check specific sections
simple-ftpd config --check ssl
simple-ftpd config --check users
simple-ftpd config --check virtual-hosts
```

#### Configuration Backup

```bash
# Backup configuration
sudo simple-ftpd config --backup --output /backup/config-$(date +%Y%m%d).tar.gz

# Restore configuration
sudo simple-ftpd config --restore --input /backup/config-20240115.tar.gz

# Export configuration
sudo simple-ftpd config --export --format json --output config.json
sudo simple-ftpd config --export --format ini --output config.ini
```

### System Maintenance

#### Database Maintenance

```bash
# Optimize user database
sudo simple-ftpd db --optimize

# Backup user database
sudo simple-ftpd db --backup --output /backup/users-$(date +%Y%m%d).db

# Restore user database
sudo simple-ftpd db --restore --input /backup/users-20240115.db

# Check database integrity
sudo simple-ftpd db --check
```

#### File System Maintenance

```bash
# Check file permissions
sudo simple-ftpd fs --check-permissions

# Fix file permissions
sudo simple-ftpd fs --fix-permissions

# Clean temporary files
sudo simple-ftpd fs --clean-temp

# Check disk usage
sudo simple-ftpd fs --disk-usage
```

### Troubleshooting

#### Common Issues

```bash
# Check server health
simple-ftpd health

# Diagnose problems
simple-ftpd diagnose

# Show system information
simple-ftpd system

# Check dependencies
simple-ftpd deps
```

#### Debug Mode

```bash
# Enable debug mode
sudo simple-ftpd debug --enable

# Set debug level
sudo simple-ftpd debug --level TRACE

# Show debug information
simple-ftpd debug --info

# Disable debug mode
sudo simple-ftpd debug --disable
```

## 📚 Advanced Operations

### Batch Operations

#### Batch User Management

```bash
# Add multiple users from file
sudo simple-ftpd user batch-add --file users.txt

# Update multiple users
sudo simple-ftpd user batch-update --file updates.txt

# Remove multiple users
sudo simple-ftpd user batch-remove --file users-to-remove.txt
```

#### Batch Virtual Host Management

```bash
# Add multiple virtual hosts
sudo simple-ftpd virtual batch-add --file virtual-hosts.txt

# Update multiple virtual hosts
sudo simple-ftpd virtual batch-update --file updates.txt
```

### Automation

#### Scheduled Tasks

```bash
# Set up automatic certificate renewal
sudo simple-ftpd ssl --auto-renew --schedule "0 2 * * 0"

# Set up automatic log rotation
sudo simple-ftpd logrotate --auto --schedule "0 1 * * *"

# Set up automatic backup
sudo simple-ftpd backup --auto --schedule "0 3 * * 0"
```

#### Integration

```bash
# Export metrics for monitoring
simple-ftpd metrics --export --format prometheus

# Export logs for log aggregation
simple-ftpd logs --export --format syslog

# Export configuration for automation
simple-ftpd config --export --format ansible
```

## 🚨 Emergency Procedures

### Emergency Stop

```bash
# Emergency stop server
sudo simple-ftpd stop --emergency

# Kill all processes
sudo pkill -f simple-ftpd

# Force kill
sudo pkill -9 -f simple-ftpd
```

### Emergency Recovery

```bash
# Start in safe mode
sudo simple-ftpd start --safe-mode --config /etc/simple-ftpd/safe.conf

# Reset to default configuration
sudo simple-ftpd config --reset

# Emergency user access
sudo simple-ftpd user emergency --username admin --password emergency123
```

## 📞 Getting Help

### Command Help

```bash
# General help
simple-ftpd --help

# Command-specific help
simple-ftpd user --help
simple-ftpd virtual --help
simple-ftpd ssl --help

# Show all commands
simple-ftpd --commands
```

### Documentation

- **Configuration Guide**: [Configuration Documentation](../configuration/README.md)
- **API Reference**: [API Documentation](../api/README.md)
- **Examples**: [Configuration Examples](../examples/README.md)
- **Development Guide**: [Development Documentation](../development/README.md)

### Support Channels

- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions
- **Documentation**: Browse comprehensive guides
- **Examples**: Check working configurations

---

**Next Steps**: Now that you understand user operations, explore:
- [Configuration Guide](../configuration/README.md) for detailed settings
- [Examples](../examples/README.md) for practical configurations
- [Development Guide](../development/README.md) for contributing
