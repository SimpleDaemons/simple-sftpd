# Advanced Configuration Examples

This directory contains enhanced configuration examples with SSL/TLS, advanced security, and performance tuning.

## Configuration Files

All configurations are available in three formats:
- **INI** (`.conf`) - Traditional format, fully supported
- **JSON** (`.json`) - Machine-readable format
- **YAML** (`.yml`) - Human-readable format

## Features

- Enhanced connection settings (port 21, 200 max connections)
- SSL/TLS support (FTPS) with certificate configuration
- Advanced logging (DEBUG level, EXTENDED format with PID)
- Security hardening (chroot, privilege dropping, SSL required)
- Performance tuning (larger buffers, memory-mapped I/O)
- Extended rate limiting (300 requests/minute)
- File transfer limits and restrictions

## Usage

```bash
# Copy the INI configuration
sudo cp config/advanced/simple-sftpd.conf /etc/simple-sftpd/simple-sftpd.conf

# Or use JSON format
sudo cp config/advanced/simple-sftpd.json /etc/simple-sftpd/simple-sftpd.json

# Or use YAML format
sudo cp config/advanced/simple-sftpd.yml /etc/simple-sftpd/simple-sftpd.yml
```

## SSL/TLS Setup

Before using this configuration, you need to set up SSL certificates:

```bash
# Generate self-signed certificate (for testing)
sudo tools/setup-ssl.sh --hostname your-domain.com

# Or use Let's Encrypt (for production)
sudo certbot certonly --standalone -d your-domain.com
```

## When to Use

Use this configuration for:
- Medium to large deployments
- Environments requiring SSL/TLS encryption
- Enhanced security requirements
- Performance-critical applications
- Multi-user environments

For maximum security in production, see the `production/` configuration.

