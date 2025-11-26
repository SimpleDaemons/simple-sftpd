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

#include "simple-sftpd/performance_monitor.hpp"
#include "simple-sftpd/logger.hpp"

namespace simple_sftpd {

PerformanceMonitor::PerformanceMonitor(std::shared_ptr<Logger> logger)
    : logger_(logger),
      total_connections_(0),
      active_connections_(0),
      total_transfers_(0),
      total_bytes_transferred_(0),
      total_uploads_(0),
      total_downloads_(0),
      total_requests_(0),
      total_errors_(0),
      total_transfer_time_ms_(0),
      start_time_(std::chrono::steady_clock::now()) {
}

void PerformanceMonitor::recordConnection() {
    total_connections_++;
    active_connections_++;
}

void PerformanceMonitor::recordDisconnection() {
    if (active_connections_ > 0) {
        active_connections_--;
    }
}

void PerformanceMonitor::recordActiveConnection() {
    // Already counted in recordConnection
}

void PerformanceMonitor::recordTransfer(size_t bytes, bool upload) {
    total_transfers_++;
    total_bytes_transferred_ += bytes;
    if (upload) {
        total_uploads_++;
    } else {
        total_downloads_++;
    }
}

void PerformanceMonitor::recordTransferTime(std::chrono::milliseconds duration) {
    total_transfer_time_ms_ += duration.count();
}

void PerformanceMonitor::recordRequest() {
    total_requests_++;
}

void PerformanceMonitor::recordError() {
    total_errors_++;
}

double PerformanceMonitor::getAverageTransferRate() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start_time_).count();
    if (elapsed == 0) {
        return 0.0;
    }
    return static_cast<double>(total_bytes_transferred_.load()) / elapsed;
}

std::chrono::milliseconds PerformanceMonitor::getAverageTransferTime() const {
    uint64_t transfers = total_transfers_.load();
    if (transfers == 0) {
        return std::chrono::milliseconds(0);
    }
    return std::chrono::milliseconds(total_transfer_time_ms_.load() / transfers);
}

void PerformanceMonitor::reset() {
    total_connections_ = 0;
    active_connections_ = 0;
    total_transfers_ = 0;
    total_bytes_transferred_ = 0;
    total_uploads_ = 0;
    total_downloads_ = 0;
    total_requests_ = 0;
    total_errors_ = 0;
    total_transfer_time_ms_ = 0;
    start_time_ = std::chrono::steady_clock::now();
}

} // namespace simple_sftpd

