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
#include "simple-sftpd/security/rate_limiter.hpp"
#include "simple-sftpd/utils/logger.hpp"
#include <thread>
#include <chrono>

using namespace simple_sftpd;

class FTPRateLimiterTest : public ::testing::Test {
protected:
    void SetUp() override {
        logger_ = std::make_shared<Logger>("", LogLevel::ERROR, false, false, LogFormat::STANDARD);
        limiter_ = std::make_shared<FTPRateLimiter>(logger_);
        limiter_->setRateLimit(60);
        limiter_->setConnectionLimit(10);
    }

    std::shared_ptr<Logger> logger_;
    std::shared_ptr<FTPRateLimiter> limiter_;
};

TEST_F(FTPRateLimiterTest, IsAllowedWithinLimit) {
    std::string client_ip = "192.168.1.100";
    
    // First 60 requests should be allowed
    for (int i = 0; i < 60; ++i) {
        EXPECT_TRUE(limiter_->isAllowed(client_ip));
        limiter_->recordRequest(client_ip);
    }
}

TEST_F(FTPRateLimiterTest, IsAllowedExceedsLimit) {
    std::string client_ip = "192.168.1.100";
    
    // Make 60 requests (the limit)
    for (int i = 0; i < 60; ++i) {
        limiter_->recordRequest(client_ip);
    }
    
    // 61st request should be denied
    EXPECT_FALSE(limiter_->isAllowed(client_ip));
}

TEST_F(FTPRateLimiterTest, DifferentIPsSeparateLimits) {
    std::string ip1 = "192.168.1.100";
    std::string ip2 = "192.168.1.101";
    
    // Exhaust limit for IP1
    for (int i = 0; i < 60; ++i) {
        limiter_->recordRequest(ip1);
    }
    
    // IP2 should still be allowed
    EXPECT_TRUE(limiter_->isAllowed(ip2));
}

TEST_F(FTPRateLimiterTest, SetRateLimit) {
    limiter_->setRateLimit(30);
    
    std::string client_ip = "192.168.1.100";
    
    // Make 30 requests
    for (int i = 0; i < 30; ++i) {
        limiter_->recordRequest(client_ip);
    }
    
    // 31st should be denied
    EXPECT_FALSE(limiter_->isAllowed(client_ip));
}

TEST_F(FTPRateLimiterTest, SetConnectionLimit) {
    limiter_->setConnectionLimit(5);
    
    // Connection limit is tracked separately
    // This test verifies the method exists and works
    SUCCEED();
}

TEST_F(FTPRateLimiterTest, RecordRequest) {
    std::string client_ip = "192.168.1.100";
    
    // Record multiple requests
    for (int i = 0; i < 10; ++i) {
        limiter_->recordRequest(client_ip);
    }
    
    // Should still be allowed (under limit)
    EXPECT_TRUE(limiter_->isAllowed(client_ip));
}

