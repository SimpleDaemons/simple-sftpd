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
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <dirent.h>

namespace simple_sftpd {

FTPConnection::FTPConnection(int socket, std::shared_ptr<Logger> logger, std::shared_ptr<FTPServerConfig> config)
    : socket_(socket), logger_(logger), config_(config), active_(false),
      authenticated_(false), current_user_(nullptr), current_directory_("/") {
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
            sendResponse("211 End");
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
    ssize_t sent = send(socket_, full_response.c_str(), full_response.length(), 0);
    
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
        ssize_t received = recv(socket_, buffer, 1, 0);
        
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
    
    if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
        current_directory_ = new_path;
        sendResponse("250 CWD command successful");
    } else {
        sendResponse("550 Failed to change directory");
    }
}

void FTPConnection::handleLIST(const std::string& path) {
    std::string list_path = path.empty() ? current_directory_ : resolvePath(path);
    
    if (!std::filesystem::exists(list_path)) {
        sendResponse("550 File or directory not found");
        return;
    }
    
    sendResponse("150 Opening ASCII mode data connection for file list");
    
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
        }
    } else {
        // Single file
        listing += "-rw-rw-rw- 1 owner group " + 
                  std::to_string(std::filesystem::file_size(list_path)) + " " +
                  std::filesystem::path(list_path).filename().string() + "\r\n";
    }
    
    // Send listing (simplified - in real FTP, this would go through data connection)
    sendResponse(listing);
    sendResponse("226 Transfer complete");
}

void FTPConnection::handlePASV() {
    // Simplified - in real implementation, would set up passive mode data connection
    sendResponse("227 Entering Passive Mode (127,0,0,1,4,28)");
}

void FTPConnection::handleTYPE(const std::string& type) {
    if (type == "A" || type == "I") {
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
    std::string filepath = resolvePath(filename);
    
    if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
        sendResponse("550 File not found");
        return;
    }
    
    sendResponse("150 Opening data connection");
    
    // Simplified - in real implementation, would send file through data connection
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        // For now, just acknowledge
        file.close();
        sendResponse("226 Transfer complete");
    } else {
        sendResponse("550 Failed to open file");
    }
}

void FTPConnection::handleSTOR(const std::string& filename) {
    std::string filepath = resolvePath(filename);
    
    sendResponse("150 Opening data connection");
    
    // Simplified - in real implementation, would receive file through data connection
    std::ofstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        file.close();
        sendResponse("226 Transfer complete");
    } else {
        sendResponse("550 Failed to create file");
    }
}

void FTPConnection::handleDELE(const std::string& filename) {
    std::string filepath = resolvePath(filename);
    
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
    std::string dirpath = resolvePath(dirname);
    
    if (std::filesystem::create_directory(dirpath)) {
        sendResponse("257 \"" + dirpath + "\" created");
    } else {
        sendResponse("550 Failed to create directory");
    }
}

void FTPConnection::handleRMD(const std::string& dirname) {
    std::string dirpath = resolvePath(dirname);
    
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
    std::string resolved;
    
    if (path.empty()) {
        return current_directory_;
    }
    
    if (path[0] == '/') {
        // Absolute path
        resolved = path;
    } else {
        // Relative path
        resolved = current_directory_ + "/" + path;
    }
    
    // Normalize path
    std::filesystem::path p(resolved);
    return std::filesystem::canonical(p).string();
}

} // namespace simple_sftpd
