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

#include "simple-sftpd/ftp_connection_manager.hpp"
#include "simple-sftpd/ftp_connection.hpp"
#include "simple-sftpd/logger.hpp"
#include <atomic>
#include <algorithm>

namespace simple_sftpd {

FTPConnectionManager::FTPConnectionManager(std::shared_ptr<FTPServerConfig> config,
                                         std::shared_ptr<Logger> logger)
    : config_(config), logger_(logger), running_(false),
      connection_timeout_(std::chrono::seconds(300)),
      cleanup_interval_(std::chrono::seconds(60)) {
}

FTPConnectionManager::~FTPConnectionManager() {
    stop();
}

bool FTPConnectionManager::start() {
    if (running_) {
        return true;
    }
    
    running_ = true;
    cleanup_thread_ = std::thread(&FTPConnectionManager::cleanupLoop, this);
    logger_->info("FTP connection manager started");
    return true;
}

void FTPConnectionManager::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (cleanup_thread_.joinable()) {
        cleanup_thread_.join();
    }
    stopAllConnections();
    logger_->info("FTP connection manager stopped");
}

void FTPConnectionManager::addConnection(std::shared_ptr<FTPConnection> connection) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.push_back(connection);
}

void FTPConnectionManager::removeConnection(std::shared_ptr<FTPConnection> connection) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(
        std::remove(connections_.begin(), connections_.end(), connection),
        connections_.end()
    );
}

void FTPConnectionManager::stopAllConnections() {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (auto& connection : connections_) {
        if (connection) {
            connection->stop();
        }
    }
    connections_.clear();
}

size_t FTPConnectionManager::getConnectionCount() const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return connections_.size();
}

std::vector<std::shared_ptr<FTPConnection>> FTPConnectionManager::getConnections() const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return connections_;
}

void FTPConnectionManager::cleanupLoop() {
    while (running_) {
        std::this_thread::sleep_for(cleanup_interval_);
        
        // Clean up inactive connections
        std::lock_guard<std::mutex> lock(connections_mutex_);
        auto it = connections_.begin();
        while (it != connections_.end()) {
            if (!(*it) || !(*it)->isActive()) {
                if (*it) {
                    (*it)->stop();
                }
                it = connections_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

} // namespace simple_sftpd
