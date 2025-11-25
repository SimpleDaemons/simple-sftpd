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

#include "simple-sftpd/ftp_user_manager.hpp"
#include "simple-sftpd/ftp_user.hpp"
#include "simple-sftpd/logger.hpp"

namespace simple_sftpd {

FTPUserManager::FTPUserManager(std::shared_ptr<Logger> logger)
    : logger_(logger) {
}

bool FTPUserManager::addUser(std::shared_ptr<FTPUser> user) {
    if (!user) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(users_mutex_);
    users_[user->getUsername()] = user;
    logger_->info("Added user: " + user->getUsername());
    return true;
}

bool FTPUserManager::removeUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(users_mutex_);
    auto it = users_.find(username);
    if (it != users_.end()) {
        users_.erase(it);
        logger_->info("Removed user: " + username);
        return true;
    }
    return false;
}

std::shared_ptr<FTPUser> FTPUserManager::getUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(users_mutex_);
    auto it = users_.find(username);
    if (it != users_.end()) {
        return it->second;
    }
    return nullptr;
}

bool FTPUserManager::authenticateUser(const std::string& username, const std::string& password) {
    auto user = getUser(username);
    if (!user) {
        return false;
    }
    return user->authenticate(password);
}

std::vector<std::string> FTPUserManager::listUsers() const {
    std::lock_guard<std::mutex> lock(users_mutex_);
    std::vector<std::string> usernames;
    for (const auto& pair : users_) {
        usernames.push_back(pair.first);
    }
    return usernames;
}

} // namespace simple_sftpd
