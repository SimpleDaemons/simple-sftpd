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

#include <gtest/gtest.h>
#include "simple-sftpd/utils/logger.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <thread>
#include <chrono>

using namespace simple_sftpd;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_log_file_ = "/tmp/test_simple_sftpd.log";
        // Clean up any existing test log file
        if (std::filesystem::exists(test_log_file_)) {
            std::filesystem::remove(test_log_file_);
        }
    }

    void TearDown() override {
        // Clean up test log file
        if (std::filesystem::exists(test_log_file_)) {
            std::filesystem::remove(test_log_file_);
        }
    }

    std::string test_log_file_;
};

TEST_F(LoggerTest, Constructor) {
    Logger logger(test_log_file_, LogLevel::INFO, true, false, LogFormat::STANDARD);
    EXPECT_EQ(logger.getLevel(), LogLevel::INFO);
    EXPECT_EQ(logger.getFormat(), LogFormat::STANDARD);
}

TEST_F(LoggerTest, SetLevel) {
    Logger logger(test_log_file_, LogLevel::INFO, true, false, LogFormat::STANDARD);
    logger.setLevel(LogLevel::DEBUG);
    EXPECT_EQ(logger.getLevel(), LogLevel::DEBUG);
    
    logger.setLevel(LogLevel::ERROR);
    EXPECT_EQ(logger.getLevel(), LogLevel::ERROR);
}

TEST_F(LoggerTest, SetFormat) {
    Logger logger(test_log_file_, LogLevel::INFO, true, false, LogFormat::STANDARD);
    logger.setFormat(LogFormat::JSON);
    EXPECT_EQ(logger.getFormat(), LogFormat::JSON);
    
    logger.setFormat(LogFormat::EXTENDED);
    EXPECT_EQ(logger.getFormat(), LogFormat::EXTENDED);
}

TEST_F(LoggerTest, LogLevelFiltering) {
    Logger logger(test_log_file_, LogLevel::WARN, true, false, LogFormat::STANDARD);
    
    // INFO should be filtered out
    logger.info("This should not appear");
    
    // WARN and above should appear
    logger.warn("This should appear");
    logger.error("This should appear");
}

TEST_F(LoggerTest, LogToFile) {
    Logger logger(test_log_file_, LogLevel::INFO, false, true, LogFormat::STANDARD);
    logger.info("Test message");
    
    // Give it a moment to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check if file was created and contains the message
    ASSERT_TRUE(std::filesystem::exists(test_log_file_));
    
    std::ifstream file(test_log_file_);
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    std::getline(file, line);
    EXPECT_NE(line.find("Test message"), std::string::npos);
    EXPECT_NE(line.find("INFO"), std::string::npos);
}

TEST_F(LoggerTest, JSONFormat) {
    Logger logger(test_log_file_, LogLevel::INFO, false, true, LogFormat::JSON);
    logger.info("Test JSON message");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    ASSERT_TRUE(std::filesystem::exists(test_log_file_));
    
    std::ifstream file(test_log_file_);
    std::string line;
    std::getline(file, line);
    
    // Check JSON format
    EXPECT_NE(line.find("\"timestamp\""), std::string::npos);
    EXPECT_NE(line.find("\"level\""), std::string::npos);
    EXPECT_NE(line.find("\"message\""), std::string::npos);
    EXPECT_NE(line.find("Test JSON message"), std::string::npos);
}

TEST_F(LoggerTest, ExtendedFormat) {
    Logger logger(test_log_file_, LogLevel::INFO, false, true, LogFormat::EXTENDED);
    logger.info("Test extended message");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    ASSERT_TRUE(std::filesystem::exists(test_log_file_));
    
    std::ifstream file(test_log_file_);
    std::string line;
    std::getline(file, line);
    
    // Check extended format includes PID
    EXPECT_NE(line.find("PID:"), std::string::npos);
    EXPECT_NE(line.find("Test extended message"), std::string::npos);
}

TEST_F(LoggerTest, AllLogLevels) {
    Logger logger(test_log_file_, LogLevel::TRACE, true, false, LogFormat::STANDARD);
    
    logger.trace("Trace message");
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warn message");
    logger.error("Error message");
    logger.fatal("Fatal message");
    
    // All should execute without throwing
    SUCCEED();
}

