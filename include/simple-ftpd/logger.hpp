#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>

namespace simple_ftpd {

/**
 * @brief Log levels
 */
enum class LogLevel {
    TRACE = 0,      // Trace level (most verbose)
    DEBUG = 1,      // Debug level
    INFO = 2,       // Information level
    WARN = 3,       // Warning level
    ERROR = 4,      // Error level
    FATAL = 5       // Fatal level (least verbose)
};

/**
 * @brief Log format types
 */
enum class LogFormat {
    SIMPLE,         // Simple format: [LEVEL] message
    STANDARD,       // Standard format: [TIMESTAMP] [LEVEL] [THREAD] message
    EXTENDED,       // Extended format: [TIMESTAMP] [LEVEL] [THREAD] [FILE:LINE] message
    JSON,           // JSON format: {"timestamp": "...", "level": "...", "message": "..."}
    CUSTOM          // Custom format
};

/**
 * @brief Logger class
 * 
 * This class provides logging functionality for the FTP server:
 * - Multiple log levels
 * - File and console output
 * - Thread-safe logging
 * - Configurable formats
 * - Log rotation
 * - Performance monitoring
 */
class Logger {
public:
    /**
     * @brief Constructor
     * @param log_file Log file path
     * @param log_level Minimum log level
     * @param log_to_console Enable console output
     * @param log_to_file Enable file output
     */
    Logger(const std::string& log_file = "",
           LogLevel log_level = LogLevel::INFO,
           bool log_to_console = true,
           bool log_to_file = true);
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Set log level
     * @param level New log level
     */
    void setLogLevel(LogLevel level) { log_level_ = level; }
    
    /**
     * @brief Get log level
     * @return Current log level
     */
    LogLevel getLogLevel() const { return log_level_; }
    
    /**
     * @brief Set log format
     * @param format New log format
     */
    void setLogFormat(LogFormat format) { log_format_ = format; }
    
    /**
     * @brief Get log format
     * @return Current log format
     */
    LogFormat getLogFormat() const { return log_format_ = format; }
    
    /**
     * @brief Set custom log format
     * @param format Custom format string
     */
    void setCustomFormat(const std::string& format) { custom_format_ = format; }
    
    /**
     * @brief Get custom log format
     * @return Custom format string
     */
    std::string getCustomFormat() const { return custom_format_; }
    
    /**
     * @brief Enable/disable console output
     * @param enable true to enable, false to disable
     */
    void setConsoleOutput(bool enable) { log_to_console_ = enable; }
    
    /**
     * @brief Check if console output is enabled
     * @return true if enabled, false otherwise
     */
    bool isConsoleOutputEnabled() const { return log_to_console_; }
    
    /**
     * @brief Enable/disable file output
     * @param enable true to enable, false to disable
     */
    void setFileOutput(bool enable) { log_to_file_ = enable; }
    
    /**
     * @brief Check if file output is enabled
     * @return true if enabled, false otherwise
     */
    bool isFileOutputEnabled() const { return log_to_file_; }
    
    /**
     * @brief Set log file path
     * @param log_file New log file path
     */
    void setLogFile(const std::string& log_file);
    
    /**
     * @brief Get log file path
     * @return Current log file path
     */
    std::string getLogFile() const { return log_file_; }
    
    /**
     * @brief Set maximum log file size
     * @param max_size Maximum size in bytes
     */
    void setMaxLogSize(size_t max_size) { max_log_size_ = max_size; }
    
    /**
     * @brief Get maximum log file size
     * @return Maximum size in bytes
     */
    size_t getMaxLogSize() const { return max_log_size_; }
    
    /**
     * @brief Set maximum number of log files
     * @param max_files Maximum number of files
     */
    void setMaxLogFiles(size_t max_files) { max_log_files_ = max_files; }
    
    /**
     * @brief Get maximum number of log files
     * @return Maximum number of files
     */
    size_t getMaxLogFiles() const { return max_log_files_; }
    
    /**
     * @brief Enable/disable log rotation
     * @param enable true to enable, false to disable
     */
    void setLogRotation(bool enable) { log_rotation_enabled_ = enable; }
    
    /**
     * @brief Check if log rotation is enabled
     * @return true if enabled, false otherwise
     */
    bool isLogRotationEnabled() const { return log_rotation_enabled_; }
    
    /**
     * @brief Log trace message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void trace(const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Log debug message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void debug(const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Log info message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void info(const std::string& message,
              const std::string& file = "",
              int line = 0,
              const std::string& function = "");
    
    /**
     * @brief Log warning message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void warn(const std::string& message,
              const std::string& file = "",
              int line = 0,
              const std::string& function = "");
    
    /**
     * @brief Log error message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void error(const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Log fatal message
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void fatal(const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "");
    
    /**
     * @brief Log message with custom level
     * @param level Log level
     * @param message Message to log
     * @param file Source file (optional)
     * @param line Source line (optional)
     * @param function Source function (optional)
     */
    void log(LogLevel level,
             const std::string& message,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "");
    
    /**
     * @brief Flush log buffers
     */
    void flush();
    
    /**
     * @brief Close log file
     */
    void close();
    
    /**
     * @brief Check if log file is open
     * @return true if open, false otherwise
     */
    bool isLogFileOpen() const;
    
    /**
     * @brief Get log statistics
     * @return Statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Reset log statistics
     */
    void resetStatistics();
    
    /**
     * @brief Enable/disable performance monitoring
     * @param enable true to enable, false to disable
     */
    void setPerformanceMonitoring(bool enable) { performance_monitoring_ = enable; }
    
    /**
     * @brief Check if performance monitoring is enabled
     * @return true if enabled, false otherwise
     */
    bool isPerformanceMonitoringEnabled() const { return performance_monitoring_; }
    
    /**
     * @brief Get performance metrics
     * @return Performance metrics string
     */
    std::string getPerformanceMetrics() const;
    
    /**
     * @brief Set log filter
     * @param filter Filter string
     */
    void setLogFilter(const std::string& filter);
    
    /**
     * @brief Get log filter
     * @return Current log filter
     */
    std::string getLogFilter() const { return log_filter_; }
    
    /**
     * @brief Check if message should be logged
     * @param message Message to check
     * @return true if should be logged, false otherwise
     */
    bool shouldLogMessage(const std::string& message) const;
    
    /**
     * @brief Set log buffer size
     * @param buffer_size Buffer size in bytes
     */
    void setLogBufferSize(size_t buffer_size);
    
    /**
     * @brief Get log buffer size
     * @return Buffer size in bytes
     */
    size_t getLogBufferSize() const { return log_buffer_size_; }
    
    /**
     * @brief Enable/disable asynchronous logging
     * @param enable true to enable, false to disable
     */
    void setAsyncLogging(bool enable);
    
    /**
     * @brief Check if asynchronous logging is enabled
     * @return true if enabled, false otherwise
     */
    bool isAsyncLoggingEnabled() const { return async_logging_; }

private:
    // Configuration
    std::string log_file_;
    LogLevel log_level_;
    LogFormat log_format_;
    std::string custom_format_;
    bool log_to_console_;
    bool log_to_file_;
    bool log_rotation_enabled_;
    size_t max_log_size_;
    size_t max_log_files_;
    bool performance_monitoring_;
    std::string log_filter_;
    size_t log_buffer_size_;
    bool async_logging_;
    
    // File handling
    std::ofstream log_stream_;
    mutable std::mutex log_mutex_;
    
    // Statistics
    std::atomic<uint64_t> messages_logged_;
    std::atomic<uint64_t> bytes_written_;
    std::atomic<uint64_t> files_rotated_;
    std::chrono::steady_clock::time_point start_time_;
    
    // Performance monitoring
    std::atomic<uint64_t> total_log_time_;
    std::atomic<uint64_t> max_log_time_;
    std::atomic<uint64_t> min_log_time_;
    std::atomic<uint64_t> log_calls_;
    
    // Asynchronous logging
    std::thread async_thread_;
    std::atomic<bool> async_running_;
    std::vector<std::string> async_buffer_;
    mutable std::mutex async_mutex_;
    std::condition_variable async_condition_;
    
    /**
     * @brief Format log message
     * @param level Log level
     * @param message Message to format
     * @param file Source file
     * @param line Source line
     * @param function Source function
     * @return Formatted message
     */
    std::string formatMessage(LogLevel level,
                             const std::string& message,
                             const std::string& file,
                             int line,
                             const std::string& function);
    
    /**
     * @brief Write message to console
     * @param formatted_message Formatted message
     */
    void writeToConsole(const std::string& formatted_message);
    
    /**
     * @brief Write message to file
     * @param formatted_message Formatted message
     */
    void writeToFile(const std::string& formatted_message);
    
    /**
     * @brief Check if log rotation is needed
     * @return true if rotation needed, false otherwise
     */
    bool shouldRotateLog() const;
    
    /**
     * @brief Rotate log file
     */
    void rotateLog();
    
    /**
     * @brief Get current timestamp
     * @return Current timestamp as string
     */
    std::string getCurrentTimestamp() const;
    
    /**
     * @brief Get current thread ID
     * @return Current thread ID as string
     */
    std::string getCurrentThreadId() const;
    
    /**
     * @brief Get log level string
     * @param level Log level
     * @return Log level string
     */
    std::string getLogLevelString(LogLevel level) const;
    
    /**
     * @brief Get log level color (for console output)
     * @param level Log level
     * @return ANSI color code
     */
    std::string getLogLevelColor(LogLevel level) const;
    
    /**
     * @brief Reset console color
     * @return ANSI reset code
     */
    std::string resetConsoleColor() const;
    
    /**
     * @brief Asynchronous logging thread function
     */
    void asyncLoggingThread();
    
    /**
     * @brief Process asynchronous log buffer
     */
    void processAsyncBuffer();
    
    /**
     * @brief Add message to asynchronous buffer
     * @param message Message to add
     */
    void addToAsyncBuffer(const std::string& message);
    
    /**
     * @brief Check if message matches filter
     * @param message Message to check
     * @return true if matches, false otherwise
     */
    bool messageMatchesFilter(const std::string& message) const;
    
    /**
     * @brief Update performance metrics
     * @param start_time Start time of logging operation
     */
    void updatePerformanceMetrics(const std::chrono::steady_clock::time_point& start_time);
};

} // namespace simple_ftpd
