#!/bin/bash

# Simple FTP Daemon - SSL Setup Script
# This script helps set up SSL certificates for the FTP server

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SSL_DIR="/etc/ssl/ssftpd"
CONFIG_DIR="/etc/ssftpd"
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

# Function to detect OS
detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [[ -f /etc/lsb-release ]]; then
        . /etc/lsb-release
        OS=$DISTRIB_ID
        VER=$DISTRIB_RELEASE
    elif [[ -f /etc/debian_version ]]; then
        OS=Debian
        VER=$(cat /etc/debian_version)
    elif [[ -f /etc/SuSe-release ]]; then
        OS=SuSE
    elif [[ -f /etc/redhat-release ]]; then
        OS=RedHat
    else
        OS=$(uname -s)
        VER=$(uname -r)
    fi
    
    print_status "Detected OS: $OS $VER"
}

# Function to install dependencies
install_dependencies() {
    print_status "Installing SSL dependencies..."
    
    case $OS in
        *Ubuntu*|*Debian*)
            apt-get update
            apt-get install -y openssl ca-certificates
            ;;
        *CentOS*|*RedHat*|*Fedora*)
            yum install -y openssl ca-certificates
            ;;
        *SUSE*|*openSUSE*)
            zypper install -y openssl ca-certificates
            ;;
        *Darwin*|*macOS*)
            # macOS usually has OpenSSL installed
            if ! command -v openssl &> /dev/null; then
                print_warning "OpenSSL not found. Please install via Homebrew: brew install openssl"
                exit 1
            fi
            ;;
        *)
            print_warning "Unknown OS. Please install OpenSSL manually."
            ;;
    esac
    
    print_success "Dependencies installed"
}

# Function to create SSL directory structure
create_ssl_dirs() {
    print_status "Creating SSL directory structure..."
    
    mkdir -p "$SSL_DIR"
    mkdir -p "$SSL_DIR/private"
    mkdir -p "$SSL_DIR/certs"
    mkdir -p "$SSL_DIR/ca"
    
    # Set proper permissions
    chown -R $FTP_USER:$FTP_GROUP "$SSL_DIR"
    chmod 700 "$SSL_DIR/private"
    chmod 755 "$SSL_DIR/certs"
    chmod 755 "$SSL_DIR/ca"
    
    print_success "SSL directories created"
}

# Function to generate self-signed certificate
generate_self_signed() {
    local domain="$1"
    local days="$2"
    
    print_status "Generating self-signed certificate for $domain (valid for $days days)..."
    
    # Create OpenSSL configuration
    cat > "$SSL_DIR/openssl.conf" << EOF
[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
prompt = no

[req_distinguished_name]
C = US
ST = State
L = City
O = Organization
OU = IT Department
CN = $domain

[v3_req]
keyUsage = keyEncipherment, dataEncipherment
extendedKeyUsage = serverAuth
subjectAltName = @alt_names

[alt_names]
DNS.1 = $domain
DNS.2 = *.$domain
IP.1 = 127.0.0.1
EOF
    
    # Generate private key
    openssl genrsa -out "$SSL_DIR/private/$domain.key" 2048
    
    # Generate certificate signing request
    openssl req -new -key "$SSL_DIR/private/$domain.key" \
        -out "$SSL_DIR/$domain.csr" \
        -config "$SSL_DIR/openssl.conf"
    
    # Generate self-signed certificate
    openssl x509 -req -in "$SSL_DIR/$domain.csr" \
        -signkey "$SSL_DIR/private/$domain.key" \
        -out "$SSL_DIR/certs/$domain.crt" \
        -days "$days" \
        -extensions v3_req \
        -extfile "$SSL_DIR/openssl.conf"
    
    # Set proper permissions
    chown $FTP_USER:$FTP_GROUP "$SSL_DIR/private/$domain.key"
    chown $FTP_USER:$FTP_GROUP "$SSL_DIR/certs/$domain.crt"
    chmod 600 "$SSL_DIR/private/$domain.key"
    chmod 644 "$SSL_DIR/certs/$domain.crt"
    
    # Clean up
    rm "$SSL_DIR/openssl.conf"
    rm "$SSL_DIR/$domain.csr"
    
    print_success "Self-signed certificate generated"
}

# Function to generate Let's Encrypt certificate
generate_letsencrypt() {
    local domain="$1"
    
    print_status "Generating Let's Encrypt certificate for $domain..."
    
    # Check if certbot is available
    if ! command -v certbot &> /dev/null; then
        print_error "certbot not found. Please install it first:"
        case $OS in
            *Ubuntu*|*Debian*)
                echo "  apt-get install certbot"
                ;;
            *CentOS*|*RedHat*|*Fedora*)
                echo "  yum install certbot"
                ;;
            *SUSE*|*openSUSE*)
                echo "  zypper install certbot"
                ;;
            *Darwin*|*macOS*)
                echo "  brew install certbot"
                ;;
        esac
        exit 1
    fi
    
    # Generate certificate using certbot
    certbot certonly --standalone \
        --cert-name "$domain" \
        --email admin@$domain \
        --agree-tos \
        --non-interactive \
        -d "$domain"
    
    # Copy certificates to our SSL directory
    cp "/etc/letsencrypt/live/$domain/fullchain.pem" "$SSL_DIR/certs/$domain.crt"
    cp "/etc/letsencrypt/live/$domain/privkey.pem" "$SSL_DIR/private/$domain.key"
    
    # Set proper permissions
    chown $FTP_USER:$FTP_GROUP "$SSL_DIR/private/$domain.key"
    chown $FTP_USER:$FTP_GROUP "$SSL_DIR/certs/$domain.crt"
    chmod 600 "$SSL_DIR/private/$domain.key"
    chmod 644 "$SSL_DIR/certs/$domain.crt"
    
    print_success "Let's Encrypt certificate generated"
}

# Function to update configuration
update_config() {
    local domain="$1"
    local cert_file="$SSL_DIR/certs/$domain.crt"
    local key_file="$SSL_DIR/private/$domain.key"
    
    print_status "Updating configuration for $domain..."
    
    # Check if configuration file exists
    if [[ ! -f "$CONFIG_DIR/ssftpd.conf" ]]; then
        print_warning "Configuration file not found at $CONFIG_DIR/ssftpd.conf"
        print_status "Please create it manually with the following SSL settings:"
        echo ""
        echo "[ssl]"
        echo "enabled = true"
        echo "certificate_file = $cert_file"
        echo "private_key_file = $key_file"
        echo "require_client_cert = false"
        echo "verify_peer = false"
        echo ""
        return
    fi
    
    # Update SSL configuration in the config file
    if grep -q "\[ssl\]" "$CONFIG_DIR/ssftpd.conf"; then
        # SSL section exists, update it
        sed -i "s|^certificate_file = .*|certificate_file = $cert_file|" "$CONFIG_DIR/ssftpd.conf"
        sed -i "s|^private_key_file = .*|private_key_file = $key_file|" "$CONFIG_DIR/ssftpd.conf"
        sed -i "s|^enabled = .*|enabled = true|" "$CONFIG_DIR/ssftpd.conf"
    else
        # SSL section doesn't exist, add it
        cat >> "$CONFIG_DIR/ssftpd.conf" << EOF

[ssl]
enabled = true
certificate_file = $cert_file
private_key_file = $key_file
require_client_cert = false
verify_peer = false
EOF
    fi
    
    print_success "Configuration updated"
}

# Function to create renewal script for Let's Encrypt
create_renewal_script() {
    local domain="$1"
    
    print_status "Creating renewal script for $domain..."
    
    cat > "$SSL_DIR/renew-$domain.sh" << EOF
#!/bin/bash
# Renew Let's Encrypt certificate for $domain

set -e

# Renew certificate
certbot renew --cert-name $domain

# Copy renewed certificates
cp "/etc/letsencrypt/live/$domain/fullchain.pem" "$SSL_DIR/certs/$domain.crt"
cp "/etc/letsencrypt/live/$domain/privkey.pem" "$SSL_DIR/private/$domain.key"

# Set proper permissions
chown $FTP_USER:$FTP_GROUP "$SSL_DIR/private/$domain.key"
chown $FTP_USER:$FTP_GROUP "$SSL_DIR/certs/$domain.crt"
chmod 600 "$SSL_DIR/private/$domain.key"
chmod 644 "$SSL_DIR/certs/$domain.crt"

# Reload FTP server if running
if systemctl is-active --quiet ssftpd; then
    systemctl reload ssftpd
fi

echo "Certificate renewed successfully"
EOF
    
    chmod +x "$SSL_DIR/renew-$domain.sh"
    
    print_success "Renewal script created at $SSL_DIR/renew-$domain.sh"
}

# Function to display usage
usage() {
    echo "Usage: $0 [OPTIONS] <domain>"
    echo ""
    echo "Options:"
    echo "  -s, --self-signed    Generate self-signed certificate (default)"
    echo "  -l, --letsencrypt    Generate Let's Encrypt certificate"
    echo "  -d, --days <days>    Validity period for self-signed cert (default: 365)"
    echo "  -h, --help           Display this help message"
    echo ""
    echo "Examples:"
    echo "  $0 ftp.example.com                    # Generate self-signed cert"
    echo "  $0 -l ftp.example.com                 # Generate Let's Encrypt cert"
    echo "  $0 -d 730 ftp.example.com             # Self-signed cert valid for 2 years"
    echo ""
}

# Main script
main() {
    local cert_type="self-signed"
    local days=365
    local domain=""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -s|--self-signed)
                cert_type="self-signed"
                shift
                ;;
            -l|--letsencrypt)
                cert_type="letsencrypt"
                shift
                ;;
            -d|--days)
                days="$2"
                shift 2
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
                domain="$1"
                shift
                ;;
        esac
    done
    
    # Check if domain is provided
    if [[ -z "$domain" ]]; then
        print_error "Domain name is required"
        usage
        exit 1
    fi
    
    # Check if running as root
    check_root
    
    # Detect OS
    detect_os
    
    # Install dependencies
    install_dependencies
    
    # Create SSL directories
    create_ssl_dirs
    
    # Generate certificate based on type
    case $cert_type in
        "self-signed")
            generate_self_signed "$domain" "$days"
            ;;
        "letsencrypt")
            generate_letsencrypt "$domain"
            ;;
    esac
    
    # Update configuration
    update_config "$domain"
    
    # Create renewal script for Let's Encrypt
    if [[ "$cert_type" == "letsencrypt" ]]; then
        create_renewal_script "$domain"
    fi
    
    print_success "SSL setup completed for $domain"
    echo ""
    echo "Next steps:"
    echo "1. Restart the FTP server: systemctl restart ssftpd"
    echo "2. Test the connection: ftp -p $domain"
    echo ""
    
    if [[ "$cert_type" == "letsencrypt" ]]; then
        echo "Note: Let's Encrypt certificates expire after 90 days."
        echo "Set up automatic renewal with cron:"
        echo "  0 12 * * * $SSL_DIR/renew-$domain.sh"
    fi
}

# Run main function with all arguments
main "$@"
