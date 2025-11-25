# Production Configuration Examples

This directory contains hardened configuration examples for production deployments with maximum security and performance.

## Configuration Files

All configurations are available in three formats:
- **INI** (`.conf`) - Traditional format, fully supported
- **JSON** (`.json`) - Machine-readable format
- **YAML** (`.yml`) - Human-readable format

## Features

- High-performance settings (1000 max connections, optimized timeouts)
- **SSL/TLS required** (TLS 1.3 only, client certificate verification)
- JSON logging format (ideal for log aggregation systems)
- Maximum security (chroot, privilege dropping, strict command filtering)
- Aggressive rate limiting (1000 requests/minute)
- File transfer restrictions (5GB max file size, 50MB/s transfer rate)
- Strict file extension filtering

## Usage

```bash
# Copy the INI configuration
sudo cp config/production/simple-sftpd.conf /etc/simple-sftpd/simple-sftpd.conf

# Or use JSON format
sudo cp config/production/simple-sftpd.json /etc/simple-sftpd/simple-sftpd.json

# Or use YAML format
sudo cp config/production/simple-sftpd.yml /etc/simple-sftpd/simple-sftpd.yml
```

## Security Requirements

**Before deploying this configuration:**

1. **SSL Certificates Required:**
   ```bash
   # Use Let's Encrypt or commercial CA
   sudo certbot certonly --standalone -d your-domain.com
   ```

2. **File Permissions:**
   ```bash
   sudo chmod 600 /etc/simple-sftpd/ssl/server.key
   sudo chown root:root /etc/simple-sftpd/ssl/server.key
   ```

3. **Firewall Configuration:**
   ```bash
   # Allow FTP and passive mode ports
   sudo ufw allow 21/tcp
   sudo ufw allow 49152:65535/tcp
   ```

4. **Remove Default Test User:**
   The default `test/test` user should be removed or changed in production.

## Log Format

This configuration uses **JSON logging** format, which is ideal for:
- Log aggregation systems (ELK Stack, Splunk, Datadog)
- Cloud logging services (AWS CloudWatch, Google Cloud Logging)
- Automated log parsing and analysis
- SIEM systems

Example JSON log entry:
```json
{"timestamp":"2024-11-25 16:30:45.123","level":"INFO","message":"FTP Server started on 0.0.0.0:21"}
```

## When to Use

Use this configuration for:
- Production deployments
- High-security environments
- Compliance requirements (HIPAA, SOX, GDPR)
- High-traffic scenarios
- Enterprise deployments
- Public-facing FTP servers

## Important Notes

⚠️ **This configuration requires SSL/TLS certificates** - the server will not start without valid certificates.

⚠️ **Client certificates are required** - ensure all clients have valid certificates.

⚠️ **File overwrite is disabled** - prevents accidental data loss.

⚠️ **Strict file filtering** - only allowed file extensions can be transferred.

