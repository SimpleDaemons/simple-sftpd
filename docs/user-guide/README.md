# User Guide

This guide covers all user operations and management tasks for Simple-Secure FTP Daemon, from basic server management to advanced user administration.

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
sudo ssftpd start --config /etc/ssftpd/ssftpd.conf

# Start as daemon
sudo ssftpd --daemon start --config /etc/ssftpd/ssftpd.conf

# Start with specific configuration
sudo ssftpd start --config /path/to/custom.conf
```

#### Stopping the Server

```bash
# Graceful shutdown
sudo ssftpd stop

# Force stop (if graceful fails)
sudo ssftpd stop --force

# Stop specific instance
sudo ssftpd stop --pid-file /var/run/ssftpd.pid
```

#### Restarting the Server

```bash
# Restart with current configuration
sudo ssftpd restart

# Restart with new configuration
sudo ssftpd restart --config /path/to/new.conf

# Reload configuration without restart
sudo ssftpd reload
```

#### Server Status

```bash
# Check server status
ssftpd status

# Show detailed status
ssftpd status --verbose

# Show status in JSON format
ssftpd status --format json

# Check specific components
ssftpd status --component ssl
ssftpd status --component users
ssftpd status --component virtual-hosts
```

### Service Management

#### Linux (systemd)

```bash
# Enable service at boot
sudo systemctl enable ssftpd

# Start service
sudo systemctl start ssftpd

# Stop service
sudo systemctl stop ssftpd

# Restart service
sudo systemctl restart ssftpd

# Check status
sudo systemctl status ssftpd

# View logs
sudo journalctl -u ssftpd -f
sudo journalctl -u ssftpd --since "1 hour ago"
```

#### macOS (launchd)

```bash
# Load service
sudo launchctl load /Library/LaunchDaemons/com.blburns.ssftpd.plist

# Start service
sudo launchctl start com.blburns.ssftpd

# Stop service
sudo launchctl stop com.blburns.ssftpd

# Unload service
sudo launchctl unload /Library/LaunchDaemons/com.blburns.ssftpd.plist

# Check status
sudo launchctl list | grep ssftpd
```

#### Windows

```cmd
# Install service
sc create ssftpd binPath= "C:\Program Files\ssftpd\bin\ssftpd.exe"

# Start service
sc start ssftpd

# Stop service
sc stop ssftpd

# Delete service
sc delete ssftpd

# Check status
sc query ssftpd
```

## 👤 User Management

### Adding Users

#### Basic User Creation

```bash
# Create a regular user
sudo ssftpd user add \
  --username john \
  --password secret123 \
  --home /var/ftp/john \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD

# Create user with specific group
sudo ssftpd user add \
  --username developer \
  --password devpass123 \
  --home /var/ftp/developer \
  --group developers \
  --permissions READ,WRITE,DELETE,LIST,UPLOAD,DOWNLOAD,ADMIN
```

#### Anonymous User

```bash
# Create anonymous user for public access
sudo ssftpd user add \
  --username anonymous \
  --home /var/ftp/public \
  --anonymous \
  --permissions READ,LIST,DOWNLOAD

# Create guest user with restrictions
sudo ssftpd user add \
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
sudo ssftpd user add \
  --username limited \
  --password limpass123 \
  --home /var/ftp/limited \
  --permissions READ,LIST,DOWNLOAD \
  --max-connections 2 \
  --max-transfer-rate 512KB \
  --max-file-size 25MB \
  --session-timeout 1800

# User with path restrictions
sudo ssftpd user add \
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
sudo ssftpd user password \
  --username john \
  --password newpassword123

# Change password interactively
sudo ssftpd user password --username john
# Will prompt for new password
```

#### Permission Updates

```bash
# Grant additional permissions
sudo ssftpd user grant \
  --username john \
  --permissions DELETE,RENAME

# Revoke permissions
sudo ssftpd user revoke \
  --username john \
  --permissions DELETE

# Set complete permission set
sudo ssftpd user permissions \
  --username john \
  --permissions READ,WRITE,LIST,UPLOAD,DOWNLOAD
```

#### User Properties

```bash
# Update home directory
sudo ssftpd user modify \
  --username john \
  --home /var/ftp/newhome

# Update connection limits
sudo ssftpd user modify \
  --username john \
  --max-connections 5 \
  --max-transfer-rate 1MB

# Update session timeout
sudo ssftpd user modify \
  --username john \
  --session-timeout 3600
```

### User Information

#### Listing Users

```bash
# List all users
sudo ssftpd user list

# List users with details
sudo ssftpd user list --verbose

# List users in specific format
sudo ssftpd user list --format json
sudo ssftpd user list --format table

# Filter users
sudo ssftpd user list --enabled-only
sudo ssftpd user list --anonymous-only
sudo ssftpd user list --guest-only
```

#### User Details

```bash
# Show user information
sudo ssftpd user show --username john

# Show user statistics
sudo ssftpd user stats --username john

# Show user permissions
sudo ssftpd user permissions --username john

# Show user connections
sudo ssftpd user connections --username john
```

#### User Status

```bash
# Enable user
sudo ssftpd user enable --username john

# Disable user
sudo ssftpd user disable --username john

# Lock user account
sudo ssftpd user lock --username john

# Unlock user account
sudo ssftpd user unlock --username john
```

### Removing Users

```bash
# Remove user (keeps files)
sudo ssftpd user remove --username john

# Remove user and files
sudo ssftpd user remove --username john --remove-files

# Remove user with confirmation
sudo ssftpd user remove --username john --confirm

# Force remove (if user is connected)
sudo ssftpd user remove --username john --force
```

## 🏠 Virtual Host Management

### Adding Virtual Hosts

#### Basic Virtual Host

```bash
# Create default virtual host
sudo ssftpd virtual add \
  --hostname default \
  --root /var/ftp \
  --welcome "Welcome to Default FTP Server" \
  --banner "Default FTP Server Ready"

# Create domain-specific virtual host
sudo ssftpd virtual add \
  --hostname ftp.example.com \
  --root /var/ftp/example \
  --welcome "Welcome to Example.com FTP Server" \
  --banner "Example.com FTP Server Ready"
```

#### Virtual Host with SSL

```bash
# Create SSL-enabled virtual host
sudo ssftpd virtual add \
  --hostname secure.example.com \
  --root /var/ftp/secure \
  --ssl \
  --certificate /etc/ssl/certs/secure.example.com.crt \
  --private-key /etc/ssl/private/secure.example.com.key \
  --require-ssl

# Create virtual host with custom SSL settings
sudo ssftpd virtual add \
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
sudo ssftpd virtual add \
  --hostname restricted.example.com \
  --root /var/ftp/restricted \
  --chroot \
  --chroot-directory /var/ftp/restricted \
  --drop-privileges \
  --run-as-user ftp \
  --run-as-group ftp

# Virtual host with transfer limits
sudo ssftpd virtual add \
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
sudo ssftpd virtual list

# List with details
sudo ssftpd virtual list --verbose

# List in specific format
sudo ssftpd virtual list --format json
sudo ssftpd virtual list --format table

# Filter virtual hosts
sudo ssftpd virtual list --enabled-only
sudo ssftpd virtual list --ssl-only
```

#### Virtual Host Information

```bash
# Show virtual host details
sudo ssftpd virtual show --hostname ftp.example.com

# Show virtual host statistics
sudo ssftpd virtual stats --hostname ftp.example.com

# Show virtual host configuration
sudo ssftpd virtual config --hostname ftp.example.com
```

#### Modifying Virtual Hosts

```bash
# Update welcome message
sudo ssftpd virtual modify \
  --hostname ftp.example.com \
  --welcome "Updated Welcome Message"

# Update document root
sudo ssftpd virtual modify \
  --hostname ftp.example.com \
  --root /var/ftp/newroot

# Update SSL settings
sudo ssftpd virtual modify \
  --hostname ftp.example.com \
  --certificate /path/to/new/cert.crt \
  --private-key /path/to/new/key.key
```

#### Virtual Host Status

```bash
# Enable virtual host
sudo ssftpd virtual enable --hostname ftp.example.com

# Disable virtual host
sudo ssftpd virtual disable --hostname ftp.example.com

# Set as default
sudo ssftpd virtual default --hostname ftp.example.com
```

#### Removing Virtual Hosts

```bash
# Remove virtual host (keeps files)
sudo ssftpd virtual remove --hostname ftp.example.com

# Remove virtual host and files
sudo ssftpd virtual remove --hostname ftp.example.com --remove-files

# Force remove
sudo ssftpd virtual remove --hostname ftp.example.com --force
```

## 🔐 SSL Certificate Management

### Generating Certificates

#### Self-Signed Certificates

```bash
# Generate basic self-signed certificate
sudo ssftpd ssl generate \
  --hostname ftp.example.com

# Generate with custom details
sudo ssftpd ssl generate \
  --hostname ftp.example.com \
  --country US \
  --state California \
  --city "San Francisco" \
  --organization "Example Corp" \
  --email admin@example.com \
  --validity 365

# Generate for multiple hostnames
sudo ssftpd ssl generate \
  --hostname ftp.example.com \
  --alt-names "ftp2.example.com,ftp3.example.com" \
  --country US \
  --state California
```

#### Certificate Signing Requests (CSR)

```bash
# Generate CSR for CA signing
sudo ssftpd ssl csr \
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
sudo ssftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key

# Install with CA certificate
sudo ssftpd ssl install \
  --hostname ftp.example.com \
  --certificate /path/to/certificate.crt \
  --private-key /path/to/private.key \
  --ca-certificate /path/to/ca.crt

# Install for specific virtual host
sudo ssftpd ssl install \
  --hostname ftp.example.com \
  --virtual-host secure.example.com \
  --certificate /path/to/secure.example.com.crt \
  --private-key /path/to/secure.example.com.key
```

#### Certificate Updates

```bash
# Update existing certificate
sudo ssftpd ssl update \
  --hostname ftp.example.com \
  --certificate /path/to/new/certificate.crt \
  --private-key /path/to/new/private.key

# Update CA certificate
sudo ssftpd ssl update \
  --hostname ftp.example.com \
  --ca-certificate /path/to/new/ca.crt
```

### Certificate Management

#### Certificate Information

```bash
# Show certificate details
sudo ssftpd ssl show --hostname ftp.example.com

# Show certificate validity
sudo ssftpd ssl validity --hostname ftp.example.com

# Show certificate chain
sudo ssftpd ssl chain --hostname ftp.example.com

# Show certificate fingerprint
sudo ssftpd ssl fingerprint --hostname ftp.example.com
```

#### Certificate Renewal

```bash
# Check certificate expiration
sudo ssftpd ssl check --hostname ftp.example.com

# Renew self-signed certificate
sudo ssftpd ssl renew \
  --hostname ftp.example.com \
  --validity 365

# Renew with new details
sudo ssftpd ssl renew \
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
sudo ssftpd ssl remove --hostname ftp.example.com

# Remove from specific virtual host
sudo ssftpd ssl remove \
  --hostname ftp.example.com \
  --virtual-host secure.example.com

# Force remove
sudo ssftpd ssl remove --hostname ftp.example.com --force
```

## 📊 Monitoring and Statistics

### Server Statistics

#### Basic Statistics

```bash
# Show server statistics
ssftpd stats

# Show statistics in specific format
ssftpd stats --format json
ssftpd stats --format table

# Show specific statistics
ssftpd stats --component connections
ssftpd stats --component transfers
ssftpd stats --component users
```

#### Performance Metrics

```bash
# Show performance metrics
ssftpd performance

# Show memory usage
ssftpd memory

# Show CPU usage
ssftpd cpu

# Show network statistics
ssftpd network
```

### Connection Monitoring

#### Active Connections

```bash
# Show active connections
ssftpd connections

# Show connection details
ssftpd connections --verbose

# Filter connections
ssftpd connections --user john
ssftpd connections --virtual-host ftp.example.com
ssftpd connections --ip 192.168.1.100

# Show connection history
ssftpd connections --history
```

#### Connection Management

```bash
# Disconnect specific connection
sudo ssftpd disconnect --connection-id 12345

# Disconnect user connections
sudo ssftpd disconnect --user john

# Disconnect IP connections
sudo ssftpd disconnect --ip 192.168.1.100

# Force disconnect
sudo ssftpd disconnect --connection-id 12345 --force
```

### Log Management

#### Viewing Logs

```bash
# View main log
tail -f /var/log/ssftpd/ssftpd.log

# View access log
tail -f /var/log/ssftpd/access.log

# View error log
tail -f /var/log/ssftpd/error.log

# View logs with ssftpd command
ssftpd logs --follow
ssftpd logs --lines 100
ssftpd logs --level ERROR
```

#### Log Analysis

```bash
# Search logs
ssftpd logs --search "connection"
ssftpd logs --search "error"
ssftpd logs --search "user john"

# Filter by time
ssftpd logs --since "1 hour ago"
ssftpd logs --since "2024-01-15 10:00:00"
ssftpd logs --until "2024-01-15 11:00:00"

# Filter by component
ssftpd logs --component ssl
ssftpd logs --component authentication
ssftpd logs --component transfer
```

#### Log Management

```bash
# Rotate logs
sudo ssftpd logrotate

# Clear logs
sudo ssftpd logclear

# Archive logs
sudo ssftpd logarchive --output /backup/logs-$(date +%Y%m%d).tar.gz

# Set log level
sudo ssftpd loglevel --level DEBUG
sudo ssftpd loglevel --level INFO
```

## 🔧 Maintenance and Troubleshooting

### Configuration Management

#### Configuration Validation

```bash
# Test configuration
ssftpd --test-config --config /etc/ssftpd/ssftpd.conf

# Validate configuration
ssftpd --validate --config /etc/ssftpd/ssftpd.conf

# Check specific sections
ssftpd config --check ssl
ssftpd config --check users
ssftpd config --check virtual-hosts
```

#### Configuration Backup

```bash
# Backup configuration
sudo ssftpd config --backup --output /backup/config-$(date +%Y%m%d).tar.gz

# Restore configuration
sudo ssftpd config --restore --input /backup/config-20240115.tar.gz

# Export configuration
sudo ssftpd config --export --format json --output config.json
sudo ssftpd config --export --format ini --output config.ini
```

### System Maintenance

#### Database Maintenance

```bash
# Optimize user database
sudo ssftpd db --optimize

# Backup user database
sudo ssftpd db --backup --output /backup/users-$(date +%Y%m%d).db

# Restore user database
sudo ssftpd db --restore --input /backup/users-20240115.db

# Check database integrity
sudo ssftpd db --check
```

#### File System Maintenance

```bash
# Check file permissions
sudo ssftpd fs --check-permissions

# Fix file permissions
sudo ssftpd fs --fix-permissions

# Clean temporary files
sudo ssftpd fs --clean-temp

# Check disk usage
sudo ssftpd fs --disk-usage
```

### Troubleshooting

#### Common Issues

```bash
# Check server health
ssftpd health

# Diagnose problems
ssftpd diagnose

# Show system information
ssftpd system

# Check dependencies
ssftpd deps
```

#### Debug Mode

```bash
# Enable debug mode
sudo ssftpd debug --enable

# Set debug level
sudo ssftpd debug --level TRACE

# Show debug information
ssftpd debug --info

# Disable debug mode
sudo ssftpd debug --disable
```

## 📚 Advanced Operations

### Batch Operations

#### Batch User Management

```bash
# Add multiple users from file
sudo ssftpd user batch-add --file users.txt

# Update multiple users
sudo ssftpd user batch-update --file updates.txt

# Remove multiple users
sudo ssftpd user batch-remove --file users-to-remove.txt
```

#### Batch Virtual Host Management

```bash
# Add multiple virtual hosts
sudo ssftpd virtual batch-add --file virtual-hosts.txt

# Update multiple virtual hosts
sudo ssftpd virtual batch-update --file updates.txt
```

### Automation

#### Scheduled Tasks

```bash
# Set up automatic certificate renewal
sudo ssftpd ssl --auto-renew --schedule "0 2 * * 0"

# Set up automatic log rotation
sudo ssftpd logrotate --auto --schedule "0 1 * * *"

# Set up automatic backup
sudo ssftpd backup --auto --schedule "0 3 * * 0"
```

#### Integration

```bash
# Export metrics for monitoring
ssftpd metrics --export --format prometheus

# Export logs for log aggregation
ssftpd logs --export --format syslog

# Export configuration for automation
ssftpd config --export --format ansible
```

## 🚨 Emergency Procedures

### Emergency Stop

```bash
# Emergency stop server
sudo ssftpd stop --emergency

# Kill all processes
sudo pkill -f ssftpd

# Force kill
sudo pkill -9 -f ssftpd
```

### Emergency Recovery

```bash
# Start in safe mode
sudo ssftpd start --safe-mode --config /etc/ssftpd/safe.conf

# Reset to default configuration
sudo ssftpd config --reset

# Emergency user access
sudo ssftpd user emergency --username admin --password emergency123
```

## 📞 Getting Help

### Command Help

```bash
# General help
ssftpd --help

# Command-specific help
ssftpd user --help
ssftpd virtual --help
ssftpd ssl --help

# Show all commands
ssftpd --commands
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
