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

#include <atomic>
#include <chrono>
#include <string>
#include <memory>

namespace simple_sftpd {

class Logger;

/**
 * @brief Performance Monitor
 * 
 * Tracks server performance metrics and statistics
 */
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Logger> logger);
    ~PerformanceMonitor() = default;

    // Connection statistics
    void recordConnection();
    void recordDisconnection();
    void recordActiveConnection();
    
    // Transfer statistics
    void recordTransfer(size_t bytes, bool upload);
    void recordTransferTime(std::chrono::milliseconds duration);
    
    // Request statistics
    void recordRequest();
    void recordError();
    
    // Get statistics
    uint64_t getTotalConnections() const { return total_connections_; }
    uint64_t getActiveConnections() const { return active_connections_; }
    uint64_t getTotalTransfers() const { return total_transfers_; }
    uint64_t getTotalBytesTransferred() const { return total_bytes_transferred_; }
    uint64_t getTotalUploads() const { return total_uploads_; }
    uint64_t getTotalDownloads() const { return total_downloads_; }
    uint64_t getTotalRequests() const { return total_requests_; }
    uint64_t getTotalErrors() const { return total_errors_; }
    
    // Performance metrics
    double getAverageTransferRate() const; // bytes per second
    std::chrono::milliseconds getAverageTransferTime() const;
    
    // Reset statistics
    void reset();

private:
    std::shared_ptr<Logger> logger_;
    
    std::atomic<uint64_t> total_connections_;
    std::atomic<uint64_t> active_connections_;
    std::atomic<uint64_t> total_transfers_;
    std::atomic<uint64_t> total_bytes_transferred_;
    std::atomic<uint64_t> total_uploads_;
    std::atomic<uint64_t> total_downloads_;
    std::atomic<uint64_t> total_requests_;
    std::atomic<uint64_t> total_errors_;
    
    std::atomic<uint64_t> total_transfer_time_ms_;
    std::chrono::steady_clock::time_point start_time_;
};

} // namespace simple_sftpd

