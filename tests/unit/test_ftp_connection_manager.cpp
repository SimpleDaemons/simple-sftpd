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
#include "simple-sftpd/ftp_connection_manager.hpp"
#include "simple-sftpd/ftp_server_config.hpp"
#include "simple-sftpd/logger.hpp"
#include "simple-sftpd/ftp_connection.hpp"
#include <memory>

using namespace simple_sftpd;

class FTPConnectionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<FTPServerConfig>();
        logger_ = std::make_shared<Logger>("", LogLevel::ERROR, false, false, LogFormat::STANDARD);
        manager_ = std::make_shared<FTPConnectionManager>(config_, logger_);
    }

    void TearDown() override {
        if (manager_) {
            manager_->stop();
        }
    }

    std::shared_ptr<FTPServerConfig> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<FTPConnectionManager> manager_;
};

TEST_F(FTPConnectionManagerTest, Constructor) {
    EXPECT_EQ(manager_->getConnectionCount(), 0);
}

TEST_F(FTPConnectionManagerTest, StartStop) {
    manager_->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    manager_->stop();
    SUCCEED();
}

TEST_F(FTPConnectionManagerTest, GetConnectionCountEmpty) {
    EXPECT_EQ(manager_->getConnectionCount(), 0);
}

TEST_F(FTPConnectionManagerTest, GetConnectionsEmpty) {
    auto connections = manager_->getConnections();
    EXPECT_TRUE(connections.empty());
}

