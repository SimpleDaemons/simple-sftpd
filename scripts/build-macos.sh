#!/bin/bash

# Simple FTP Daemon - macOS Build Script
# This script builds and packages the application for macOS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
DIST_DIR="$PROJECT_ROOT/dist"
VERSION="0.1.0"

# Platform detection
ARCH=$(uname -m)
if [[ "$ARCH" == "arm64" ]]; then
    PLATFORM="macOS-ARM64"
    CMAKE_ARCH="arm64"
elif [[ "$ARCH" == "x86_64" ]]; then
    PLATFORM="macOS-x86_64"
    CMAKE_ARCH="x86_64"
else
    echo -e "${RED}Error: Unsupported architecture: $ARCH${NC}"
    exit 1
fi

# Functions
print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Simple FTP Daemon Build${NC}"
    echo -e "${BLUE}  Platform: $PLATFORM${NC}"
    echo -e "${BLUE}  Version: $VERSION${NC}"
    echo -e "${BLUE}================================${NC}"
}

print_step() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_dependencies() {
    print_step "Checking build dependencies..."
    
    # Check for Homebrew
    if ! command -v brew &> /dev/null; then
        print_error "Homebrew is not installed. Please install it first:"
        echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        exit 1
    fi
    
    # Check for required tools
    local missing_tools=()
    
    if ! command -v cmake &> /dev/null; then
        missing_tools+=("cmake")
    fi
    
    if ! command -v make &> /dev/null; then
        missing_tools+=("make")
    fi
    
    if ! command -v git &> /dev/null; then
        missing_tools+=("git")
    fi
    
    # Check for required libraries
    if ! brew list openssl &> /dev/null; then
        missing_tools+=("openssl")
    fi
    
    if ! brew list jsoncpp &> /dev/null; then
        missing_tools+=("jsoncpp")
    fi
    
    if ! brew list pkg-config &> /dev/null; then
        missing_tools+=("pkg-config")
    fi
    
    # Install missing tools
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        print_warning "Installing missing dependencies: ${missing_tools[*]}"
        brew install "${missing_tools[@]}"
    fi
    
    print_step "All dependencies are available"
}

clean_build() {
    print_step "Cleaning previous build artifacts..."
    
    if [[ -d "$BUILD_DIR" ]]; then
        rm -rf "$BUILD_DIR"
    fi
    
    if [[ -d "$DIST_DIR" ]]; then
        rm -rf "$DIST_DIR"
    fi
    
    mkdir -p "$BUILD_DIR" "$DIST_DIR"
}

configure_build() {
    print_step "Configuring build with CMake..."
    
    cd "$BUILD_DIR"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_ARCHITECTURES="$CMAKE_ARCH" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_TESTS=ON \
        -DBUILD_EXAMPLES=OFF \
        -DENABLE_LOGGING=ON \
        -DENABLE_SSL=ON \
        -DUSE_SYSTEM_LIBS=ON
    
    if [[ $? -ne 0 ]]; then
        print_error "CMake configuration failed"
        exit 1
    fi
}

build_project() {
    print_step "Building project..."
    
    cd "$BUILD_DIR"
    
    # Get number of CPU cores for parallel build
    local cpu_cores=$(sysctl -n hw.ncpu)
    
    make -j"$cpu_cores"
    
    if [[ $? -ne 0 ]]; then
        print_error "Build failed"
        exit 1
    fi
    
    print_step "Build completed successfully"
}

run_tests() {
    print_step "Running tests..."
    
    cd "$BUILD_DIR"
    
    if make test; then
        print_step "All tests passed"
    else
        print_warning "Some tests failed, but continuing with packaging"
    fi
}

create_packages() {
    print_step "Creating macOS packages..."
    
    cd "$BUILD_DIR"
    
    # Create DMG package
    if cpack -G DragNDrop; then
        local dmg_file=$(find . -name "*.dmg" -type f | head -1)
        if [[ -n "$dmg_file" ]]; then
            mv "$dmg_file" "$DIST_DIR/"
            print_step "DMG package created: $(basename "$dmg_file")"
        fi
    else
        print_warning "Failed to create DMG package"
    fi
    
    # Create PKG package
    if cpack -G productbuild; then
        local pkg_file=$(find . -name "*.pkg" -type f | head -1)
        if [[ -n "$pkg_file" ]]; then
            mv "$pkg_file" "$DIST_DIR/"
            print_step "PKG package created: $(basename "$pkg_file")"
        fi
    else
        print_warning "Failed to create PKG package"
    fi
    
    # Create source archive
    cd "$PROJECT_ROOT"
    local source_archive="ssftpd-${VERSION}-source.tar.gz"
    if git archive --format=tar.gz --prefix="ssftpd-${VERSION}/" HEAD > "$DIST_DIR/$source_archive"; then
        print_step "Source archive created: $source_archive"
    fi
}

install_package() {
    print_step "Installing package for testing..."
    
    cd "$BUILD_DIR"
    
    if sudo make install; then
        print_step "Package installed successfully"
        
        # Test the installation
        if command -v ssftpd &> /dev/null; then
            print_step "Installation verification successful"
            ssftpd --version
        else
            print_warning "Installation verification failed - binary not found in PATH"
        fi
    else
        print_error "Package installation failed"
        exit 1
    fi
}

create_checksums() {
    print_step "Creating checksums for packages..."
    
    cd "$DIST_DIR"
    
    for file in *; do
        if [[ -f "$file" ]]; then
            shasum -a 256 "$file" > "$file.sha256"
            print_step "Created checksum for: $file"
        fi
    done
}

print_summary() {
    echo
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Build Summary${NC}"
    echo -e "${BLUE}================================${NC}"
    echo -e "Platform: ${GREEN}$PLATFORM${NC}"
    echo -e "Version: ${GREEN}$VERSION${NC}"
    echo -e "Build Directory: ${GREEN}$BUILD_DIR${NC}"
    echo -e "Distribution Directory: ${GREEN}$DIST_DIR${NC}"
    echo
    
    if [[ -d "$DIST_DIR" ]]; then
        echo -e "${GREEN}Created packages:${NC}"
        for file in "$DIST_DIR"/*; do
            if [[ -f "$file" ]]; then
                local size=$(du -h "$file" | cut -f1)
                echo -e "  $(basename "$file") (${size})"
            fi
        done
    fi
    
    echo
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "Packages are available in: ${GREEN}$DIST_DIR${NC}"
}

# Main build process
main() {
    print_header
    
    # Check if we're on macOS
    if [[ "$(uname)" != "Darwin" ]]; then
        print_error "This script is designed for macOS only"
        exit 1
    fi
    
    # Parse command line arguments
    local install_pkg=false
    local skip_tests=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --install)
                install_pkg=true
                shift
                ;;
            --skip-tests)
                skip_tests=true
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  --install      Install package after building"
                echo "  --skip-tests   Skip running tests"
                echo "  --help, -h     Show this help message"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # Execute build steps
    check_dependencies
    clean_build
    configure_build
    build_project
    
    if [[ "$skip_tests" != "true" ]]; then
        run_tests
    fi
    
    create_packages
    
    if [[ "$install_pkg" == "true" ]]; then
        install_package
    fi
    
    create_checksums
    print_summary
}

# Run main function with all arguments
main "$@"
