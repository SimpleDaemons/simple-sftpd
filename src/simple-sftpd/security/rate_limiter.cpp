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

#include "simple-sftpd/security/rate_limiter.hpp"
#include "simple-sftpd/utils/logger.hpp"
#include <chrono>

namespace simple_sftpd {

FTPRateLimiter::FTPRateLimiter(std::shared_ptr<Logger> logger)
    : logger_(logger) {
}

bool FTPRateLimiter::isAllowed(const std::string& client_ip) {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto& entry = rate_limits_[client_ip];
    
    // Check if window has expired (1 minute)
    auto window_duration = std::chrono::minutes(1);
    if (entry.window_start == std::chrono::steady_clock::time_point{} ||
        (now - entry.window_start) >= window_duration) {
        // Reset window
        entry.window_start = now;
        entry.request_count = 0;
    }
    
    // Check rate limit
    if (entry.request_count >= max_requests_per_minute_) {
        logger_->warn("Rate limit exceeded for IP: " + client_ip);
        return false;
    }
    
    return true;
}

void FTPRateLimiter::recordRequest(const std::string& client_ip) {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto& entry = rate_limits_[client_ip];
    
    // Check if window has expired
    auto window_duration = std::chrono::minutes(1);
    if (entry.window_start == std::chrono::steady_clock::time_point{} ||
        (now - entry.window_start) >= window_duration) {
        entry.window_start = now;
        entry.request_count = 0;
    }
    
    entry.request_count++;
}

void FTPRateLimiter::setRateLimit(int max_requests_per_minute) {
    max_requests_per_minute_ = max_requests_per_minute;
}

void FTPRateLimiter::setConnectionLimit(int max_connections_per_ip) {
    max_connections_per_ip_ = max_connections_per_ip;
}

} // namespace simple_sftpd
