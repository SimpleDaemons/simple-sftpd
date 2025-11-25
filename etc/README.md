# System Integration Configuration

This directory contains system-level configuration files for integrating simple-sftpd with various operating systems and services.

## 📁 Directory Structure

```
etc/
├── README.md                    # This file
├── systemd/                     # systemd service files (Linux)
│   ├── simple-sftpd.service          # Main service file
│   ├── simple-sftpd@.service         # Template for multiple instances
│   └── simple-sftpd.target           # Target for managing instances
├── launchd/                     # macOS LaunchDaemon files
│   └── com.simple-sftpd.simple-sftpd.plist # macOS service configuration
├── init.d/                      # SysV init scripts (legacy Linux)
├── logrotate.d/                 # Log rotation configuration
└── rsyslog.d/                   # rsyslog configuration
```

## 🐧 Linux systemd Integration

### Service Management

```bash
# Enable and start the service
sudo systemctl enable simple-sftpd
sudo systemctl start simple-sftpd

# Check service status
sudo systemctl status simple-sftpd

# Reload configuration
sudo systemctl reload simple-sftpd

# Stop the service
sudo systemctl stop simple-sftpd

# View logs
sudo journalctl -u simple-sftpd -f
```

### Multiple Instances

```bash
# Start additional instances
sudo systemctl start simple-sftpd@instance1
sudo systemctl start simple-sftpd@instance2

# Enable instances
sudo systemctl enable simple-sftpd@instance1
sudo systemctl enable simple-sftpd@instance2

# Manage all instances
sudo systemctl start simple-sftpd.target
sudo systemctl stop simple-sftpd.target
```

### Service Configuration

The systemd service files include:

- **Security hardening**: NoNewPrivileges, ProtectSystem, etc.
- **Resource limits**: File descriptors, process limits
- **Directory management**: Runtime, state, logs, configuration
- **Restart policies**: Automatic restart on failure

## 🍎 macOS Integration

### Install LaunchDaemon

```bash
# Copy the plist file
sudo cp etc/launchd/com.simple-sftpd.simple-sftpd.plist /Library/LaunchDaemons/

# Set permissions
sudo chown root:wheel /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist
sudo chmod 644 /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist

# Load the service
sudo launchctl load /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist
```

### Service Management

```bash
# Start the service
sudo launchctl start com.simple-sftpd.simple-sftpd

# Stop the service
sudo launchctl stop com.simple-sftpd.simple-sftpd

# Unload the service
sudo launchctl unload /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist

# Check if running
sudo launchctl list | grep simple-sftpd
```

## 📊 Log Management

### Logrotate Configuration

The logrotate configuration:

- Rotates logs daily
- Keeps 52 weeks of general logs
- Keeps 30 days of access/error/SSL logs
- Compresses old logs
- Reloads simple-sftpd after rotation
- Runs as simple-sftpd user for security

### Manual Log Rotation

```bash
# Test logrotate configuration
sudo logrotate -d /etc/logrotate.d/simple-sftpd

# Force log rotation
sudo logrotate -f /etc/logrotate.d/simple-sftpd

# Check logrotate status
sudo cat /var/lib/logrotate/status | grep simple-sftpd
```

### Log Locations

```
/var/log/simple-sftpd/
├── simple-sftpd.log          # Main application log
├── access.log          # Access log
├── error.log           # Error log
├── ssl.log             # SSL/TLS log
└── ssl-renewal.log     # SSL certificate renewal log
```

## 🔧 Configuration Integration

### Service Configuration

The service files reference the deployment configuration:

```bash
# Main configuration
--config /etc/simple-sftpd/deployment/simple-sftpd.conf

# Instance-specific configuration
--config /etc/simple-sftpd/deployment/simple-sftpd.conf --instance instance1
```

### Directory Structure

```
/usr/local/etc/simple-sftpd/          # macOS
/etc/simple-sftpd/                    # Linux
├── deployment/                 # Main configuration
├── ssl/                       # SSL certificates
└── users/                     # User configurations
```

## 🚀 Quick Setup

### Linux (systemd)

```bash
# 1. Copy service files
sudo cp etc/systemd/*.service /etc/systemd/system/
sudo cp etc/systemd/*.target /etc/systemd/system/

# 2. Reload systemd
sudo systemctl daemon-reload

# 3. Enable and start
sudo systemctl enable simple-sftpd
sudo systemctl start simple-sftpd

# 4. Check status
sudo systemctl status simple-sftpd
```

### macOS (LaunchDaemon)

```bash
# 1. Copy plist file
sudo cp etc/launchd/com.simple-sftpd.simple-sftpd.plist /Library/LaunchDaemons/

# 2. Set permissions
sudo chown root:wheel /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist
sudo chmod 644 /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist

# 3. Load service
sudo launchctl load /Library/LaunchDaemons/com.simple-sftpd.simple-sftpd.plist

# 4. Start service
sudo launchctl start com.simple-sftpd.simple-sftpd
```

### Log Rotation

```bash
# 1. Copy logrotate configuration
sudo cp etc/logrotate.d/simple-sftpd /etc/logrotate.d/

# 2. Test configuration
sudo logrotate -d /etc/logrotate.d/simple-sftpd

# 3. Set up cron job (if not using systemd timer)
sudo crontab -e
# Add: 0 0 * * * /usr/sbin/logrotate /etc/logrotate.d/simple-sftpd
```

## 🔍 Troubleshooting

### Service Won't Start

```bash
# Check service status
sudo systemctl status simple-sftpd

# View detailed logs
sudo journalctl -u simple-sftpd -n 50

# Check configuration
sudo simple-sftpd --test-config --config /etc/simple-sftpd/deployment/simple-sftpd.conf

# Check file permissions
sudo ls -la /etc/simple-sftpd/
sudo ls -la /var/log/simple-sftpd/
```

### Permission Issues

```bash
# Fix directory permissions
sudo chown -R simple-sftpd:simple-sftpd /etc/simple-sftpd/
sudo chown -R simple-sftpd:simple-sftpd /var/log/simple-sftpd/
sudo chown -R simple-sftpd:simple-sftpd /var/lib/simple-sftpd/

# Fix file permissions
sudo chmod 600 /etc/simple-sftpd/ssl/*.key
sudo chmod 644 /etc/simple-sftpd/ssl/*.crt
```

### Port Conflicts

```bash
# Check what's using the ports
sudo netstat -tlnp | grep :21
sudo netstat -tlnp | grep :990

# Stop conflicting services
sudo systemctl stop vsftpd
sudo systemctl stop pure-ftpd
```

## 📚 Additional Resources

### systemd Documentation
- [systemd.service(5)](https://www.freedesktop.org/software/systemd/man/systemd.service.html)
- [systemd.target(5)](https://www.freedesktop.org/software/systemd/man/systemd.target.html)

### macOS Documentation
- [launchd.plist(5)](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html)

### Logrotate Documentation
- [logrotate(8)](https://linux.die.net/man/8/logrotate)

---

**Next Steps**: 
1. Copy the appropriate service files for your system
2. Configure the service to start automatically
3. Set up log rotation
4. Test the service integration
5. Configure firewall rules if needed
