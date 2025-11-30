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
#include <chrono>

namespace simple_sftpd {

class FTPUser {
public:
    FTPUser() = default;
    FTPUser(const std::string& username, const std::string& password, const std::string& home_dir);
    ~FTPUser() = default;

    const std::string& getUsername() const { return username_; }
    const std::string& getPassword() const { return password_; }
    const std::string& getHomeDirectory() const { return home_directory_; }
    
    void setUsername(const std::string& username) { username_ = username; }
    void setPassword(const std::string& password) { password_ = password; }
    void setHomeDirectory(const std::string& home_dir) { home_directory_ = home_dir; }

    bool authenticate(const std::string& password) const;
    bool hasPermission(const std::string& operation, const std::string& path) const;

private:
    std::string username_;
    std::string password_;
    std::string home_directory_;
    std::vector<std::string> permissions_;
};

} // namespace simple_sftpd
