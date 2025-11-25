# Packaging Guide

This directory contains packaging configurations for building distribution packages for multiple platforms and package managers.

## 📦 Supported Package Formats

### Linux
- **RPM** - Red Hat, CentOS, Fedora, RHEL, SUSE
- **DEB** - Debian, Ubuntu, Linux Mint
- **Generic** - Source tarballs, manual installation

### Windows
- **NSIS Installer** - Windows executable installer (.exe)
- **MSI** - Windows Installer package (planned)
- **Chocolatey** - Windows package manager (planned)

### macOS
- **Homebrew** - macOS package manager (planned)
- **DMG** - macOS disk image installer (planned)

## 🏗️ Building Packages

### Prerequisites

#### For RPM Packages
```bash
# CentOS/RHEL/Fedora
sudo yum install rpm-build rpmdevtools
# or
sudo dnf install rpm-build rpmdevtools

# Ubuntu/Debian
sudo apt install rpm rpm-build
```

#### For DEB Packages
```bash
# Ubuntu/Debian
sudo apt install build-essential devscripts debhelper dh-make

# CentOS/RHEL/Fedora
sudo yum install rpm-build rpmdevtools
# or
sudo dnf install rpm-build rpmdevtools
```

#### For Windows Installers
```bash
# Install NSIS (Nullsoft Scriptable Install System)
# Download from: https://nsis.sourceforge.io/Download
# or use Chocolatey: choco install nsis

# Install NSSM (Non-Sucking Service Manager)
# Download from: https://nssm.cc/
```

### Building RPM Packages

#### 1. Setup RPM Build Environment
```bash
# Create RPM build structure
rpmdev-setuptree

# Copy source to RPM build directory
cp simple-sftpd-0.1.0.tar.gz ~/rpmbuild/SOURCES/
cp packaging/rpm/simple-sftpd.spec ~/rpmbuild/SPECS/
```

#### 2. Build RPM Package
```bash
# Build the package
rpmbuild -ba ~/rpmbuild/SPECS/simple-sftpd.spec

# The package will be created in ~/rpmbuild/RPMS/
```

#### 3. Install RPM Package
```bash
# Install the package
sudo rpm -ivh ~/rpmbuild/RPMS/x86_64/simple-sftpd-0.1.0-1.el8.x86_64.rpm

# Or upgrade existing installation
sudo rpm -Uvh ~/rpmbuild/RPMS/x86_64/simple-sftpd-0.1.0-1.el8.x86_64.rpm
```

### Building DEB Packages

#### 1. Setup DEB Build Environment
```bash
# Create package directory
mkdir simple-sftpd-0.1.0
cd simple-sftpd-0.1.0

# Copy source files
cp -r ../src ../include ../cmake ../deployment ../etc ../docs ./
cp ../CMakeLists.txt ../LICENSE ../README.md ./

# Copy Debian packaging files
cp -r ../packaging/deb/debian ./
```

#### 2. Build DEB Package
```bash
# Build the package
debuild -b -us -uc

# The package will be created in the parent directory
```

#### 3. Install DEB Package
```bash
# Install the package
sudo dpkg -i ../simple-sftpd_0.1.0_amd64.deb

# Fix dependencies if needed
sudo apt-get install -f
```

### Building Windows Installers

#### 1. Prepare Windows Build
```bash
# Cross-compile for Windows (if building on Linux/macOS)
# Or build natively on Windows

# Ensure all required files are present:
# - simple-sftpd.exe
# - simple-sftpd-site.exe
# - simple-sftpd-module.exe
# - *.dll files
# - deployment/ directory
# - etc/windows/ directory
# - docs/ directory
# - nssm.exe
# - LICENSE file
```

#### 2. Build NSIS Installer
```bash
# Run NSIS compiler
makensis packaging/windows/installer.nsi

# The installer will be created as simple-sftpd-0.1.0-windows-x64.exe
```

#### 3. Install on Windows
- Run the installer as Administrator
- Follow the installation wizard
- The service will be automatically installed and started

## 📁 Package Contents

### RPM Package Structure
```
simple-sftpd-0.1.0-1.el8.x86_64.rpm
├── /usr/sbin/simple-sftpd
├── /usr/sbin/simple-sftpd-site
├── /usr/sbin/simple-sftpd-module
├── /usr/lib64/libsimple-sftpd.so
├── /usr/include/simple-sftpd/
├── /etc/simple-sftpd/
├── /etc/systemd/system/simple-sftpd.service
├── /etc/rc.d/init.d/simple-sftpd
├── /var/ftp/
├── /var/log/simple-sftpd/
└── /var/lib/simple-sftpd/
```

### DEB Package Structure
```
simple-sftpd_0.1.0_amd64.deb
├── /usr/sbin/simple-sftpd
├── /usr/sbin/simple-sftpd-site
├── /usr/sbin/simple-sftpd-module
├── /usr/lib/x86_64-linux-gnu/libsimple-sftpd.so
├── /usr/include/simple-sftpd/
├── /etc/simple-sftpd/
├── /etc/systemd/system/simple-sftpd.service
├── /etc/init.d/simple-sftpd
├── /var/ftp/
├── /var/log/simple-sftpd/
└── /var/lib/simple-sftpd/
```

### Windows Installer Structure
```
simple-sftpd-0.1.0-windows-x64.exe
├── C:\Program Files\simple-sftpd\
│   ├── simple-sftpd.exe
│   ├── simple-sftpd-site.exe
│   ├── simple-sftpd-module.exe
│   ├── *.dll
│   ├── etc\deployment\
│   ├── etc\windows\
│   ├── docs\
│   ├── logs\
│   ├── var\
│   └── ssl\
└── Windows Service: simple-sftpd
```

## 🔧 Package Configuration

### Service Management

#### systemd (Modern Linux)
```bash
# Enable and start service
sudo systemctl enable simple-sftpd
sudo systemctl start simple-sftpd

# Check status
sudo systemctl status simple-sftpd

# Reload configuration
sudo systemctl reload simple-sftpd
```

#### SysV init (Traditional Linux)
```bash
# Start service
sudo /etc/init.d/simple-sftpd start

# Check status
sudo /etc/init.d/simple-sftpd status

# Reload configuration
sudo /etc/init.d/simple-sftpd reload
```

#### Windows Service
```bash
# Start service
net start simple-sftpd

# Stop service
net stop simple-sftpd

# Check status
sc query simple-sftpd
```

### Configuration Management

#### Enable/Disable Sites
```bash
# Enable site
sudo simple-sftpd-site enable example.com

# Disable site
sudo simple-sftpd-site disable example.com

# List sites
simple-sftpd-site list
```

#### Enable/Disable Modules
```bash
# Enable module
sudo simple-sftpd-module enable ssl

# Disable module
sudo simple-sftpd-module disable rate_limit

# List modules
simple-sftpd-module list
```

## 🚀 Quick Package Installation

### Ubuntu/Debian
```bash
# Add repository (when available)
# sudo apt-add-repository ppa:simple-sftpd/stable
# sudo apt update

# Install package
sudo apt install simple-sftpd

# Start service
sudo systemctl start simple-sftpd
sudo systemctl enable simple-sftpd
```

### CentOS/RHEL/Fedora
```bash
# Install package
sudo yum install simple-sftpd
# or
sudo dnf install simple-sftpd

# Start service
sudo systemctl start simple-sftpd
sudo systemctl enable simple-sftpd
```

### Windows
```bash
# Download and run installer as Administrator
# The service will be automatically installed and started
```

## 📊 Package Verification

### Verify Package Contents
```bash
# RPM
rpm -qlp simple-sftpd-0.1.0-1.el8.x86_64.rpm

# DEB
dpkg -c simple-sftpd_0.1.0_amd64.deb

# Windows
# Use NSIS installer to view contents
```

### Verify Installation
```bash
# Check if service is running
sudo systemctl status simple-sftpd

# Check configuration
simple-sftpd --test-config --config /etc/simple-sftpd/deployment/simple-sftpd.conf

# Check ports
sudo netstat -tlnp | grep :21
sudo netstat -tlnp | grep :990
```

## 🔍 Troubleshooting

### Common Issues

#### Package Installation Fails
```bash
# Check dependencies
sudo apt-get install -f  # Debian/Ubuntu
sudo yum check  # CentOS/RHEL

# Check package integrity
rpm -K package.rpm
dpkg -I package.deb
```

#### Service Won't Start
```bash
# Check logs
sudo journalctl -u simple-sftpd -f  # systemd
sudo tail -f /var/log/simple-sftpd/simple-sftpd.log

# Check configuration
simple-sftpd --test-config --config /etc/simple-sftpd/deployment/simple-sftpd.conf

# Check permissions
sudo ls -la /etc/simple-sftpd/
sudo ls -la /var/log/simple-sftpd/
```

#### Port Conflicts
```bash
# Check what's using the ports
sudo netstat -tlnp | grep :21
sudo netstat -tlnp | grep :990

# Stop conflicting services
sudo systemctl stop vsftpd
sudo systemctl stop pure-ftpd
```

## 📚 Additional Resources

### Package Manager Documentation
- [RPM Packaging Guide](https://rpm-packaging-guide.github.io/)
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [NSIS Documentation](https://nsis.sourceforge.io/Docs/)

### Distribution-Specific Guides
- [Fedora Packaging Guidelines](https://docs.fedoraproject.org/en-US/packaging-guidelines/)
- [Ubuntu Packaging Guide](https://packaging.ubuntu.com/html/)
- [CentOS Packaging Guidelines](https://wiki.centos.org/HowTos/PackageManagement/)

### Community Support
- [GitHub Issues](https://github.com/simple-sftpd/simple-sftpd/issues)
- [Community Forum](https://community.simple-sftpd.org)
- [Discord Server](https://discord.gg/simple-sftpd)

---

**Next Steps**: 
1. Choose your target platform and package format
2. Install the required build tools
3. Build and test the package
4. Install and verify the package
5. Configure and customize simple-sftpd for your needs
