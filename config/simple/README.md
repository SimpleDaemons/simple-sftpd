# Simple Configuration Examples

This directory contains minimal configuration examples for basic FTP server setups.

## Configuration Files

All configurations are available in three formats:
- **INI** (`.conf`) - Traditional format, fully supported
- **JSON** (`.json`) - Machine-readable format
- **YAML** (`.yml`) - Human-readable format

## Features

- Basic connection settings (port 21, 50 max connections)
- Simple logging (INFO level, STANDARD format)
- Basic security (no SSL, no anonymous access)
- Rate limiting enabled (60 requests/minute)

## Usage

```bash
# Copy the INI configuration
sudo cp config/simple/simple-sftpd.conf /etc/simple-sftpd/simple-sftpd.conf

# Or use JSON format
sudo cp config/simple/simple-sftpd.json /etc/simple-sftpd/simple-sftpd.json

# Or use YAML format
sudo cp config/simple/simple-sftpd.yml /etc/simple-sftpd/simple-sftpd.yml
```

## Default Test User

For testing purposes, a default user is automatically created:
- **Username:** `test`
- **Password:** `test`
- **Home Directory:** `/tmp`

**Note:** Change or remove this default user in production environments!

## When to Use

Use this configuration for:
- Development and testing
- Small internal networks
- Low-traffic scenarios
- Learning and experimentation

For production deployments, see the `advanced/` or `production/` configurations.

