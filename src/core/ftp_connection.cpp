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

#include "ssftpd/ftp_connection.hpp"
#include "ssftpd/logger.hpp"

namespace ssftpd {

FTPConnection::FTPConnection(int socket, std::shared_ptr<Logger> logger, std::shared_ptr<FTPServerConfig> config)
    : socket_(socket), logger_(logger), config_(config), active_(false) {
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
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
    logger_->info("FTP connection stopped");
}

bool FTPConnection::isActive() const {
    return active_;
}

void FTPConnection::handleClient() {
    // Stub implementation
    while (active_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FTPConnection::sendResponse(const std::string& response) {
    (void)response; // Suppress unused parameter warning
    // Stub implementation
}

std::string FTPConnection::readLine() {
    // Stub implementation
    return "";
}

} // namespace ssftpd
