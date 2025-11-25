# Docker Deployment Guide

This guide covers deploying simple-sftpd using Docker containers for development, testing, and production environments.

## Overview

The Docker integration for simple-sftpd provides:

- **Multi-stage builds** for different Linux distributions (Ubuntu, CentOS, Alpine)
- **Multi-architecture support** (x86_64, ARM64, ARMv7)
- **Development environment** with debugging tools
- **Production-ready runtime** with minimal footprint
- **Health checks** and monitoring capabilities
- **Volume mounts** for configuration, logs, and FTP data
- **SSL/TLS support** for secure file transfers

## Quick Start

### Prerequisites

- Docker Engine 20.10+
- Docker Compose 2.0+
- Git

### 1. Clone and Setup

```bash
git clone <repository-url>
cd simple-sftpd

# Create configuration directory
mkdir -p deployment/examples/docker/config
mkdir -p deployment/examples/docker/logs
mkdir -p deployment/examples/docker/data

# Copy example configuration
cp config/examples/simple/simple-sftpd.conf.example deployment/examples/docker/config/simple-sftpd.conf
```

### 2. Deploy with Docker Compose

```bash
# Navigate to Docker examples
cd deployment/examples/docker

# Start the service
docker-compose up -d

# Check status
docker-compose ps

# View logs
docker-compose logs -f simple-sftpd
```

### 3. Test the Service

```bash
# Test FTP service connectivity
nc -z localhost 21

# Test FTPS service connectivity (if SSL enabled)
nc -z localhost 990

# Check health status
docker inspect --format='{{.State.Health.Status}}' simple-sftpd
```

## Build Options

### Using Docker Compose Profiles

The project supports multiple build profiles:

```bash
# Development environment
docker-compose --profile dev up -d

# Runtime environment
docker-compose --profile runtime up -d

# Build for specific distributions
docker-compose --profile build build build-ubuntu
docker-compose --profile build build build-centos
docker-compose --profile build build build-alpine
```

### Using Build Scripts

```bash
# Build for all distributions
./scripts/build-docker.sh -d all

# Build for specific distribution
./scripts/build-docker.sh -d ubuntu -a x86_64

# Clean build
./scripts/build-docker.sh --clean
```

### Using Deployment Script

```bash
# Deploy runtime environment
./scripts/deploy-docker.sh -p runtime

# Deploy development environment
./scripts/deploy-docker.sh -p dev

# Deploy with custom directories
./scripts/deploy-docker.sh -p runtime -c ./config -l ./logs -d ./data
```

## Configuration

### Environment Variables

The following environment variables can be configured:

| Variable | Description | Default |
|----------|-------------|---------|
| `FTP_ROOT_DIR` | FTP root directory | `/var/ftp` |
| `FTP_CONFIG_FILE` | Configuration file path | `/etc/simple-sftpd/simple-sftpd.conf` |
| `LOG_LEVEL` | Log level (DEBUG, INFO, WARN, ERROR) | `INFO` |

### Volume Mounts

| Host Path | Container Path | Description |
|-----------|----------------|-------------|
| `./config` | `/etc/simple-sftpd` | Configuration files (read-only) |
| `./logs` | `/var/log/simple-sftpd` | Log files |
| `./data` | `/var/ftp` | FTP data directory |

### Port Configuration

| Port | Protocol | Description |
|------|----------|-------------|
| 21 | TCP | FTP control port |
| 990 | TCP | FTPS control port (SSL/TLS) |
| 1024-65535 | TCP | Passive mode data ports |

## Production Deployment

### 1. Security Configuration

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    security_opt:
      - no-new-privileges:true
    read_only: true
    tmpfs:
      - /tmp
      - /var/tmp
```

### 2. Resource Limits

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    deploy:
      resources:
        limits:
          memory: 512M
          cpus: '1.0'
        reservations:
          memory: 256M
          cpus: '0.5'
```

### 3. SSL/TLS Configuration

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    volumes:
      - ./ssl:/etc/ssl/simple-sftpd:ro
    environment:
      - SSL_ENABLED=true
      - SSL_CERT_FILE=/etc/ssl/simple-sftpd/server.crt
      - SSL_KEY_FILE=/etc/ssl/simple-sftpd/server.key
```

### 4. Logging Configuration

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    logging:
      driver: "json-file"
      options:
        max-size: "10m"
        max-file: "3"
```

## Development Environment

### 1. Start Development Container

```bash
# Start development environment
docker-compose --profile dev up -d

# Access the container
docker-compose exec dev bash
```

### 2. Development Tools

The development container includes:

- Build tools (CMake, GCC, etc.)
- Debugging tools (GDB, Valgrind)
- Code analysis tools (cppcheck, clang-format)
- Security scanning tools (bandit, semgrep)

### 3. Live Development

```bash
# Mount source code for live development
docker-compose --profile dev up -d

# Build inside container
docker-compose exec dev bash -c "cd /app && mkdir -p build && cd build && cmake .. && make"
```

## Monitoring and Health Checks

### Health Check Configuration

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    healthcheck:
      test: ["CMD", "nc", "-z", "localhost", "21"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s
```

### Monitoring Commands

```bash
# Check container health
docker inspect --format='{{.State.Health.Status}}' simple-sftpd

# View health check logs
docker inspect --format='{{range .State.Health.Log}}{{.Output}}{{end}}' simple-sftpd

# Monitor resource usage
docker stats simple-sftpd

# View container logs
docker-compose logs -f simple-sftpd
```

## Scaling and Load Balancing

### Multiple Instances

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    deploy:
      replicas: 3
      endpoint_mode: dnsrr
```

### Load Balancer Configuration

```yaml
services:
  haproxy:
    image: haproxy:2.6
    ports:
      - "21:21"
      - "990:990"
    volumes:
      - ./haproxy.cfg:/usr/local/etc/haproxy/haproxy.cfg:ro
    depends_on:
      - simple-sftpd
```

## Backup and Recovery

### Backup Script

```bash
#!/bin/bash
# backup-simple-sftpd.sh

BACKUP_DIR="./backup/$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

# Backup configuration
docker cp simple-sftpd:/etc/simple-sftpd/ "$BACKUP_DIR/"

# Backup FTP data
docker cp simple-sftpd:/var/ftp/ "$BACKUP_DIR/"

# Backup logs
docker cp simple-sftpd:/var/log/simple-sftpd/ "$BACKUP_DIR/"

echo "Backup completed: $BACKUP_DIR"
```

### Restore Script

```bash
#!/bin/bash
# restore-simple-sftpd.sh

BACKUP_DIR="$1"
if [ -z "$BACKUP_DIR" ]; then
    echo "Usage: $0 <backup_directory>"
    exit 1
fi

# Stop service
docker-compose down

# Restore configuration
docker cp "$BACKUP_DIR/simple-sftpd/" simple-sftpd:/etc/

# Restore FTP data
docker cp "$BACKUP_DIR/ftp/" simple-sftpd:/var/

# Restore logs
docker cp "$BACKUP_DIR/simple-sftpd/" simple-sftpd:/var/log/

# Start service
docker-compose up -d

echo "Restore completed from: $BACKUP_DIR"
```

## Troubleshooting

### Common Issues

1. **Port Conflicts**
   ```bash
   # Check port usage
   sudo lsof -i :21
   sudo lsof -i :990

   # Stop conflicting services
   sudo systemctl stop vsftpd
   ```

2. **Permission Issues**
   ```bash
   # Fix directory permissions
   sudo chown -R 1000:1000 deployment/examples/docker/
   ```

3. **SSL Certificate Issues**
   ```bash
   # Generate self-signed certificate
   openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes
   ```

### Debug Mode

```bash
# Run with debug logging
docker-compose run --rm simple-sftpd simple-sftpd --verbose --foreground

# Access container shell
docker-compose exec simple-sftpd bash

# Check configuration
docker-compose exec simple-sftpd simple-sftpd --test-config
```

## Performance Tuning

### Container Optimization

```yaml
services:
  simple-sftpd:
    # ... existing configuration ...
    ulimits:
      nofile:
        soft: 65536
        hard: 65536
    sysctls:
      - net.core.rmem_max=26214400
      - net.core.wmem_max=26214400
```

### Host System Tuning

```bash
# Increase TCP buffer sizes
echo 'net.core.rmem_max=26214400' >> /etc/sysctl.conf
echo 'net.core.wmem_max=26214400' >> /etc/sysctl.conf
sysctl -p

# Increase file descriptor limits
echo '* soft nofile 65536' >> /etc/security/limits.conf
echo '* hard nofile 65536' >> /etc/security/limits.conf
```

## Security Best Practices

1. **Run as non-root user** (already configured)
2. **Use read-only configuration mounts**
3. **Enable SSL/TLS for secure transfers**
4. **Implement proper firewall rules**
5. **Regular security updates**
6. **Monitor access logs**
7. **Use strong authentication**

## Integration with CI/CD

### GitHub Actions Example

```yaml
name: Build and Deploy
on:
  push:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build Docker image
        run: ./scripts/build-docker.sh -d ubuntu
      - name: Deploy to staging
        run: ./scripts/deploy-docker.sh -p runtime
```

## Support and Documentation

- **Configuration Guide**: [Configuration Documentation](../configuration/README.md)
- **API Reference**: [API Documentation](../reference/README.md)
- **Troubleshooting**: [Troubleshooting Guide](../troubleshooting/README.md)
- **Community Support**: [GitHub Issues](https://github.com/your-org/simple-sftpd/issues)
