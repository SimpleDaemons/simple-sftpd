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

#include "simple-sftpd/security/ip_access_control.hpp"
#include "simple-sftpd/utils/logger.hpp"
#include <sstream>
#include <algorithm>

namespace simple_sftpd {

IPAccessControl::IPAccessControl(std::shared_ptr<Logger> logger)
    : logger_(logger) {
}

void IPAccessControl::addWhitelist(const std::string& ip) {
    whitelist_.push_back(ip);
    logger_->debug("Added to whitelist: " + ip);
}

void IPAccessControl::addBlacklist(const std::string& ip) {
    blacklist_.push_back(ip);
    logger_->debug("Added to blacklist: " + ip);
}

bool IPAccessControl::isAllowed(const std::string& ip) const {
    // Check blacklist first
    for (const auto& blocked : blacklist_) {
        if (matchesCIDR(ip, blocked) || matchesIP(ip, blocked)) {
            logger_->warn("IP blocked by blacklist: " + ip);
            return false;
        }
    }

    // If whitelist exists, check it
    if (!whitelist_.empty()) {
        bool allowed = false;
        for (const auto& allowed_ip : whitelist_) {
            if (matchesCIDR(ip, allowed_ip) || matchesIP(ip, allowed_ip)) {
                allowed = true;
                break;
            }
        }
        if (!allowed) {
            logger_->warn("IP not in whitelist: " + ip);
            return false;
        }
    }

    return true;
}

void IPAccessControl::clearWhitelist() {
    whitelist_.clear();
}

void IPAccessControl::clearBlacklist() {
    blacklist_.clear();
}

bool IPAccessControl::matchesCIDR(const std::string& ip, const std::string& cidr) const {
    size_t slash_pos = cidr.find('/');
    if (slash_pos == std::string::npos) {
        return matchesIP(ip, cidr);
    }

    std::string network = cidr.substr(0, slash_pos);
    int prefix_len = std::stoi(cidr.substr(slash_pos + 1));

    // Simple CIDR matching (basic implementation)
    // Convert IPs to integers for comparison
    std::istringstream ip_stream(ip);
    std::istringstream net_stream(network);
    std::string ip_part, net_part;
    
    int ip_bytes[4] = {0};
    int net_bytes[4] = {0};
    
    for (int i = 0; i < 4; ++i) {
        std::getline(ip_stream, ip_part, '.');
        std::getline(net_stream, net_part, '.');
        ip_bytes[i] = std::stoi(ip_part);
        net_bytes[i] = std::stoi(net_part);
    }

    // Check if IP matches network prefix
    int mask_bytes = prefix_len / 8;
    int mask_bits = prefix_len % 8;
    
    for (int i = 0; i < mask_bytes; ++i) {
        if (ip_bytes[i] != net_bytes[i]) {
            return false;
        }
    }
    
    if (mask_bits > 0 && mask_bytes < 4) {
        int mask = (0xFF << (8 - mask_bits)) & 0xFF;
        if ((ip_bytes[mask_bytes] & mask) != (net_bytes[mask_bytes] & mask)) {
            return false;
        }
    }
    
    return true;
}

bool IPAccessControl::matchesIP(const std::string& ip1, const std::string& ip2) const {
    return ip1 == ip2;
}

} // namespace simple_sftpd

