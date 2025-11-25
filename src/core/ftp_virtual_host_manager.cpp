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

#include "simple-sftpd/ftp_virtual_host_manager.hpp"
#include "simple-sftpd/ftp_virtual_host.hpp"
#include "simple-sftpd/logger.hpp"
#include <vector>
#include <string>

namespace simple_sftpd {

FTPVirtualHostManager::FTPVirtualHostManager(std::shared_ptr<Logger> logger)
    : logger_(logger) {
}

bool FTPVirtualHostManager::addVirtualHost(std::shared_ptr<FTPVirtualHost> host) {
    if (!host) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(hosts_mutex_);
    virtual_hosts_[host->getHostname()] = host;
    logger_->info("Added virtual host: " + host->getHostname());
    return true;
}

bool FTPVirtualHostManager::removeVirtualHost(const std::string& hostname) {
    std::lock_guard<std::mutex> lock(hosts_mutex_);
    auto it = virtual_hosts_.find(hostname);
    if (it != virtual_hosts_.end()) {
        virtual_hosts_.erase(it);
        logger_->info("Removed virtual host: " + hostname);
        return true;
    }
    return false;
}

std::shared_ptr<FTPVirtualHost> FTPVirtualHostManager::getVirtualHost(const std::string& hostname) {
    std::lock_guard<std::mutex> lock(hosts_mutex_);
    auto it = virtual_hosts_.find(hostname);
    if (it != virtual_hosts_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> FTPVirtualHostManager::listVirtualHosts() const {
    std::lock_guard<std::mutex> lock(hosts_mutex_);
    std::vector<std::string> hostnames;
    for (const auto& pair : virtual_hosts_) {
        hostnames.push_back(pair.first);
    }
    return hostnames;
}

} // namespace simple_sftpd
