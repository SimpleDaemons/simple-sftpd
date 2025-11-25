# Deployment Configuration

This directory contains the deployment configuration structure for simple-sftpd, inspired by Apache2 and Pure-FTPD configuration management.

## 📁 Directory Structure

```
deployment/
├── README.md                    # This file
├── sites-available/             # Available virtual host configurations
├── sites-enabled/               # Enabled virtual host configurations (symlinks)
├── modules-available/           # Available module configurations
├── modules-enabled/             # Enabled module configurations (symlinks)
├── conf.d/                      # Additional configuration files
├── simple-sftpd.conf                  # Main server configuration
├── modules.conf                 # Module loading configuration
└── sites.conf                   # Virtual host configuration
```

## 🔧 Configuration Management

### Virtual Hosts (Apache2 Style)

Virtual hosts are managed using the `sites-available` and `sites-enabled` directories:

```bash
# Enable a virtual host
sudo ln -s /etc/simple-sftpd/sites-available/example.com.conf /etc/simple-sftpd/sites-enabled/

# Disable a virtual host
sudo rm /etc/simple-sftpd/sites-enabled/example.com.conf

# List enabled sites
ls -la /etc/simple-sftpd/sites-enabled/

# List available sites
ls -la /etc/simple-sftpd/sites-available/
```

### Modules (Pure-FTPD Style)

Modules are managed using the `modules-available` and `modules-enabled` directories:

```bash
# Enable a module
sudo ln -s /etc/simple-sftpd/modules-available/ssl.conf /etc/simple-sftpd/modules-enabled/

# Disable a module
sudo rm /etc/simple-sftpd/modules-enabled/ssl.conf

# List enabled modules
ls -la /etc/simple-sftpd/modules-enabled/

# List available modules
ls -la /etc/simple-sftpd/modules-available/
```

## 📝 Configuration Files

### Main Configuration (`simple-sftpd.conf`)

The main configuration file that includes all enabled configurations:

```json
{
  "server": {
    "name": "Simple-Secure FTP Daemon",
    "version": "0.1.0",
    "listen_address": "0.0.0.0",
    "listen_port": 21
  },
  "includes": [
    "conf.d/*.conf",
    "modules-enabled/*.conf",
    "sites-enabled/*.conf"
  ]
}
```

### Module Configuration (`modules.conf`)

Controls which modules are loaded and their order:

```json
{
  "modules": {
    "ssl": {
      "enabled": true,
      "priority": 10,
      "config_file": "modules-enabled/ssl.conf"
    },
    "auth": {
      "enabled": true,
      "priority": 20,
      "config_file": "modules-enabled/auth.conf"
    },
    "rate_limit": {
      "enabled": true,
      "priority": 30,
      "config_file": "modules-enabled/rate_limit.conf"
    }
  }
}
```

### Site Configuration (`sites.conf`)

Manages virtual host configurations:

```json
{
  "sites": {
    "default": {
      "enabled": true,
      "config_file": "sites-enabled/default.conf"
    },
    "example.com": {
      "enabled": true,
      "config_file": "sites-enabled/example.com.conf"
    },
    "ftp.example.com": {
      "enabled": false,
      "config_file": "sites-available/ftp.example.com.conf"
    }
  }
}
```

## 🚀 Quick Start

### 1. Enable Default Configuration

```bash
# Enable default site
sudo ln -s /etc/simple-sftpd/sites-available/default.conf /etc/simple-sftpd/sites-enabled/

# Enable essential modules
sudo ln -s /etc/simple-sftpd/modules-available/ssl.conf /etc/simple-sftpd/modules-enabled/
sudo ln -s /etc/simple-sftpd/modules-available/auth.conf /etc/simple-sftpd/modules-enabled/

# Reload configuration
sudo simple-sftpd reload
```

### 2. Add New Virtual Host

```bash
# Create configuration
sudo cp /etc/simple-sftpd/sites-available/example.com.conf /etc/simple-sftpd/sites-available/mysite.com.conf

# Edit configuration
sudo nano /etc/simple-sftpd/sites-available/mysite.com.conf

# Enable site
sudo ln -s /etc/simple-sftpd/sites-available/mysite.com.conf /etc/simple-sftpd/sites-enabled/

# Reload configuration
sudo simple-sftpd reload
```

### 3. Enable/Disable Features

```bash
# Enable SSL
sudo ln -s /etc/simple-sftpd/modules-available/ssl.conf /etc/simple-sftpd/modules-enabled/

# Enable rate limiting
sudo ln -s /etc/simple-sftpd/modules-available/rate_limit.conf /etc/simple-sftpd/modules-enabled/

# Disable module
sudo rm /etc/simple-sftpd/modules-enabled/rate_limit.conf

# Reload configuration
sudo simple-sftpd reload
```

## 🔍 Configuration Validation

### Test Configuration

```bash
# Test main configuration
simple-sftpd --test-config --config /etc/simple-sftpd/deployment/simple-sftpd.conf

# Test specific module
simple-sftpd --test-module --module ssl --config /etc/simple-sftpd/deployment/modules-enabled/ssl.conf

# Test specific site
simple-sftpd --test-site --site example.com --config /etc/simple-sftpd/deployment/sites-enabled/example.com.conf
```

### Validate Symlinks

```bash
# Check for broken symlinks
find /etc/simple-sftpd/sites-enabled/ -type l -exec test ! -e {} \; -print
find /etc/simple-sftpd/modules-enabled/ -type l -exec test ! -e {} \; -print

# Verify symlink targets
ls -la /etc/simple-sftpd/sites-enabled/
ls -la /etc/simple-sftpd/modules-enabled/
```

## 📚 Best Practices

### 1. Configuration Organization

- Keep configurations modular and focused
- Use descriptive filenames
- Document custom configurations
- Version control your configurations

### 2. Security

- Restrict access to configuration files
- Use proper file permissions (600 for configs)
- Validate configurations before enabling
- Test in staging environment first

### 3. Maintenance

- Regular configuration backups
- Monitor for configuration errors
- Document changes and reasons
- Use configuration management tools

## 🛠️ Management Scripts

### Enable/Disable Sites

```bash
# Enable site
sudo simple-sftpd-site enable example.com

# Disable site
sudo simple-sftpd-site disable example.com

# List sites
simple-sftpd-site list

# Show site status
simple-sftpd-site status example.com
```

### Enable/Disable Modules

```bash
# Enable module
sudo simple-sftpd-module enable ssl

# Disable module
sudo simple-sftpd-module disable ssl

# List modules
simple-sftpd-module list

# Show module status
simple-sftpd-module status ssl
```

## 🔄 Migration from Traditional Config

### From Single Config File

```bash
# Backup existing configuration
sudo cp /etc/simple-sftpd/simple-sftpd.conf /etc/simple-sftpd/simple-sftpd.conf.backup

# Extract modules to separate files
sudo simple-sftpd-config extract-modules /etc/simple-sftpd/simple-sftpd.conf

# Extract virtual hosts to separate files
sudo simple-sftpd-config extract-sites /etc/simple-sftpd/simple-sftpd.conf

# Enable extracted configurations
sudo simple-sftpd-config enable-all
```

### From Multiple Config Files

```bash
# Import existing configurations
sudo simple-sftpd-config import /path/to/existing/configs/

# Validate imported configurations
sudo simple-sftpd-config validate

# Enable imported configurations
sudo simple-sftpd-config enable-imported
```

## 📞 Support

For help with deployment configuration:

- Check configuration examples in each directory
- Review the main documentation
- Open an issue on GitHub
- Join community discussions

---

**Next Steps**: Explore the configuration examples in each directory to understand how to structure your deployments.
