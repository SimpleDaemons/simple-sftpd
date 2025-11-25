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
    
    running_ = true;
    logger_->info("FTP Server started");
    return true;
}

void FTPServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (connection_manager_) {
        connection_manager_->stopAllConnections();
    }
    logger_->info("FTP Server stopped");
}

bool FTPServer::isRunning() const {
    return running_;
}

void FTPServer::serverLoop() {
    // Stub implementation
}

void FTPServer::handleConnection(int client_socket) {
    (void)client_socket; // Suppress unused parameter warning
    // Stub implementation
}

} // namespace simple_sftpd
