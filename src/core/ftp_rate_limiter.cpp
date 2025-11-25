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

#include "simple-sftpd/ftp_rate_limiter.hpp"
#include "simple-sftpd/logger.hpp"

namespace simple_sftpd {

FTPRateLimiter::FTPRateLimiter(std::shared_ptr<Logger> logger)
    : logger_(logger) {
}

bool FTPRateLimiter::isAllowed(const std::string& client_ip) {
    (void)client_ip; // Suppress unused parameter warning
    return true; // Stub implementation - allow all requests
}

void FTPRateLimiter::recordRequest(const std::string& client_ip) {
    (void)client_ip; // Suppress unused parameter warning
    // Stub implementation
}

void FTPRateLimiter::setRateLimit(int max_requests_per_minute) {
    max_requests_per_minute_ = max_requests_per_minute;
}

void FTPRateLimiter::setConnectionLimit(int max_connections_per_ip) {
    max_connections_per_ip_ = max_connections_per_ip;
}

} // namespace simple_sftpd
