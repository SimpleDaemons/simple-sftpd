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
#include <memory>

namespace simple_sftpd {

class FTPUserManager;

class FTPVirtualHost {
public:
    FTPVirtualHost() = default;
    FTPVirtualHost(const std::string& hostname, const std::string& root_directory);
    ~FTPVirtualHost() = default;

    const std::string& getHostname() const { return hostname_; }
    const std::string& getRootDirectory() const { return root_directory_; }
    bool isEnabled() const { return enabled_; }
    
    void setHostname(const std::string& hostname) { hostname_ = hostname; }
    void setRootDirectory(const std::string& root_dir) { root_directory_ = root_dir; }
    void setEnabled(bool enabled) { enabled_ = enabled; }

    std::shared_ptr<FTPUserManager> getUserManager() const { return user_manager_; }
    void setUserManager(std::shared_ptr<FTPUserManager> user_manager) { user_manager_ = user_manager; }

private:
    std::string hostname_;
    std::string root_directory_;
    bool enabled_ = true;
    std::shared_ptr<FTPUserManager> user_manager_;
};

} // namespace simple_sftpd
