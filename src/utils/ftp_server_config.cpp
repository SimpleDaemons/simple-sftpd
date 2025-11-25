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

#include "simple-sftpd/ftp_server_config.hpp"
#include <fstream>
#include <iostream>

namespace simple_sftpd {

bool FTPServerConfig::loadFromFile(const std::string& filename) {
    (void)filename; // Suppress unused parameter warning
    // Stub implementation - just set some default values
    connection.bind_address = "0.0.0.0";
    connection.bind_port = 21;
    connection.max_connections = 100;
    connection.timeout_seconds = 300;
    
    logging.log_file = "/var/log/simple-sftpd/simple-sftpd.log";
    logging.log_level = "INFO";
    logging.log_to_console = true;
    logging.log_to_file = true;
    
    security.require_ssl = false;
    security.allow_anonymous = false;
    
    rate_limit.enabled = false;
    rate_limit.max_requests_per_minute = 60;
    rate_limit.max_connections_per_ip = 10;
    
    return true;
}

bool FTPServerConfig::validate() {
    clearErrors();
    
    // Basic validation
    if (connection.bind_port <= 0 || connection.bind_port > 65535) {
        addError("Invalid bind port: " + std::to_string(connection.bind_port));
    }
    
    if (connection.max_connections <= 0) {
        addError("Invalid max connections: " + std::to_string(connection.max_connections));
    }
    
    if (connection.timeout_seconds <= 0) {
        addError("Invalid timeout: " + std::to_string(connection.timeout_seconds));
    }
    
    return errors_.empty();
}

void FTPServerConfig::clearErrors() {
    errors_.clear();
    warnings_.clear();
}

void FTPServerConfig::addError(const std::string& error) {
    errors_.push_back(error);
}

void FTPServerConfig::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

} // namespace simple_sftpd
