#!/bin/bash

# Simple-Secure FTP Daemon - Project Rename Script
# This script renames the project from "ssftpd" to "ssftpd"

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if running in git repository
check_git_repo() {
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        print_error "Not in a git repository"
        exit 1
    fi
}

# Function to backup current state
backup_current_state() {
    print_status "Creating backup of current state..."
    
    local backup_branch="backup-before-rename-$(date +%Y%m%d_%H%M%S)"
    git checkout -b "$backup_branch"
    git checkout main
    
    print_success "Backup created in branch: $backup_branch"
}

# Function to rename directories
rename_directories() {
    print_status "Renaming directories..."
    
    # Rename include directory
    if [[ -d "include/ssftpd" ]]; then
        mv "include/ssftpd" "include/ssftpd"
        print_success "Renamed include/ssftpd to include/ssftpd"
    fi
    
    # Rename main project directory if needed
    local current_dir=$(basename "$PWD")
    if [[ "$current_dir" == "ssftpd" ]]; then
        print_warning "Current directory is 'ssftpd'"
        print_warning "You may need to rename the parent directory manually after this script completes"
    fi
}

# Function to rename files
rename_files() {
    print_status "Renaming files..."
    
    # Find and rename files with ssftpd in the name
    find . -name "*ssftpd*" -type f | while read -r file; do
        local new_name=$(echo "$file" | sed 's/ssftpd/ssftpd/g')
        if [[ "$file" != "$new_name" ]]; then
            mv "$file" "$new_name"
            print_success "Renamed: $file -> $new_name"
        fi
    done
    
    # Find and rename directories with ssftpd in the name
    find . -name "*ssftpd*" -type d | sort -r | while read -r dir; do
        local new_name=$(echo "$dir" | sed 's/ssftpd/ssftpd/g')
        if [[ "$dir" != "$new_name" ]]; then
            mv "$dir" "$new_name"
            print_success "Renamed: $dir -> $new_name"
        fi
    done
}

# Function to update file contents
update_file_contents() {
    print_status "Updating file contents..."
    
    # Use sed to replace all occurrences of ssftpd with ssftpd
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" -o -name "*.cmake" -o -name "CMakeLists.txt" -o -name "*.md" -o -name "*.txt" -o -name "*.conf" -o -name "*.sh" -o -name "*.py" -o -name "*.yml" -o -name "*.yaml" -o -name "*.json" -o -name "*.xml" -o -name "*.ini" -o -name "*.cfg" \) -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    else
        # Linux sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" -o -name "*.cmake" -o -name "CMakeLists.txt" -o -name "*.md" -o -name "*.txt" -o -name "*.conf" -o -name "*.sh" -o -name "*.py" -o -name "*.yml" -o -name "*.yaml" -o -name "*.json" -o -name "*.xml" -o -name "*.ini" -o -name "*.cfg" \) -exec sed -i 's/ssftpd/ssftpd/g' {} \;
    fi
    
    print_success "Updated file contents"
}

# Function to update specific patterns
update_specific_patterns() {
    print_status "Updating specific patterns..."
    
    # Update project name in CMake files
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        sed -i '' 's/project(ssftpd/project(ssftpd/g' CMakeLists.txt
        sed -i '' 's/ssftpd-lib/ssftpd-lib/g' CMakeLists.txt
        sed -i '' 's/ssftpd-config/ssftpd-config/g' CMakeLists.txt
        sed -i '' 's/ssftpd-${SYSTEM_ARCH}/ssftpd-${SYSTEM_ARCH}/g' CMakeLists.txt
    else
        # Linux sed
        sed -i 's/project(ssftpd/project(ssftpd/g' CMakeLists.txt
        sed -i 's/ssftpd-lib/ssftpd-lib/g' CMakeLists.txt
        sed -i 's/ssftpd-config/ssftpd-config/g' CMakeLists.txt
        sed -i 's/ssftpd-${SYSTEM_ARCH}/ssftpd-${SYSTEM_ARCH}/g' CMakeLists.txt
    fi
    
    # Update include paths
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i '' 's/#include "ssftpd\//#include "ssftpd\//g' {} \;
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i '' 's/#include <ssftpd\//#include <ssftpd\//g' {} \;
    else
        # Linux sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i 's/#include "ssftpd\//#include "ssftpd\//g' {} \;
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i 's/#include <ssftpd\//#include <ssftpd\//g' {} \;
    fi
    
    # Update namespace references
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i '' 's/namespace simple_ftpd/namespace ssftpd/g' {} \;
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i '' 's/simple_ftpd::/ssftpd::/g' {} \;
    else
        # Linux sed
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i 's/namespace simple_ftpd/namespace ssftpd/g' {} \;
        find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec sed -i 's/simple_ftpd::/ssftpd::g' {} \;
    fi
    
    print_success "Updated specific patterns"
}

# Function to update configuration files
update_config_files() {
    print_status "Updating configuration files..."
    
    # Update config file names and contents
    if [[ -f "config/ssftpd.conf.example" ]]; then
        mv "config/ssftpd.conf.example" "config/ssftpd.conf.example"
        print_success "Renamed config/ssftpd.conf.example to config/ssftpd.conf.example"
    fi
    
    # Update config file contents
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find config/ -type f -name "*.conf*" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    else
        # Linux sed
        find config/ -type f -name "*.conf*" -exec sed -i 's/ssftpd/ssftpd/g' {} \;
    fi
    
    print_success "Updated configuration files"
}

# Function to update documentation
update_documentation() {
    print_status "Updating documentation..."
    
    # Update project title and description
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        sed -i '' 's/Simple FTP Daemon/Simple-Secure FTP Daemon/g' README.md
        sed -i '' 's/ssftpd/ssftpd/g' README.md
        sed -i '' 's/Simple FTP Daemon/Simple-Secure FTP Daemon/g' docs/*/README.md 2>/dev/null || true
        sed -i '' 's/ssftpd/ssftpd/g' docs/*/README.md 2>/dev/null || true
    else
        # Linux sed
        sed -i 's/Simple FTP Daemon/Simple-Secure FTP Daemon/g' README.md
        sed -i 's/ssftpd/ssftpd/g' README.md
        sed -i 's/Simple FTP Daemon/Simple-Secure FTP Daemon/g' docs/*/README.md 2>/dev/null || true
        sed -i 's/ssftpd/ssftpd/g' docs/*/README.md 2>/dev/null || true
    fi
    
    print_success "Updated documentation"
}

# Function to update build scripts
update_build_scripts() {
    print_status "Updating build scripts..."
    
    # Update Makefile
    if [[ -f "Makefile" ]]; then
        if [[ "$OSTYPE" == "darwin"* ]]; then
            # macOS sed
            sed -i '' 's/ssftpd/ssftpd/g' Makefile
        else
            # Linux sed
            sed -i 's/ssftpd/ssftpd/g' Makefile
        fi
        print_success "Updated Makefile"
    fi
    
    # Update CMake files
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find . -name "CMakeLists.txt" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
        find . -name "*.cmake" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    else
        # Linux sed
        find . -name "CMakeLists.txt" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
        find . -name "*.cmake" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    fi
    
    print_success "Updated build scripts"
}

# Function to update utility scripts
update_utility_scripts() {
    print_status "Updating utility scripts..."
    
    # Update script contents
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        find tools/ -type f -name "*.sh" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
        find scripts/ -type f -name "*.sh" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    else
        # Linux sed
        find tools/ -type f -name "*.sh" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
        find scripts/ -type f -name "*.sh" -exec sed -i '' 's/ssftpd/ssftpd/g' {} \;
    fi
    
    print_success "Updated utility scripts"
}

# Function to update package names
update_package_names() {
    print_status "Updating package names..."
    
    # Update package names in various files
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        sed -i '' 's/ssftpd/ssftpd/g' cmake/*.cmake.in 2>/dev/null || true
        sed -i '' 's/ssftpd/ssftpd/g' cmake/*.cmake 2>/dev/null || true
    else
        # Linux sed
        sed -i 's/ssftpd/ssftpd/g' cmake/*.cmake.in 2>/dev/null || true
        sed -i 's/ssftpd/ssftpd/g' cmake/*.cmake 2>/dev/null || true
    fi
    
    print_success "Updated package names"
}

# Function to clean up and verify
cleanup_and_verify() {
    print_status "Cleaning up and verifying..."
    
    # Remove any backup files created by sed
    find . -name "*.bak" -delete 2>/dev/null || true
    find . -name "*~" -delete 2>/dev/null || true
    
    # Check for any remaining ssftpd references
    local remaining_refs=$(grep -r "ssftpd" . --exclude-dir=.git --exclude-dir=build --exclude-dir=dist 2>/dev/null | wc -l)
    
    if [[ $remaining_refs -eq 0 ]]; then
        print_success "No remaining 'ssftpd' references found"
    else
        print_warning "Found $remaining_refs remaining 'ssftpd' references:"
        grep -r "ssftpd" . --exclude-dir=.git --exclude-dir=build --exclude-dir=dist 2>/dev/null | head -10
    fi
    
    print_success "Cleanup and verification completed"
}

# Function to display usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -b, --backup           Create backup branch before proceeding"
    echo "  -f, --force            Skip confirmation prompts"
    echo "  -h, --help             Display this help message"
    echo ""
    echo "This script will rename the project from 'ssftpd' to 'ssftpd'"
    echo "throughout the entire codebase, including:"
    echo "- Directory names"
    echo "- File names"
    echo "- File contents"
    echo "- Build configurations"
    echo "- Documentation"
    echo "- Scripts and utilities"
    echo ""
    echo "WARNING: This is a destructive operation that will rename many files!"
    echo "Make sure you have a backup before proceeding!"
}

# Main script
main() {
    local create_backup=false
    local force=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -b|--backup)
                create_backup=true
                shift
                ;;
            -f|--force)
                force=true
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            -*)
                print_error "Unknown option: $1"
                usage
                exit 1
                ;;
            *)
                print_error "Unexpected argument: $1"
                usage
                exit 1
                ;;
        esac
    done
    
    # Check if we're in a git repository
    check_git_repo
    
    print_status "Starting project rename from 'ssftpd' to 'ssftpd'..."
    echo ""
    
    if [[ "$force" != "true" ]]; then
        print_warning "This script will rename the project throughout the entire codebase!"
        print_warning "This includes renaming directories, files, and updating all content!"
        print_warning "Make sure you have a backup before proceeding!"
        
        read -p "Do you want to continue? (y/N): " -n 1 -r
        echo
        
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_error "Aborted by user"
            exit 1
        fi
    fi
    
    # Create backup if requested
    if [[ "$create_backup" == "true" ]]; then
        backup_current_state
        echo ""
    fi
    
    # Perform the rename operations
    rename_directories
    echo ""
    
    rename_files
    echo ""
    
    update_file_contents
    echo ""
    
    update_specific_patterns
    echo ""
    
    update_config_files
    echo ""
    
    update_documentation
    echo ""
    
    update_build_scripts
    echo ""
    
    update_utility_scripts
    echo ""
    
    update_package_names
    echo ""
    
    cleanup_and_verify
    echo ""
    
    print_success "Project rename completed successfully!"
    echo ""
    print_status "Next steps:"
    echo "1. Review the changes: git status"
    echo "2. Test the build system: make clean && make build"
    echo "3. Commit the changes: git add . && git commit -m 'refactor: Rename project from ssftpd to ssftpd'"
    echo "4. Update any external references or CI/CD systems"
    echo ""
    print_warning "Note: You may need to manually rename the parent directory if it contains 'ssftpd'"
}

# Run main function with all arguments
main "$@"
