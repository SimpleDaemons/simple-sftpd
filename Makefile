# Makefile for simple-ftpd
# Simple FTP Daemon - A secure and configurable FTP server

# Variables
PROJECT_NAME = simple-ftpd
VERSION = 0.1.0
BUILD_DIR = build
DIST_DIR = dist
PACKAGE_DIR = packaging

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    CXX = clang++
    CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
    LDFLAGS = -lssl -lcrypto
    # macOS specific flags
    CXXFLAGS += -target x86_64-apple-macos12.0 -target arm64-apple-macos12.0
    # Detect processor cores for parallel builds
    PARALLEL_JOBS = $(shell sysctl -n hw.ncpu)
    # macOS install paths
    INSTALL_PREFIX = /usr/local
    CONFIG_DIR = $(INSTALL_PREFIX)/etc/$(PROJECT_NAME)
else
    PLATFORM = linux
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
    LDFLAGS = -lssl -lcrypto -lpthread
    # Linux specific flags
    PARALLEL_JOBS = $(shell nproc)
    # Linux install paths
    INSTALL_PREFIX = /usr/local
    CONFIG_DIR = /etc/$(PROJECT_NAME)
endif

# Directories
SRC_DIR = src
INCLUDE_DIR = include
CONFIG_DIR_SRC = config
SCRIPTS_DIR = scripts

# Default target
all: build

# Create build directory
$(BUILD_DIR)-dir:
	mkdir -p $(BUILD_DIR)

# Build using CMake
build: $(BUILD_DIR)-dir
	cd $(BUILD_DIR) && cmake .. && make -j$(PARALLEL_JOBS)

# Clean build
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)

# Install
install: build
	cd $(BUILD_DIR) && sudo make install

# Uninstall
uninstall:
	sudo rm -f $(INSTALL_PREFIX)/bin/$(PROJECT_NAME)
	sudo rm -f $(INSTALL_PREFIX)/lib/lib$(PROJECT_NAME).so
	sudo rm -f $(INSTALL_PREFIX)/lib/lib$(PROJECT_NAME).dylib
	sudo rm -rf $(INSTALL_PREFIX)/include/$(PROJECT_NAME)
	sudo rm -rf $(CONFIG_DIR)

# Test
test: build
	cd $(BUILD_DIR) && make test

# Package RPM (Linux only)
package-rpm: build
ifeq ($(PLATFORM),linux)
	@echo "Building RPM package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G RPM
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm $(DIST_DIR)/
else
	@echo "RPM packages are only supported on Linux"
endif

# Package DEB (Linux only)
package-deb: build
ifeq ($(PLATFORM),linux)
	@echo "Building DEB package..."
	@mkdir -p $(DIST_DIR)
	cd $(BUILD_DIR) && cpack -G DEB
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb $(DIST_DIR)/
else
	@echo "DEB packages are only supported on Linux"
endif

# Package using build scripts (recommended)
package-script:
ifeq ($(PLATFORM),macos)
	@echo "Building macOS package with build script..."
	@mkdir -p $(DIST_DIR)
	./scripts/build-macos.sh
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.pkg $(DIST_DIR)/
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.dmg $(DIST_DIR)/
else ifeq ($(PLATFORM),linux)
	@echo "Building Linux package with build script..."
	@mkdir -p $(DIST_DIR)
	./scripts/build-linux.sh
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.deb $(DIST_DIR)/
	mv $(BUILD_DIR)/$(PROJECT_NAME)-$(VERSION)-*.rpm $(DIST_DIR)/
else
	@echo "Package script not available for this platform"
endif

# Package all formats
package: package-script

# Development targets
debug: $(BUILD_DIR)-dir
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(PARALLEL_JOBS)

release: $(BUILD_DIR)-dir
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(PARALLEL_JOBS)

# Documentation
docs:
	@echo "Generating documentation..."
	@mkdir -p docs
	# Add documentation generation commands here

# Code formatting
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Static analysis
analyze: build
	@echo "Running static analysis..."
	cd $(BUILD_DIR) && make clean && scan-build make

# Coverage (Linux only)
coverage: build
ifeq ($(PLATFORM),linux)
	@echo "Generating coverage report..."
	cd $(BUILD_DIR) && make coverage
else
	@echo "Coverage is only supported on Linux"
endif

# Valgrind memory check (Linux only)
memcheck: build
ifeq ($(PLATFORM),linux)
	@echo "Running memory check..."
	cd $(BUILD_DIR) && make test && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./bin/$(PROJECT_NAME)-test
else
	@echo "Valgrind is only supported on Linux"
endif

# Install development dependencies
install-dev:
ifeq ($(PLATFORM),macos)
	@echo "Installing development dependencies for macOS..."
	brew install cmake openssl jsoncpp clang-format
else ifeq ($(PLATFORM),linux)
	@echo "Installing development dependencies for Linux..."
	sudo apt-get update
	sudo apt-get install -y build-essential cmake libssl-dev libjsoncpp-dev clang-format
else
	@echo "Development dependencies installation not configured for this platform"
endif

# Service management
install-service:
ifeq ($(PLATFORM),macos)
	@echo "Installing macOS service..."
	sudo cp scripts/com.blburns.simple-ftpd.plist /Library/LaunchDaemons/
	sudo chown root:wheel /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
	sudo chmod 644 /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
	sudo launchctl load /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
else ifeq ($(PLATFORM),linux)
	@echo "Installing Linux service..."
	sudo cp scripts/simple-ftpd.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable simple-ftpd
else
	@echo "Service installation not configured for this platform"
endif

uninstall-service:
ifeq ($(PLATFORM),macos)
	@echo "Uninstalling macOS service..."
	sudo launchctl unload /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
	sudo rm -f /Library/LaunchDaemons/com.blburns.simple-ftpd.plist
else ifeq ($(PLATFORM),linux)
	@echo "Uninstalling Linux service..."
	sudo systemctl stop simple-ftpd
	sudo systemctl disable simple-ftpd
	sudo rm -f /etc/systemd/system/simple-ftpd.service
	sudo systemctl daemon-reload
else
	@echo "Service uninstallation not configured for this platform"
endif

# Configuration management
install-config:
	@echo "Installing configuration files..."
	sudo mkdir -p $(CONFIG_DIR)
	sudo cp -r $(CONFIG_DIR_SRC)/* $(CONFIG_DIR)/
	sudo chown -R root:root $(CONFIG_DIR)
	sudo chmod -R 644 $(CONFIG_DIR)/*
	sudo chmod 755 $(CONFIG_DIR)

# SSL certificate management
setup-ssl:
	@echo "Setting up SSL certificates..."
	@mkdir -p $(CONFIG_DIR)/ssl
	@echo "Please place your SSL certificates in $(CONFIG_DIR)/ssl/"
	@echo "Required files:"
	@echo "  - server.crt (server certificate)"
	@echo "  - server.key (private key)"
	@echo "  - ca.crt (CA certificate, optional)"

# User management
setup-users:
	@echo "Setting up user accounts..."
	@mkdir -p $(CONFIG_DIR)/users
	@echo "Please configure user accounts in $(CONFIG_DIR)/users/"

# Virtual host setup
setup-virtuals:
	@echo "Setting up virtual hosts..."
	@mkdir -p $(CONFIG_DIR)/virtuals
	@echo "Please configure virtual hosts in $(CONFIG_DIR)/virtuals/"

# Complete setup
setup: install install-config setup-ssl setup-users setup-virtuals install-service
	@echo "Complete setup finished!"
	@echo "Configuration directory: $(CONFIG_DIR)"
	@echo "Log directory: /var/log/$(PROJECT_NAME)"
	@echo "SSL directory: $(CONFIG_DIR)/ssl"

# Clean setup
clean-setup: uninstall-service uninstall
	@echo "Clean setup finished!"

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build the project (default)"
	@echo "  build            - Build using CMake"
	@echo "  clean            - Clean build artifacts"
	@echo "  install          - Install the application"
	@echo "  uninstall        - Uninstall the application"
	@echo "  test             - Run tests"
	@echo "  package          - Create packages for all platforms"
	@echo "  package-rpm      - Create RPM package (Linux only)"
	@echo "  package-deb      - Create DEB package (Linux only)"
	@echo "  debug            - Build debug version"
	@echo "  release          - Build release version"
	@echo "  docs             - Generate documentation"
	@echo "  format           - Format code with clang-format"
	@echo "  analyze          - Run static analysis"
	@echo "  coverage         - Generate coverage report (Linux only)"
	@echo "  memcheck         - Run memory check (Linux only)"
	@echo "  install-dev      - Install development dependencies"
	@echo "  install-service  - Install system service"
	@echo "  uninstall-service - Uninstall system service"
	@echo "  install-config   - Install configuration files"
	@echo "  setup-ssl        - Setup SSL certificates"
	@echo "  setup-users      - Setup user accounts"
	@echo "  setup-virtuals   - Setup virtual hosts"
	@echo "  setup            - Complete setup"
	@echo "  clean-setup      - Clean complete setup"
	@echo "  help             - Show this help message"

# Phony targets
.PHONY: all build clean install uninstall test package package-rpm package-deb package-script \
        debug release docs format analyze coverage memcheck install-dev install-service \
        uninstall-service install-config setup-ssl setup-users setup-virtuals setup \
        clean-setup help
