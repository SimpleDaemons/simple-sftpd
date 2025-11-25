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

#include "simple-sftpd/ftp_server.hpp"
#include "simple-sftpd/logger.hpp"
#include "simple-sftpd/ftp_connection_manager.hpp"
#include "simple-sftpd/ftp_connection.hpp"
#include "simple-sftpd/ftp_server_config.hpp"
#include "simple-sftpd/ftp_user_manager.hpp"
#include "simple-sftpd/ftp_user.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

namespace simple_sftpd {

FTPServer::FTPServer(std::shared_ptr<FTPServerConfig> config)
    : config_(config), running_(false), server_socket_(-1) {
    logger_ = std::make_shared<Logger>("", LogLevel::INFO, true, false);
    connection_manager_ = std::make_shared<FTPConnectionManager>(config, logger_);
}

FTPServer::~FTPServer() {
    stop();
}

bool FTPServer::start() {
    if (running_) {
        return true;
    }
    
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        logger_->error("Failed to create socket: " + std::string(strerror(errno)));
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        logger_->error("Failed to set socket options: " + std::string(strerror(errno)));
        close(server_socket_);
        server_socket_ = -1;
        return false;
    }
    
    // Set non-blocking mode
    int flags = fcntl(server_socket_, F_GETFL, 0);
    fcntl(server_socket_, F_SETFL, flags | O_NONBLOCK);
    
    // Bind socket
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(config_->connection.bind_address.c_str());
    addr.sin_port = htons(config_->connection.bind_port);
    
    if (bind(server_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        logger_->error("Failed to bind socket: " + std::string(strerror(errno)));
        close(server_socket_);
        server_socket_ = -1;
        return false;
    }
    
    // Listen
    if (listen(server_socket_, config_->connection.max_connections) < 0) {
        logger_->error("Failed to listen on socket: " + std::string(strerror(errno)));
        close(server_socket_);
        server_socket_ = -1;
        return false;
    }
    
    // Start connection manager
    if (!connection_manager_->start()) {
        logger_->error("Failed to start connection manager");
        close(server_socket_);
        server_socket_ = -1;
        return false;
    }
    
    running_ = true;
    server_thread_ = std::thread(&FTPServer::serverLoop, this);
    
    logger_->info("FTP Server started on " + config_->connection.bind_address + 
                  ":" + std::to_string(config_->connection.bind_port));
    return true;
}

void FTPServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Close server socket
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }
    
    // Stop connection manager
    if (connection_manager_) {
        connection_manager_->stopAllConnections();
        connection_manager_->stop();
    }
    
    // Wait for server thread
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    
    logger_->info("FTP Server stopped");
}

bool FTPServer::isRunning() const {
    return running_;
}

void FTPServer::serverLoop() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket >= 0) {
            // Check connection limit
            if (connection_manager_->getConnectionCount() >= static_cast<size_t>(config_->connection.max_connections)) {
                logger_->warn("Connection limit reached, rejecting new connection");
                close(client_socket);
                continue;
            }
            
            // Handle new connection
            handleConnection(client_socket);
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            if (running_) {
                logger_->error("Accept error: " + std::string(strerror(errno)));
            }
        }
        
        // Small sleep to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void FTPServer::handleConnection(int client_socket) {
    auto connection = std::make_shared<FTPConnection>(client_socket, logger_, config_);
    connection_manager_->addConnection(connection);
    connection->start();
    
    // Note: Connection will remove itself when done
    // For now, we'll let the connection manager handle cleanup
}

} // namespace simple_sftpd
