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

#include <string>
#include <vector>
#include <memory>

namespace simple_sftpd {

class Logger;

/**
 * @brief IP-based Access Control
 * 
 * Manages IP whitelisting and blacklisting for connection filtering
 */
class IPAccessControl {
public:
    IPAccessControl(std::shared_ptr<Logger> logger);
    ~IPAccessControl() = default;

    /**
     * @brief Add IP to whitelist
     * @param ip IP address or CIDR notation (e.g., "192.168.1.0/24")
     */
    void addWhitelist(const std::string& ip);

    /**
     * @brief Add IP to blacklist
     * @param ip IP address or CIDR notation
     */
    void addBlacklist(const std::string& ip);

    /**
     * @brief Check if IP is allowed
     * @param ip IP address to check
     * @return true if allowed, false if blocked
     */
    bool isAllowed(const std::string& ip) const;

    /**
     * @brief Clear all whitelist entries
     */
    void clearWhitelist();

    /**
     * @brief Clear all blacklist entries
     */
    void clearBlacklist();

private:
    std::shared_ptr<Logger> logger_;
    std::vector<std::string> whitelist_;
    std::vector<std::string> blacklist_;

    bool matchesCIDR(const std::string& ip, const std::string& cidr) const;
    bool matchesIP(const std::string& ip1, const std::string& ip2) const;
};

} // namespace simple_sftpd

