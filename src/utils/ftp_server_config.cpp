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
#include <sstream>
#include <algorithm>
#include <cctype>

namespace simple_sftpd {

bool FTPServerConfig::loadFromFile(const std::string& filename) {
    clearErrors();
    
    // Set defaults first
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
    
    // Try to load from file
    std::ifstream file(filename);
    if (!file.is_open()) {
        addWarning("Configuration file not found: " + filename + ", using defaults");
        return true; // Not an error, just use defaults
    }
    
    std::string line;
    std::string current_section;
    
    while (std::getline(file, line)) {
        // Remove comments
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        // Check for section header [section]
        if (line[0] == '[' && line.back() == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key = value
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Remove quotes if present
        if (value.length() >= 2 && value[0] == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        // Parse based on section and key
        if (current_section == "connection" || current_section.empty()) {
            if (key == "bind_address") {
                connection.bind_address = value;
            } else if (key == "bind_port") {
                connection.bind_port = std::stoi(value);
            } else if (key == "max_connections") {
                connection.max_connections = std::stoi(value);
            } else if (key == "timeout_seconds" || key == "connection_timeout") {
                connection.timeout_seconds = std::stoi(value);
            }
        } else if (current_section == "logging") {
            if (key == "log_file") {
                logging.log_file = value;
            } else if (key == "log_level") {
                logging.log_level = value;
            } else if (key == "log_to_console") {
                logging.log_to_console = (value == "true" || value == "1");
            } else if (key == "log_to_file") {
                logging.log_to_file = (value == "true" || value == "1");
            }
        } else if (current_section == "security") {
            if (key == "require_ssl") {
                security.require_ssl = (value == "true" || value == "1");
            } else if (key == "allow_anonymous") {
                security.allow_anonymous = (value == "true" || value == "1");
            }
        } else if (current_section == "rate_limit") {
            if (key == "enabled") {
                rate_limit.enabled = (value == "true" || value == "1");
            } else if (key == "max_requests_per_minute") {
                rate_limit.max_requests_per_minute = std::stoi(value);
            } else if (key == "max_connections_per_ip") {
                rate_limit.max_connections_per_ip = std::stoi(value);
            }
        }
    }
    
    file.close();
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
