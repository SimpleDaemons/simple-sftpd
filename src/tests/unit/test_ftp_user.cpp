#include <gtest/gtest.h>
#include "simple-ftpd/ftp_user.hpp"
#include <memory>

class FTPUserTest : public ::testing::Test {
protected:
    void SetUp() override {
        user = std::make_shared<simple_ftpd::FTPUser>("testuser");
    }
    
    void TearDown() override {
        user.reset();
    }
    
    std::shared_ptr<simple_ftpd::FTPUser> user;
};

TEST_F(FTPUserTest, Constructor) {
    EXPECT_EQ(user->getUsername(), "testuser");
    EXPECT_EQ(user->getStatus(), simple_ftpd::UserStatus::ACTIVE);
    EXPECT_TRUE(user->isEnabled());
}

TEST_F(FTPUserTest, PasswordVerification) {
    // Test password setting and verification
    user->setPassword("testpass123");
    EXPECT_TRUE(user->verifyPassword("testpass123"));
    EXPECT_FALSE(user->verifyPassword("wrongpass"));
    EXPECT_FALSE(user->verifyPassword(""));
}

TEST_F(FTPUserTest, PermissionManagement) {
    // Test permission granting
    user->grantPermission(simple_ftpd::UserPermission::READ);
    user->grantPermission(simple_ftpd::UserPermission::WRITE);
    
    EXPECT_TRUE(user->hasPermission(simple_ftpd::UserPermission::READ));
    EXPECT_TRUE(user->hasPermission(simple_ftpd::UserPermission::WRITE));
    EXPECT_FALSE(user->hasPermission(simple_ftpd::UserPermission::DELETE));
    
    // Test permission revocation
    user->revokePermission(simple_ftpd::UserPermission::WRITE);
    EXPECT_FALSE(user->hasPermission(simple_ftpd::UserPermission::WRITE));
}

TEST_F(FTPUserTest, HomeDirectory) {
    std::string home_dir = "/var/ftp/testuser";
    user->setHomeDirectory(home_dir);
    EXPECT_EQ(user->getHomeDirectory(), home_dir);
}

TEST_F(FTPUserTest, UserStatus) {
    // Test status changes
    user->setStatus(simple_ftpd::UserStatus::INACTIVE);
    EXPECT_EQ(user->getStatus(), simple_ftpd::UserStatus::INACTIVE);
    EXPECT_FALSE(user->isEnabled());
    
    user->setStatus(simple_ftpd::UserStatus::ACTIVE);
    EXPECT_EQ(user->getStatus(), simple_ftpd::UserStatus::ACTIVE);
    EXPECT_TRUE(user->isEnabled());
}

TEST_F(FTPUserTest, BasicPermissions) {
    // Test basic permission operations
    std::vector<simple_ftpd::UserPermission> permissions = {
        simple_ftpd::UserPermission::READ,
        simple_ftpd::UserPermission::WRITE,
        simple_ftpd::UserPermission::DELETE,
        simple_ftpd::UserPermission::UPLOAD,
        simple_ftpd::UserPermission::DOWNLOAD
    };
    
    for (const auto& permission : permissions) {
        user->grantPermission(permission);
        EXPECT_TRUE(user->hasPermission(permission));
    }
    
    // Test getting all permissions
    auto all_permissions = user->getPermissions();
    EXPECT_EQ(all_permissions.size(), permissions.size());
    
    for (const auto& permission : permissions) {
        EXPECT_TRUE(all_permissions.find(permission) != all_permissions.end());
    }
}

TEST_F(FTPUserTest, PermissionRemoval) {
    // Test removing all permissions
    user->grantPermission(simple_ftpd::UserPermission::READ);
    user->grantPermission(simple_ftpd::UserPermission::WRITE);
    
    EXPECT_TRUE(user->hasPermission(simple_ftpd::UserPermission::READ));
    EXPECT_TRUE(user->hasPermission(simple_ftpd::UserPermission::WRITE));
    
    user->clearPermissions();
    
    EXPECT_FALSE(user->hasPermission(simple_ftpd::UserPermission::READ));
    EXPECT_FALSE(user->hasPermission(simple_ftpd::UserPermission::WRITE));
}
