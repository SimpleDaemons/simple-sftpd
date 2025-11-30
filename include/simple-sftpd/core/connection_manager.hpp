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

#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

namespace simple_sftpd {

class FTPConnection;
class FTPServerConfig;
class Logger;

class FTPConnectionManager {
public:
    FTPConnectionManager(std::shared_ptr<FTPServerConfig> config,
                        std::shared_ptr<Logger> logger);
    ~FTPConnectionManager();

    bool start();
    void stop();
    void addConnection(std::shared_ptr<FTPConnection> connection);
    void removeConnection(std::shared_ptr<FTPConnection> connection);
    void stopAllConnections();
    
    size_t getConnectionCount() const;
    std::vector<std::shared_ptr<FTPConnection>> getConnections() const;
    
    // Connection Pooling
    std::shared_ptr<FTPConnection> acquireConnection();
    void releaseConnection(std::shared_ptr<FTPConnection> connection);
    void setPoolSize(size_t pool_size);
    size_t getPoolSize() const { return pool_size_; }

private:
    void cleanupLoop();
    void poolMaintenanceLoop();

    std::shared_ptr<FTPServerConfig> config_;
    std::shared_ptr<Logger> logger_;
    mutable std::mutex connections_mutex_;
    std::vector<std::shared_ptr<FTPConnection>> connections_;
    std::atomic<bool> running_;
    std::thread cleanup_thread_;
    std::chrono::seconds connection_timeout_;
    std::chrono::seconds cleanup_interval_;
    
    // Connection Pooling
    mutable std::mutex pool_mutex_;
    std::vector<std::shared_ptr<FTPConnection>> connection_pool_;
    size_t pool_size_;
    std::thread pool_maintenance_thread_;
};

} // namespace simple_sftpd
