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
#include <fstream>
#include <mutex>
#include <memory>

namespace simple_sftpd {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

enum class LogFormat {
    STANDARD = 0,
    JSON = 1,
    EXTENDED = 2
};

class Logger {
public:
    Logger(const std::string& log_file, LogLevel level, bool console, bool file, LogFormat format = LogFormat::STANDARD);
    ~Logger();

    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setFormat(LogFormat format);
    LogFormat getFormat() const;

private:
    void log(LogLevel level, const std::string& message);
    std::string levelToString(LogLevel level) const;
    std::string getTimestamp() const;
    std::string formatMessage(LogLevel level, const std::string& message) const;
    std::string escapeJsonString(const std::string& str) const;

    std::string log_file_;
    LogLevel level_;
    LogFormat format_;
    bool console_;
    bool file_;
    std::ofstream file_stream_;
    mutable std::mutex mutex_;
};

} // namespace simple_sftpd
