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
#include "simple-sftpd/server.hpp"
#include "simple-sftpd/server_config.hpp"
#include "simple-sftpd/utils/logger.hpp"
#include <memory>
#include <thread>
#include <chrono>

using namespace simple_sftpd;

// Note: Full integration tests for FTPServer would require actual network setup
// These are basic tests that verify the class can be instantiated

class FTPServerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<FTPServerConfig>();
        logger_ = std::make_shared<Logger>("", LogLevel::ERROR, false, false, LogFormat::STANDARD);
    }

    void TearDown() override {
        if (server_) {
            server_->stop();
        }
    }

    std::shared_ptr<FTPServerConfig> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<FTPServer> server_;
};

TEST_F(FTPServerIntegrationTest, Constructor) {
    server_ = std::make_shared<FTPServer>(config_);
    EXPECT_FALSE(server_->isRunning());
}

TEST_F(FTPServerIntegrationTest, IsRunningInitiallyFalse) {
    server_ = std::make_shared<FTPServer>(config_);
    EXPECT_FALSE(server_->isRunning());
}

