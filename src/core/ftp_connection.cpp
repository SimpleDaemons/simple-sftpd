/*
 * Copyright 2024 SimpleDaemons
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "simple-sftpd/ftp_connection.hpp"
#include "simple-sftpd/logger.hpp"
#include "simple-sftpd/ftp_user_manager.hpp"
#include "simple-sftpd/ftp_user.hpp"
#include "simple-sftpd/ftp_server_config.hpp"
#include "simple-sftpd/ssl_context.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <dirent.h>
#include <errno.h>

namespace simple_sftpd {

FTPConnection::FTPConnection(int socket, std::shared_ptr<Logger> logger, std::shared_ptr<FTPServerConfig> config)
    : socket_(socket), logger_(logger), config_(config), active_(false),
      authenticated_(false), current_user_(nullptr), current_directory_("/"),
      ssl_enabled_(false), ssl_active_(false), ssl_(nullptr), data_ssl_(nullptr),
      passive_listen_socket_(-1), data_socket_(-1), transfer_type_("A"), protection_level_("C"),
      active_mode_port_(0), active_mode_enabled_(false) {
    user_manager_ = std::make_shared<FTPUserManager>(logger_);
    
    // Add default test user for development/testing
    auto test_user = std::make_shared<FTPUser>("test", "test", "/tmp");
    user_manager_->addUser(test_user);
    
    // Add anonymous user if allowed
    if (config->security.allow_anonymous) {
        auto anon_user = std::make_shared<FTPUser>("anonymous", "", "/tmp");
        user_manager_->addUser(anon_user);
    }
}

FTPConnection::~FTPConnection() {
    stop();
}

void FTPConnection::start() {
    if (active_) {
        return;
    }
    
    active_ = true;
    client_thread_ = std::thread(&FTPConnection::handleClient, this);
    logger_->info("FTP connection started");
}

void FTPConnection::stop() {
    if (!active_) {
        return;
    }
    
    active_ = false;
    
    // Cleanup SSL
    if (ssl_context_ && ssl_) {
        ssl_context_->shutdownSSL(ssl_);
        ssl_context_->freeSSL(ssl_);
        ssl_ = nullptr;
    }
    if (ssl_context_ && data_ssl_) {
        ssl_context_->shutdownSSL(data_ssl_);
        ssl_context_->freeSSL(data_ssl_);
        data_ssl_ = nullptr;
    }
    
    closeDataSocket();
    if (socket_ >= 0) {
        close(socket_);
        socket_ = -1;
    }
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
    logger_->info("FTP connection stopped");
}

bool FTPConnection::isActive() const {
    return active_;
}

void FTPConnection::handleClient() {
    // Send welcome message
    sendResponse("220 Welcome to Simple Secure FTP Daemon");
    
    std::string line;
    while (active_ && (line = readLine()).length() > 0) {
        if (line.empty()) {
            continue;
        }
        
        // Parse command
        std::istringstream iss(line);
        std::string command;
        std::string argument;
        
        iss >> command;
        std::getline(iss, argument);
        
        // Trim whitespace from argument
        argument.erase(0, argument.find_first_not_of(" \t"));
        argument.erase(argument.find_last_not_of(" \t") + 1);
        
        // Convert command to uppercase
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);
        
        logger_->debug("Received command: " + command + (argument.empty() ? "" : " " + argument));
        
        // Handle commands
        if (command == "USER") {
            handleUSER(argument);
        } else if (command == "PASS") {
            handlePASS(argument);
        } else if (command == "QUIT") {
            handleQUIT();
            break;
        } else if (command == "NOOP") {
            sendResponse("200 NOOP command successful");
        } else if (command == "SYST") {
            sendResponse("215 UNIX Type: L8");
        } else if (command == "FEAT") {
            sendResponse("211-Features:");
            if (ssl_enabled_) {
                sendResponse(" AUTH TLS");
                sendResponse(" PBSZ");
                sendResponse(" PROT");
            }
            sendResponse("211 End");
        } else if (command == "AUTH") {
            handleAUTH(argument);
        } else if (command == "PBSZ") {
            handlePBSZ(argument);
        } else if (command == "PROT") {
            handlePROT(argument);
        } else if (authenticated_) {
            // Commands that require authentication
            if (command == "PWD" || command == "XPWD") {
                handlePWD();
            } else if (command == "CWD" || command == "XCWD") {
                handleCWD(argument);
            } else if (command == "LIST" || command == "NLST") {
                handleLIST(argument);
            } else if (command == "PASV") {
                handlePASV();
            } else if (command == "TYPE") {
                handleTYPE(argument);
            } else if (command == "SIZE") {
                handleSIZE(argument);
            } else if (command == "RETR") {
                handleRETR(argument);
            } else if (command == "STOR") {
                handleSTOR(argument);
            } else if (command == "DELE") {
                handleDELE(argument);
            } else if (command == "MKD" || command == "XMKD") {
                handleMKD(argument);
            } else if (command == "RMD" || command == "XRMD") {
                handleRMD(argument);
            } else {
                sendResponse("502 Command not implemented");
            }
        } else {
            sendResponse("530 Please login with USER and PASS");
        }
    }
    
    active_ = false;
}

void FTPConnection::sendResponse(const std::string& response) {
    if (socket_ < 0) {
        return;
    }
    
    std::string full_response = response + "\r\n";
    ssize_t sent;
    
    if (ssl_active_ && ssl_ && ssl_context_) {
        sent = ssl_context_->writeSSL(ssl_, full_response.c_str(), full_response.length());
    } else {
        sent = send(socket_, full_response.c_str(), full_response.length(), 0);
    }
    
    if (sent < 0) {
        logger_->error("Failed to send response: " + std::string(strerror(errno)));
        active_ = false;
    } else {
        logger_->debug("Sent: " + response);
    }
}

std::string FTPConnection::readLine() {
    if (socket_ < 0) {
        return "";
    }

    std::string line;
    char buffer[1];
    
    while (active_) {
        ssize_t received;
        
        if (ssl_active_ && ssl_ && ssl_context_) {
            received = ssl_context_->readSSL(ssl_, buffer, 1);
        } else {
            received = recv(socket_, buffer, 1, 0);
        }
        
        if (received <= 0) {
            if (received == 0) {
                // Connection closed
                active_ = false;
            }
            break;
        }
        
        if (buffer[0] == '\n') {
            // Remove \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            break;
        }
        
        line += buffer[0];
        
        // Safety limit
        if (line.length() > 1024) {
            break;
        }
    }
    
    return line;
}

// FTP Command Handlers
void FTPConnection::handleUSER(const std::string& username) {
    username_ = username;
    authenticated_ = false;
    sendResponse("331 User name okay, need password");
}

void FTPConnection::handlePASS(const std::string& password) {
    if (username_.empty()) {
        sendResponse("503 Login with USER first");
        return;
    }
    
    current_user_ = user_manager_->getUser(username_);
    if (current_user_ && current_user_->authenticate(password)) {
        authenticated_ = true;
        current_directory_ = current_user_->getHomeDirectory();
        // Ensure current directory is within home
        if (!isPathWithinHome(current_directory_)) {
            current_directory_ = current_user_->getHomeDirectory();
        }
        sendResponse("230 User logged in, proceed");
        logger_->info("User " + username_ + " logged in");
    } else {
        sendResponse("530 Login incorrect");
        logger_->warn("Failed login attempt for user: " + username_);
    }
}

void FTPConnection::handleQUIT() {
    sendResponse("221 Goodbye");
    active_ = false;
}

void FTPConnection::handlePWD() {
    sendResponse("257 \"" + current_directory_ + "\"");
}

void FTPConnection::handleCWD(const std::string& path) {
    std::string new_path = resolvePath(path);
    
    if (!validatePath(new_path)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
        current_directory_ = new_path;
        sendResponse("250 CWD command successful");
    } else {
        sendResponse("550 Failed to change directory");
    }
}

void FTPConnection::handleLIST(const std::string& path) {
    if (!hasPermission("list", "")) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string list_path = path.empty() ? current_directory_ : resolvePath(path);
    
    if (!validatePath(list_path)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (!std::filesystem::exists(list_path)) {
        sendResponse("550 File or directory not found");
        return;
    }
    
    sendResponse("150 Opening ASCII mode data connection for file list");
    
    // Accept data connection
    int data_fd = acceptDataConnection();
    if (data_fd < 0) {
        sendResponse("425 Can't open data connection");
        return;
    }
    
    std::string listing;
    if (std::filesystem::is_directory(list_path)) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(list_path)) {
                std::string filename = entry.path().filename().string();
                auto status = entry.status();
                std::string perms = std::filesystem::is_directory(status) ? "d" : "-";
                perms += "rw-rw-rw-";
                
                auto size = std::filesystem::is_directory(status) ? 0 : std::filesystem::file_size(entry.path());
                (void)std::filesystem::last_write_time(entry.path()); // Suppress unused warning
                
                listing += perms + " 1 owner group " + std::to_string(size) + " " + filename + "\r\n";
            }
        } catch (const std::exception& e) {
            logger_->error("Error listing directory: " + std::string(e.what()));
            close(data_fd);
            sendResponse("550 Error listing directory");
            return;
        }
    } else {
        // Single file
        listing += "-rw-rw-rw- 1 owner group " + 
                  std::to_string(std::filesystem::file_size(list_path)) + " " +
                  std::filesystem::path(list_path).filename().string() + "\r\n";
    }
    
    // Send listing through data connection
    send(data_fd, listing.c_str(), listing.length(), 0);
    close(data_fd);
    sendResponse("226 Transfer complete");
}

void FTPConnection::handlePASV() {
    // Disable active mode if it was enabled
    active_mode_enabled_ = false;
    closeDataSocket(); // Close any existing passive socket
    
    int port = createPassiveDataSocket();
    if (port < 0) {
        sendResponse("425 Can't open passive connection");
        return;
    }
    
    std::string response = formatPassiveResponse(port);
    sendResponse(response);
    logger_->debug("Passive mode enabled on port " + std::to_string(port));
}

void FTPConnection::handlePORT(const std::string& address_port) {
    // Parse PORT command: PORT h1,h2,h3,h4,p1,p2
    // Example: PORT 192,168,1,100,4,28
    std::vector<int> parts;
    std::istringstream iss(address_port);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        try {
            parts.push_back(std::stoi(token));
        } catch (...) {
            sendResponse("501 Invalid PORT command format");
            return;
        }
    }
    
    if (parts.size() != 6) {
        sendResponse("501 Invalid PORT command format");
        return;
    }
    
    // Build IP address
    active_mode_ip_ = std::to_string(parts[0]) + "." +
                      std::to_string(parts[1]) + "." +
                      std::to_string(parts[2]) + "." +
                      std::to_string(parts[3]);
    
    // Calculate port: p1 * 256 + p2
    active_mode_port_ = parts[4] * 256 + parts[5];
    
    if (active_mode_port_ < 1024 || active_mode_port_ > 65535) {
        sendResponse("501 Invalid port number");
        return;
    }
    
    // Close any existing passive connection
    closeDataSocket();
    active_mode_enabled_ = true;
    
    logger_->info("Active mode enabled: " + active_mode_ip_ + ":" + std::to_string(active_mode_port_));
    sendResponse("200 PORT command successful");
}

void FTPConnection::handleTYPE(const std::string& type) {
    if (type == "A" || type == "I") {
        transfer_type_ = type;
        sendResponse("200 Type set to " + type);
    } else {
        sendResponse("504 Command not implemented for that parameter");
    }
}

void FTPConnection::handleSIZE(const std::string& filename) {
    std::string filepath = resolvePath(filename);
    
    if (std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath)) {
        auto size = std::filesystem::file_size(filepath);
        sendResponse("213 " + std::to_string(size));
    } else {
        sendResponse("550 File not found");
    }
}

void FTPConnection::handleRETR(const std::string& filename) {
    if (!hasPermission("read", filename)) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string filepath = resolvePath(filename);
    
    if (!validatePath(filepath)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
        sendResponse("550 File not found");
        return;
    }
    
    sendResponse("150 Opening " + transfer_type_ + " mode data connection");
    
    // Accept data connection
    int data_fd = acceptDataConnection();
    if (data_fd < 0) {
        sendResponse("425 Can't open data connection");
        return;
    }
    
    // Open file
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        close(data_fd);
        sendResponse("550 Failed to open file");
        return;
    }
    
    // Transfer file
    char buffer[8192];
    size_t total_bytes = 0;
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        ssize_t sent = send(data_fd, buffer, bytes_read, 0);
        if (sent < 0) {
            logger_->error("Error sending file data: " + std::string(strerror(errno)));
            file.close();
            close(data_fd);
            sendResponse("426 Connection closed, transfer aborted");
            return;
        }
        total_bytes += sent;
    }
    
    file.close();
    close(data_fd);
    logger_->info("File transfer complete: " + filename + " (" + std::to_string(total_bytes) + " bytes)");
    sendResponse("226 Transfer complete");
}

void FTPConnection::handleSTOR(const std::string& filename) {
    if (!hasPermission("write", filename)) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string filepath = resolvePath(filename);
    
    if (!validatePath(filepath)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    sendResponse("150 Opening " + transfer_type_ + " mode data connection");
    
    // Accept data connection
    int data_fd = acceptDataConnection();
    if (data_fd < 0) {
        sendResponse("425 Can't open data connection");
        return;
    }
    
    // Create parent directory if needed
    std::filesystem::path file_path(filepath);
    if (file_path.has_parent_path()) {
        std::filesystem::create_directories(file_path.parent_path());
    }
    
    // Open file for writing
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        close(data_fd);
        sendResponse("550 Failed to create file");
        return;
    }
    
    // Receive file
    char buffer[8192];
    size_t total_bytes = 0;
    while (true) {
        ssize_t received = recv(data_fd, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            break; // Connection closed or error
        }
        file.write(buffer, received);
        total_bytes += received;
    }
    
    file.close();
    close(data_fd);
    logger_->info("File upload complete: " + filename + " (" + std::to_string(total_bytes) + " bytes)");
    sendResponse("226 Transfer complete");
}

void FTPConnection::handleDELE(const std::string& filename) {
    if (!hasPermission("write", filename)) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string filepath = resolvePath(filename);
    
    if (!validatePath(filepath)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath)) {
        if (std::filesystem::remove(filepath)) {
            sendResponse("250 DELE command successful");
        } else {
            sendResponse("550 Failed to delete file");
        }
    } else {
        sendResponse("550 File not found");
    }
}

void FTPConnection::handleMKD(const std::string& dirname) {
    if (!hasPermission("write", dirname)) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string dirpath = resolvePath(dirname);
    
    if (!validatePath(dirpath)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (std::filesystem::create_directory(dirpath)) {
        sendResponse("257 \"" + dirpath + "\" created");
    } else {
        sendResponse("550 Failed to create directory");
    }
}

void FTPConnection::handleRMD(const std::string& dirname) {
    if (!hasPermission("write", dirname)) {
        sendResponse("550 Permission denied");
        return;
    }
    
    std::string dirpath = resolvePath(dirname);
    
    if (!validatePath(dirpath)) {
        sendResponse("550 Invalid path");
        return;
    }
    
    if (std::filesystem::exists(dirpath) && std::filesystem::is_directory(dirpath)) {
        if (std::filesystem::remove(dirpath)) {
            sendResponse("250 RMD command successful");
        } else {
            sendResponse("550 Failed to remove directory");
        }
    } else {
        sendResponse("550 Directory not found");
    }
}

std::string FTPConnection::resolvePath(const std::string& path) {
    if (path.empty()) {
        return current_directory_;
    }
    
    std::string resolved;
    if (path[0] == '/') {
        // Absolute path - resolve relative to user's home directory
        if (current_user_) {
            resolved = current_user_->getHomeDirectory() + path;
        } else {
            resolved = path;
        }
    } else {
        // Relative path
        resolved = current_directory_ + "/" + path;
    }
    
    // Normalize path (remove .. and .)
    std::filesystem::path p(resolved);
    try {
        return std::filesystem::canonical(p).string();
    } catch (const std::exception&) {
        // If canonical fails, at least normalize
        return p.lexically_normal().string();
    }
}

bool FTPConnection::validatePath(const std::string& path) {
    if (!current_user_) {
        return false;
    }
    
    std::string home = current_user_->getHomeDirectory();
    std::string resolved = resolvePath(path);
    
    // Check if resolved path is within home directory
    return isPathWithinHome(resolved);
}

bool FTPConnection::isPathWithinHome(const std::string& path) {
    if (!current_user_) {
        return false;
    }
    
    std::string home = current_user_->getHomeDirectory();
    std::filesystem::path path_p(path);
    std::filesystem::path home_p(home);
    
    try {
        std::filesystem::path canonical_path = std::filesystem::canonical(path_p);
        std::filesystem::path canonical_home = std::filesystem::canonical(home_p);
        
        // Check if canonical_path starts with canonical_home
        auto it = canonical_path.begin();
        auto home_it = canonical_home.begin();
        
        while (home_it != canonical_home.end()) {
            if (it == canonical_path.end() || *it != *home_it) {
                return false;
            }
            ++it;
            ++home_it;
        }
        return true;
    } catch (const std::exception&) {
        // If canonical fails, do simple string comparison
        return path.find(home) == 0;
    }
}

bool FTPConnection::hasPermission(const std::string& operation, const std::string& path) {
    if (!current_user_) {
        return false;
    }
    
    // Basic permission check - delegate to FTPUser
    return current_user_->hasPermission(operation, path);
}

int FTPConnection::createPassiveDataSocket() {
    closeDataSocket();
    
    // Create socket
    passive_listen_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (passive_listen_socket_ < 0) {
        logger_->error("Failed to create passive socket: " + std::string(strerror(errno)));
        return -1;
    }
    
    // Set socket options
    int reuse = 1;
    setsockopt(passive_listen_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // Bind to any available port in the range
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int port_range_start = config_->connection.passive_port_range_start;
    int port_range_end = config_->connection.passive_port_range_end;
    
    for (int port = port_range_start; port <= port_range_end; ++port) {
        addr.sin_port = htons(port);
        if (bind(passive_listen_socket_, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            if (listen(passive_listen_socket_, 1) == 0) {
                logger_->debug("Passive socket listening on port " + std::to_string(port));
                return port;
            }
        }
    }
    
    close(passive_listen_socket_);
    passive_listen_socket_ = -1;
    logger_->error("Failed to bind passive socket in port range");
    return -1;
}

int FTPConnection::acceptDataConnection() {
    std::lock_guard<std::mutex> lock(data_socket_mutex_);
    
    if (passive_listen_socket_ < 0) {
        logger_->error("No passive socket available");
        return -1;
    }
    
    // Set socket to non-blocking for timeout
    int flags = fcntl(passive_listen_socket_, F_GETFL, 0);
    fcntl(passive_listen_socket_, F_SETFL, flags | O_NONBLOCK);
    
    // Wait for connection with timeout
    struct timeval timeout;
    timeout.tv_sec = 10; // 10 second timeout
    timeout.tv_usec = 0;
    
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(passive_listen_socket_, &read_fds);
    
    int result = select(passive_listen_socket_ + 1, &read_fds, nullptr, nullptr, &timeout);
    if (result <= 0) {
        logger_->error("Timeout waiting for data connection");
        return -1;
    }
    
    // Accept connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    data_socket_ = accept(passive_listen_socket_, (struct sockaddr*)&client_addr, &client_len);
    
    if (data_socket_ < 0) {
        logger_->error("Failed to accept data connection: " + std::string(strerror(errno)));
        return -1;
    }
    
    // Restore blocking mode
    fcntl(passive_listen_socket_, F_SETFL, flags);
    
    logger_->debug("Data connection accepted from " + std::string(inet_ntoa(client_addr.sin_addr)));
    return data_socket_;
}

void FTPConnection::closeDataSocket() {
    std::lock_guard<std::mutex> lock(data_socket_mutex_);
    
    if (data_socket_ >= 0) {
        close(data_socket_);
        data_socket_ = -1;
    }
    
    if (passive_listen_socket_ >= 0) {
        close(passive_listen_socket_);
        passive_listen_socket_ = -1;
    }
}

std::string FTPConnection::formatPassiveResponse(int port) {
    // Get server IP (simplified - use localhost for now)
    // In production, should get actual server IP
    std::string ip = "127,0,0,1";
    
    int p1 = port / 256;
    int p2 = port % 256;
    
    return "227 Entering Passive Mode (" + ip + "," + std::to_string(p1) + "," + std::to_string(p2) + ")";
}

// SSL/TLS Command Handlers
void FTPConnection::handleAUTH(const std::string& method) {
    std::string method_upper = method;
    std::transform(method_upper.begin(), method_upper.end(), method_upper.begin(), ::toupper);
    
    if (method_upper == "TLS" || method_upper == "SSL") {
        if (!ssl_enabled_ || !ssl_context_) {
            sendResponse("534 SSL/TLS not available");
            return;
        }
        
        if (ssl_active_) {
            sendResponse("534 SSL/TLS already active");
            return;
        }
        
        sendResponse("234 AUTH TLS successful");
        
        // Upgrade connection to SSL
        if (!upgradeToSSL()) {
            logger_->error("Failed to upgrade connection to SSL");
            active_ = false;
        } else {
            ssl_active_ = true;
            logger_->info("Connection upgraded to SSL/TLS");
        }
    } else {
        sendResponse("504 Unsupported AUTH method");
    }
}

void FTPConnection::handlePBSZ(const std::string& size) {
    (void)size; // PBSZ size parameter is ignored for TLS
    if (!ssl_active_) {
        sendResponse("503 PBSZ command only valid in secure mode");
        return;
    }
    
    // PBSZ (Protection Buffer Size) - always 0 for TLS
    sendResponse("200 PBSZ=0");
}

void FTPConnection::handlePROT(const std::string& level) {
    if (!ssl_active_) {
        sendResponse("503 PROT command only valid in secure mode");
        return;
    }
    
    std::string level_upper = level;
    std::transform(level_upper.begin(), level_upper.end(), level_upper.begin(), ::toupper);
    
    if (level_upper == "C" || level_upper == "CLEAR") {
        protection_level_ = "C";
        sendResponse("200 Protection level set to Clear");
    } else if (level_upper == "P" || level_upper == "PRIVATE") {
        protection_level_ = "P";
        sendResponse("200 Protection level set to Private");
    } else if (level_upper == "S" || level_upper == "SAFE") {
        protection_level_ = "S";
        sendResponse("200 Protection level set to Safe");
    } else if (level_upper == "E" || level_upper == "CONFIDENTIAL") {
        protection_level_ = "E";
        sendResponse("200 Protection level set to Confidential");
    } else {
        sendResponse("504 Unsupported protection level");
    }
}

bool FTPConnection::upgradeToSSL() {
    if (!ssl_context_ || !ssl_enabled_) {
        return false;
    }
    
    ssl_ = ssl_context_->createSSL(socket_);
    if (!ssl_) {
        logger_->error("Failed to create SSL connection: " + ssl_context_->getLastError());
        return false;
    }
    
    if (!ssl_context_->acceptSSL(ssl_)) {
        logger_->error("SSL handshake failed: " + ssl_context_->getLastError());
        ssl_context_->freeSSL(ssl_);
        ssl_ = nullptr;
        return false;
    }
    
    return true;
}

} // namespace simple_sftpd
