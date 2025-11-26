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
#include <mutex>

namespace simple_sftpd {

class Logger;
class FTPServerConfig;
class FTPUserManager;
class FTPUser;
class SSLContext;

class FTPConnection {
public:
    FTPConnection(int socket, std::shared_ptr<Logger> logger, std::shared_ptr<FTPServerConfig> config);
    ~FTPConnection();

    void start();
    void stop();
    bool isActive() const;

private:
    void handleClient();
    void sendResponse(const std::string& response);
    std::string readLine();
    
    // FTP Command Handlers
    void handleUSER(const std::string& username);
    void handlePASS(const std::string& password);
    void handleQUIT();
    void handlePWD();
    void handleCWD(const std::string& path);
    void handleLIST(const std::string& path);
    void handlePASV();
    void handleTYPE(const std::string& type);
    void handleSIZE(const std::string& filename);
    void handleRETR(const std::string& filename);
    void handleSTOR(const std::string& filename);
    void handleDELE(const std::string& filename);
    void handleMKD(const std::string& dirname);
    void handleRMD(const std::string& dirname);
    void handleAUTH(const std::string& method);
    void handlePBSZ(const std::string& size);
    void handlePROT(const std::string& level);
    
    // Data Connection Management
    int createPassiveDataSocket();
    int acceptDataConnection();
    void closeDataSocket();
    std::string formatPassiveResponse(int port);
    
    // Path and Permission Utilities
    std::string resolvePath(const std::string& path);
    bool validatePath(const std::string& path);
    bool hasPermission(const std::string& operation, const std::string& path);
    bool isPathWithinHome(const std::string& path);

    // SSL/TLS Support
    bool initializeSSL();
    bool upgradeToSSL();
    void* getSSL() const { return ssl_; }

    int socket_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<FTPServerConfig> config_;
    std::shared_ptr<FTPUserManager> user_manager_;
    std::shared_ptr<SSLContext> ssl_context_;
    
    std::atomic<bool> active_;
    std::thread client_thread_;
    
    bool authenticated_;
    std::string username_;
    std::shared_ptr<FTPUser> current_user_;
    std::string current_directory_;
    
    // SSL/TLS state
    bool ssl_enabled_;
    bool ssl_active_;
    void* ssl_;  // SSL* pointer (void* to avoid OpenSSL dependency in header)
    void* data_ssl_;  // SSL* for data connection
    
    // Data connection state
    int passive_listen_socket_;
    int data_socket_;
    std::mutex data_socket_mutex_;
    std::string transfer_type_;  // "A" for ASCII, "I" for binary
    std::string protection_level_;  // "C" for clear, "P" for private (encrypted)
};

} // namespace simple_sftpd
