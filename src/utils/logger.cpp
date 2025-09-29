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

#include "ssftpd/logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace ssftpd {

Logger::Logger(const std::string& log_file, LogLevel level, bool console, bool file)
    : log_file_(log_file), level_(level), console_(console), file_(file) {
    if (file && !log_file_.empty()) {
        file_stream_.open(log_file_, std::ios::app);
    }
}

Logger::~Logger() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

void Logger::trace(const std::string& message) {
    log(LogLevel::TRACE, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}

LogLevel Logger::getLevel() const {
    return level_;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < level_) {
        return;
    }
    
    std::string log_message = "[" + getTimestamp() + "] [" + levelToString(level) + "] " + message;
    
    if (console_) {
        std::cout << log_message << std::endl;
    }
    
    if (file_ && file_stream_.is_open()) {
        file_stream_ << log_message << std::endl;
        file_stream_.flush();
    }
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace ssftpd
