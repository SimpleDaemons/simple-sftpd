#!/bin/bash

# Simple FTP Daemon - CMake Validation Script
# This script validates CMake configuration files for syntax errors

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

# Function to check CMake syntax
check_cmake_syntax() {
    local file="$1"
    
    if [[ ! -f "$file" ]]; then
        print_error "File not found: $file"
        return 1
    fi
    
    print_status "Checking CMake syntax: $file"
    
    # Basic CMake syntax checks
    local errors=0
    
    # Check for unmatched parentheses
    local open_parens=$(grep -o '(' "$file" | wc -l)
    local close_parens=$(grep -o ')' "$file" | wc -l)
    
    if [[ $open_parens -ne $close_parens ]]; then
        print_error "Unmatched parentheses in $file (open: $open_parens, close: $close_parens)"
        ((errors++))
    fi
    
    # Check for unmatched quotes
    local single_quotes=$(grep -o "'" "$file" | wc -l)
    local double_quotes=$(grep -o '"' "$file" | wc -l)
    
    if [[ $((single_quotes % 2)) -ne 0 ]]; then
        print_error "Unmatched single quotes in $file (count: $single_quotes)"
        ((errors++))
    fi
    
    if [[ $((double_quotes % 2)) -ne 0 ]]; then
        print_error "Unmatched double quotes in $file (count: $double_quotes)"
        ((errors++))
    fi
    
    # Check for common CMake syntax issues
    if grep -q "add_executable(" "$file"; then
        if ! grep -A 10 "add_executable(" "$file" | grep -q ")"; then
            print_error "Incomplete add_executable statement in $file"
            ((errors++))
        fi
    fi
    
    if grep -q "add_library(" "$file"; then
        if ! grep -A 10 "add_library(" "$file" | grep -q ")"; then
            print_error "Incomplete add_library statement in $file"
            ((errors++))
        fi
    fi
    
    if grep -q "target_link_libraries(" "$file"; then
        if ! grep -A 10 "target_link_libraries(" "$file" | grep -q ")"; then
            print_error "Incomplete target_link_libraries statement in $file"
            ((errors++))
        fi
    fi
    
    # Check for missing source files
    local source_files=$(grep -o 'src/[^[:space:]]*\.cpp' "$file" | sort -u)
    for src_file in $source_files; do
        if [[ ! -f "$src_file" ]]; then
            print_warning "Source file referenced but not found: $src_file"
        fi
    done
    
    # Check for missing header files
    local header_files=$(grep -o 'include/[^[:space:]]*\.hpp' "$file" | sort -u)
    for hdr_file in $header_files; do
        if [[ ! -f "$hdr_file" ]]; then
            print_warning "Header file referenced but not found: $hdr_file"
        fi
    done
    
    if [[ $errors -eq 0 ]]; then
        print_success "CMake syntax validation passed for $file"
        return 0
    else
        print_error "CMake syntax validation failed for $file ($errors errors)"
        return 1
    fi
}

# Function to validate project structure
validate_project_structure() {
    print_status "Validating project structure..."
    
    local required_dirs=("include" "src" "cmake" "config" "docs" "tools" "scripts")
    local missing_dirs=()
    
    for dir in "${required_dirs[@]}"; do
        if [[ ! -d "$dir" ]]; then
            missing_dirs+=("$dir")
        fi
    done
    
    if [[ ${#missing_dirs[@]} -gt 0 ]]; then
        print_warning "Missing directories: ${missing_dirs[*]}"
    else
        print_success "Project structure validation passed"
    fi
    
    # Check for required files
    local required_files=("CMakeLists.txt" "README.md" "LICENSE" ".gitignore")
    local missing_files=()
    
    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            missing_files+=("$file")
        fi
    done
    
    if [[ ${#missing_files[@]} -gt 0 ]]; then
        print_warning "Missing files: ${missing_files[*]}"
    else
        print_success "Required files validation passed"
    fi
}

# Function to check for common issues
check_common_issues() {
    print_status "Checking for common CMake issues..."
    
    local issues=0
    
    # Check main CMakeLists.txt
    if [[ -f "CMakeLists.txt" ]]; then
        # Check for project name
        if ! grep -q "project(" "CMakeLists.txt"; then
            print_warning "No project() declaration found in main CMakeLists.txt"
            ((issues++))
        fi
        
        # Check for cmake_minimum_required
        if ! grep -q "cmake_minimum_required(" "CMakeLists.txt"; then
            print_warning "No cmake_minimum_required() found in main CMakeLists.txt"
            ((issues++))
        fi
        
        # Check for add_subdirectory calls
        if ! grep -q "add_subdirectory(" "CMakeLists.txt"; then
            print_warning "No add_subdirectory() calls found in main CMakeLists.txt"
            ((issues++))
        fi
    fi
    
    # Check for circular dependencies
    if grep -r "add_subdirectory(" . --include="CMakeLists.txt" | grep -q "src/tests"; then
        if grep -r "add_subdirectory(" . --include="CMakeLists.txt" | grep -q "src/core"; then
            print_warning "Potential circular dependency: tests depend on core"
            ((issues++))
        fi
    fi
    
    if [[ $issues -eq 0 ]]; then
        print_success "Common issues check passed"
    else
        print_warning "Found $issues potential issues"
    fi
}

# Function to display usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -f, --file <file>     Validate specific CMake file"
    echo "  -a, --all             Validate all CMake files (default)"
    echo "  -s, --structure       Validate project structure only"
    echo "  -c, --common          Check for common issues only"
    echo "  -h, --help            Display this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Validate all CMake files"
    echo "  $0 -f CMakeLists.txt  # Validate specific file"
    echo "  $0 -s                 # Validate project structure only"
    echo "  $0 -c                 # Check for common issues only"
}

# Main script
main() {
    local validate_all=true
    local validate_structure=true
    local validate_common=true
    local specific_file=""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -f|--file)
                specific_file="$2"
                validate_all=false
                shift 2
                ;;
            -a|--all)
                validate_all=true
                shift
                ;;
            -s|--structure)
                validate_all=false
                validate_structure=true
                validate_common=false
                shift
                ;;
            -c|--common)
                validate_all=false
                validate_structure=false
                validate_common=true
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
    
    print_status "Starting CMake validation..."
    echo ""
    
    local exit_code=0
    
    # Validate project structure
    if [[ "$validate_structure" == "true" ]]; then
        validate_project_structure
        echo ""
    fi
    
    # Check for common issues
    if [[ "$validate_common" == "true" ]]; then
        check_common_issues
        echo ""
    fi
    
    # Validate CMake files
    if [[ "$validate_all" == "true" ]]; then
        # Find all CMakeLists.txt files
        local cmake_files=$(find . -name "CMakeLists.txt" -type f)
        
        if [[ -z "$cmake_files" ]]; then
            print_warning "No CMakeLists.txt files found"
        else
            print_status "Found CMake files:"
            echo "$cmake_files" | sed 's/^/  /'
            echo ""
            
            for file in $cmake_files; do
                if ! check_cmake_syntax "$file"; then
                    exit_code=1
                fi
                echo ""
            done
        fi
    elif [[ -n "$specific_file" ]]; then
        if ! check_cmake_syntax "$specific_file"; then
            exit_code=1
        fi
    fi
    
    # Summary
    echo "=========================================="
    if [[ $exit_code -eq 0 ]]; then
        print_success "CMake validation completed successfully!"
    else
        print_error "CMake validation completed with errors!"
    fi
    echo "=========================================="
    
    exit $exit_code
}

# Run main function with all arguments
main "$@"
