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

#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <vector>

namespace simple_sftpd {

class FTPServerConfig;
class Logger;
class FTPConnectionManager;
class IPAccessControl;

class FTPServer {
public:
    explicit FTPServer(std::shared_ptr<FTPServerConfig> config);
    ~FTPServer();

    bool start();
    void stop();
    bool isRunning() const;

private:
    void serverLoop();
    void handleConnection(int client_socket);
    void dropPrivileges();

    std::shared_ptr<FTPServerConfig> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<FTPConnectionManager> connection_manager_;
    std::shared_ptr<IPAccessControl> ip_access_control_;
    
    std::atomic<bool> running_;
    std::thread server_thread_;
    int server_socket_;
};

} // namespace simple_sftpd
