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

#include "simple-sftpd/logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <unistd.h>

namespace simple_sftpd {

Logger::Logger(const std::string& log_file, LogLevel level, bool console, bool file, LogFormat format)
    : log_file_(log_file), level_(level), format_(format), console_(console), file_(file) {
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

void Logger::setFormat(LogFormat format) {
    format_ = format;
}

LogFormat Logger::getFormat() const {
    return format_;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < level_) {
        return;
    }
    
    std::string log_message = formatMessage(level, message);
    
    if (console_) {
        std::cout << log_message << std::endl;
    }
    
    if (file_ && file_stream_.is_open()) {
        file_stream_ << log_message << std::endl;
        file_stream_.flush();
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) const {
    switch (format_) {
        case LogFormat::JSON: {
            std::stringstream ss;
            ss << "{"
               << "\"timestamp\":\"" << getTimestamp() << "\","
               << "\"level\":\"" << levelToString(level) << "\","
               << "\"message\":\"" << escapeJsonString(message) << "\""
               << "}";
            return ss.str();
        }
        case LogFormat::EXTENDED: {
            std::stringstream ss;
            ss << "[" << getTimestamp() << "] "
               << "[" << levelToString(level) << "] "
               << "[PID:" << getpid() << "] "
               << message;
            return ss.str();
        }
        case LogFormat::STANDARD:
        default: {
            return "[" + getTimestamp() + "] [" + levelToString(level) + "] " + message;
        }
    }
}

std::string Logger::escapeJsonString(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    std::stringstream ss;
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                       << static_cast<int>(static_cast<unsigned char>(c));
                    escaped += ss.str();
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
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

} // namespace simple_sftpd
