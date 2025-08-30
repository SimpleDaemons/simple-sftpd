#!/bin/bash

# Simple FTP Daemon - User Management Script
# This script helps manage FTP user accounts

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
USERS_FILE="/etc/ssftpd/users.conf"
FTP_DIR="/var/ftp"
FTP_USER="ftp"
FTP_GROUP="ftp"

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

# Function to check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root"
        exit 1
    fi
}

# Function to check if user exists
user_exists() {
    local username="$1"
    if [[ -f "$USERS_FILE" ]] && grep -q "^\[user:$username\]" "$USERS_FILE"; then
        return 0
    else
        return 1
    fi
}

# Function to create user configuration
create_user_config() {
    local username="$1"
    local password="$2"
    local home_dir="$3"
    local permissions="$4"
    
    print_status "Creating user configuration for $username..."
    
    # Create users directory if it doesn't exist
    mkdir -p "$(dirname "$USERS_FILE")"
    
    # Add user to configuration file
    cat >> "$USERS_FILE" << EOF

[user:$username]
password = $password
home_directory = $home_dir
permissions = $permissions
status = active
max_connections = 5
transfer_rate_limit = 1048576
max_file_size = 104857600
EOF
    
    print_success "User configuration created"
}

# Function to create user directory
create_user_directory() {
    local username="$1"
    local home_dir="$2"
    
    print_status "Creating user directory: $home_dir"
    
    # Create user directory
    mkdir -p "$home_dir"
    
    # Set ownership
    chown $FTP_USER:$FTP_GROUP "$home_dir"
    
    # Set permissions
    chmod 755 "$home_dir"
    
    # Create welcome file
    cat > "$home_dir/README.txt" << EOF
Welcome to your FTP directory, $username!

This is your personal FTP space. You can:
- Upload files using FTP clients
- Download files you've uploaded
- Create and manage directories
- Share files with others

For help, contact your system administrator.
EOF
    
    chown $FTP_USER:$FTP_GROUP "$home_dir/README.txt"
    chmod 644 "$home_dir/README.txt"
    
    print_success "User directory created"
}

# Function to add a new user
add_user() {
    local username="$1"
    local password="$2"
    local home_dir="$3"
    local permissions="$4"
    
    # Validate username
    if [[ ! "$username" =~ ^[a-zA-Z0-9_-]+$ ]]; then
        print_error "Invalid username. Use only letters, numbers, hyphens, and underscores."
        exit 1
    fi
    
    # Check if user already exists
    if user_exists "$username"; then
        print_error "User $username already exists"
        exit 1
    fi
    
    # Set default values if not provided
    [[ -z "$home_dir" ]] && home_dir="$FTP_DIR/$username"
    [[ -z "$permissions" ]] && permissions="read,list,download,upload"
    
    # Create user configuration
    create_user_config "$username" "$password" "$home_dir" "$permissions"
    
    # Create user directory
    create_user_directory "$username" "$home_dir"
    
    print_success "User $username added successfully"
    echo ""
    echo "User details:"
    echo "  Username: $username"
    echo "  Home directory: $home_dir"
    echo "  Permissions: $permissions"
    echo ""
    echo "Next steps:"
    echo "1. Restart the FTP server: systemctl restart ssftpd"
    echo "2. Test the connection: ftp -u $username localhost"
}

# Function to remove a user
remove_user() {
    local username="$1"
    local remove_files="$2"
    
    # Check if user exists
    if ! user_exists "$username"; then
        print_error "User $username does not exist"
        exit 1
    fi
    
    print_status "Removing user $username..."
    
    # Get user's home directory
    local home_dir=$(grep "^home_directory = " "$USERS_FILE" | grep -A 10 "^\[user:$username\]" | head -1 | cut -d'=' -f2 | tr -d ' ')
    
    # Remove user from configuration
    if [[ -f "$USERS_FILE" ]]; then
        # Find the line numbers for the user section
        local start_line=$(grep -n "^\[user:$username\]" "$USERS_FILE" | cut -d: -f1)
        if [[ -n "$start_line" ]]; then
            # Find the end of the section (next section or end of file)
            local end_line=$(tail -n +$((start_line + 1)) "$USERS_FILE" | grep -n "^\[" | head -1 | cut -d: -f1)
            if [[ -n "$end_line" ]]; then
                end_line=$((start_line + end_line))
            else
                end_line=$(wc -l < "$USERS_FILE")
            fi
            
            # Remove the section
            sed -i "${start_line},${end_line}d" "$USERS_FILE"
        fi
    fi
    
    # Remove user directory if requested
    if [[ "$remove_files" == "true" ]] && [[ -n "$home_dir" ]] && [[ -d "$home_dir" ]]; then
        print_warning "Removing user directory: $home_dir"
        rm -rf "$home_dir"
    fi
    
    print_success "User $username removed successfully"
}

# Function to list all users
list_users() {
    if [[ ! -f "$USERS_FILE" ]]; then
        print_warning "No users configuration file found"
        return
    fi
    
    print_status "Listing all users:"
    echo ""
    
    # Extract user information
    local users=$(grep "^\[user:" "$USERS_FILE" | sed 's/^\[user://' | sed 's/\]$//')
    
    if [[ -z "$users" ]]; then
        echo "No users found"
        return
    fi
    
    printf "%-20s %-30s %-15s %-10s\n" "Username" "Home Directory" "Permissions" "Status"
    echo "--------------------------------------------------------------------------------"
    
    for username in $users; do
        # Get user details
        local section_start=$(grep -n "^\[user:$username\]" "$USERS_FILE" | cut -d: -f1)
        if [[ -n "$section_start" ]]; then
            local home_dir=$(tail -n +$section_start "$USERS_FILE" | grep "^home_directory = " | head -1 | cut -d'=' -f2 | tr -d ' ')
            local permissions=$(tail -n +$section_start "$USERS_FILE" | grep "^permissions = " | head -1 | cut -d'=' -f2 | tr -d ' ')
            local status=$(tail -n +$section_start "$USERS_FILE" | grep "^status = " | head -1 | cut -d'=' -f2 | tr -d ' ')
            
            # Set defaults if not found
            [[ -z "$home_dir" ]] && home_dir="N/A"
            [[ -z "$permissions" ]] && permissions="N/A"
            [[ -z "$status" ]] && status="active"
            
            printf "%-20s %-30s %-15s %-10s\n" "$username" "$home_dir" "$permissions" "$status"
        fi
    done
    echo ""
}

# Function to modify user
modify_user() {
    local username="$1"
    local field="$2"
    local value="$3"
    
    # Check if user exists
    if ! user_exists "$username"; then
        print_error "User $username does not exist"
        exit 1
    fi
    
    # Validate field
    local valid_fields="password home_directory permissions status max_connections transfer_rate_limit max_file_size"
    if ! echo "$valid_fields" | grep -q "\b$field\b"; then
        print_error "Invalid field: $field"
        echo "Valid fields: $valid_fields"
        exit 1
    fi
    
    print_status "Modifying user $username: $field = $value"
    
    # Find and replace the field value
    local section_start=$(grep -n "^\[user:$username\]" "$USERS_FILE" | cut -d: -f1)
    if [[ -n "$section_start" ]]; then
        # Find the line with the field
        local field_line=$(tail -n +$section_start "$USERS_FILE" | grep -n "^$field = " | head -1 | cut -d: -f1)
        if [[ -n "$field_line" ]]; then
            local actual_line=$((section_start + field_line - 1))
            sed -i "${actual_line}s/^$field = .*/$field = $value/" "$USERS_FILE"
            print_success "Field $field updated successfully"
        else
            # Field doesn't exist, add it
            local next_section=$(tail -n +$((section_start + 1)) "$USERS_FILE" | grep -n "^\[" | head -1 | cut -d: -f1)
            if [[ -n "$next_section" ]]; then
                local insert_line=$((section_start + next_section - 1))
                sed -i "${insert_line}i$field = $value" "$USERS_FILE"
            else
                # Add at end of file
                echo "$field = $value" >> "$USERS_FILE"
            fi
            print_success "Field $field added successfully"
        fi
    fi
}

# Function to change user password
change_password() {
    local username="$1"
    local new_password="$2"
    
    if [[ -z "$new_password" ]]; then
        print_error "New password is required"
        exit 1
    fi
    
    modify_user "$username" "password" "$new_password"
    print_success "Password for user $username changed successfully"
}

# Function to enable/disable user
toggle_user_status() {
    local username="$1"
    local action="$2"
    
    if [[ "$action" == "enable" ]]; then
        modify_user "$username" "status" "active"
        print_success "User $username enabled"
    elif [[ "$action" == "disable" ]]; then
        modify_user "$username" "status" "inactive"
        print_success "User $username disabled"
    else
        print_error "Invalid action. Use 'enable' or 'disable'"
        exit 1
    fi
}

# Function to backup users configuration
backup_users() {
    local backup_file="$USERS_FILE.backup.$(date +%Y%m%d_%H%M%S)"
    
    if [[ -f "$USERS_FILE" ]]; then
        cp "$USERS_FILE" "$backup_file"
        print_success "Users configuration backed up to $backup_file"
    else
        print_warning "No users configuration file to backup"
    fi
}

# Function to restore users configuration
restore_users() {
    local backup_file="$1"
    
    if [[ ! -f "$backup_file" ]]; then
        print_error "Backup file not found: $backup_file"
        exit 1
    fi
    
    # Create backup of current file
    if [[ -f "$USERS_FILE" ]]; then
        cp "$USERS_FILE" "$USERS_FILE.backup.before_restore.$(date +%Y%m%d_%H%M%S)"
    fi
    
    # Restore from backup
    cp "$backup_file" "$USERS_FILE"
    print_success "Users configuration restored from $backup_file"
}

# Function to display usage
usage() {
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  add <username> [password] [home_dir] [permissions]  Add a new user"
    echo "  remove <username> [--remove-files]                  Remove a user"
    echo "  list                                                 List all users"
    echo "  modify <username> <field> <value>                   Modify user field"
    echo "  password <username> <new_password>                  Change user password"
    echo "  enable <username>                                    Enable a user"
    echo "  disable <username>                                   Disable a user"
    echo "  backup                                               Backup users configuration"
    echo "  restore <backup_file>                                Restore from backup"
    echo ""
    echo "Options:"
    echo "  --remove-files                                       Remove user files when removing user"
    echo "  -h, --help                                           Display this help message"
    echo ""
    echo "Examples:"
    echo "  $0 add john secret123 /var/ftp/john read,write,upload"
    echo "  $0 remove john --remove-files"
    echo "  $0 list"
    echo "  $0 modify john permissions read,download"
    echo "  $0 password john newpass123"
    echo "  $0 enable john"
    echo "  $0 backup"
    echo "  $0 restore users.conf.backup.20231201_143022"
    echo ""
}

# Main script
main() {
    local command="$1"
    
    # Check if running as root
    check_root
    
    case $command in
        "add")
            if [[ $# -lt 2 ]]; then
                print_error "Username is required for add command"
                usage
                exit 1
            fi
            add_user "$2" "$3" "$4" "$5"
            ;;
        "remove")
            if [[ $# -lt 2 ]]; then
                print_error "Username is required for remove command"
                usage
                exit 1
            fi
            local remove_files="false"
            if [[ "$3" == "--remove-files" ]]; then
                remove_files="true"
            fi
            remove_user "$2" "$remove_files"
            ;;
        "list")
            list_users
            ;;
        "modify")
            if [[ $# -lt 4 ]]; then
                print_error "Username, field, and value are required for modify command"
                usage
                exit 1
            fi
            modify_user "$2" "$3" "$4"
            ;;
        "password")
            if [[ $# -lt 3 ]]; then
                print_error "Username and new password are required for password command"
                usage
                exit 1
            fi
            change_password "$2" "$3"
            ;;
        "enable")
            if [[ $# -lt 2 ]]; then
                print_error "Username is required for enable command"
                usage
                exit 1
            fi
            toggle_user_status "$2" "enable"
            ;;
        "disable")
            if [[ $# -lt 2 ]]; then
                print_error "Username is required for disable command"
                usage
                exit 1
            fi
            toggle_user_status "$2" "disable"
            ;;
        "backup")
            backup_users
            ;;
        "restore")
            if [[ $# -lt 2 ]]; then
                print_error "Backup file is required for restore command"
                usage
                exit 1
            fi
            restore_users "$2"
            ;;
        "-h"|"--help"|"help")
            usage
            ;;
        "")
            print_error "Command is required"
            usage
            exit 1
            ;;
        *)
            print_error "Unknown command: $command"
            usage
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
