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
#include "simple-sftpd/ftp_user.hpp"

using namespace simple_sftpd;

class FTPUserTest : public ::testing::Test {
protected:
    void SetUp() override {
        user_ = std::make_shared<FTPUser>("testuser", "testpass", "/home/testuser");
    }

    std::shared_ptr<FTPUser> user_;
};

TEST_F(FTPUserTest, Constructor) {
    EXPECT_EQ(user_->getUsername(), "testuser");
    EXPECT_EQ(user_->getPassword(), "testpass");
    EXPECT_EQ(user_->getHomeDirectory(), "/home/testuser");
}

TEST_F(FTPUserTest, AuthenticateSuccess) {
    EXPECT_TRUE(user_->authenticate("testpass"));
}

TEST_F(FTPUserTest, AuthenticateFailure) {
    EXPECT_FALSE(user_->authenticate("wrongpass"));
    EXPECT_FALSE(user_->authenticate(""));
    EXPECT_FALSE(user_->authenticate("testpass "));
}

TEST_F(FTPUserTest, SetUsername) {
    user_->setUsername("newuser");
    EXPECT_EQ(user_->getUsername(), "newuser");
}

TEST_F(FTPUserTest, SetPassword) {
    user_->setPassword("newpass");
    EXPECT_TRUE(user_->authenticate("newpass"));
    EXPECT_FALSE(user_->authenticate("testpass"));
}

TEST_F(FTPUserTest, SetHomeDirectory) {
    user_->setHomeDirectory("/home/newuser");
    EXPECT_EQ(user_->getHomeDirectory(), "/home/newuser");
}

TEST_F(FTPUserTest, HasPermissionDefault) {
    // Default behavior: empty permissions means allow all
    EXPECT_TRUE(user_->hasPermission("read", "/home/testuser/file.txt"));
    EXPECT_TRUE(user_->hasPermission("write", "/home/testuser/file.txt"));
    EXPECT_TRUE(user_->hasPermission("list", "/home/testuser"));
}

TEST_F(FTPUserTest, DefaultConstructor) {
    FTPUser user;
    EXPECT_EQ(user.getUsername(), "");
    EXPECT_EQ(user.getPassword(), "");
    EXPECT_EQ(user.getHomeDirectory(), "");
    EXPECT_TRUE(user.hasPermission("read", "")); // Default allows all
}

