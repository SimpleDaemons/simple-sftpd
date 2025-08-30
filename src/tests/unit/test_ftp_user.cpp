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
    EXPECT_EQ(user->getStatus(), simple_ftpd::UserPermission::INACTIVE);
    EXPECT_FALSE(user->isEnabled());
    
    user->setStatus(simple_ftpd::UserStatus::ACTIVE);
    EXPECT_EQ(user->getStatus(), simple_ftpd::UserStatus::ACTIVE);
    EXPECT_TRUE(user->isEnabled());
}

TEST_F(FTPUserTest, ConnectionLimits) {
    // Test connection limit management
    user->setMaxConnections(5);
    EXPECT_EQ(user->getMaxConnections(), 5);
    
    // Test connection increment/decrement
    EXPECT_TRUE(user->incrementConnections());
    EXPECT_TRUE(user->incrementConnections());
    EXPECT_EQ(user->getCurrentConnections(), 2);
    
    user->decrementConnections();
    EXPECT_EQ(user->getCurrentConnections(), 1);
}

TEST_F(FTPUserTest, TransferRateLimits) {
    // Test transfer rate limits
    size_t rate_limit = 1024 * 1024; // 1 MB/s
    user->setTransferRateLimit(rate_limit);
    EXPECT_EQ(user->getTransferRateLimit(), rate_limit);
}

TEST_F(FTPUserTest, FileSizeLimits) {
    // Test file size limits
    size_t max_file_size = 100 * 1024 * 1024; // 100 MB
    user->setMaxFileSize(max_file_size);
    EXPECT_EQ(user->getMaxFileSize(), max_file_size);
}

TEST_F(FTPUserTest, PathAccess) {
    // Test path access validation
    user->setHomeDirectory("/var/ftp/testuser");
    
    // Valid paths
    EXPECT_TRUE(user->canAccessPath("/var/ftp/testuser"));
    EXPECT_TRUE(user->canAccessPath("/var/ftp/testuser/subdir"));
    EXPECT_TRUE(user->canAccessPath("/var/ftp/testuser/file.txt"));
    
    // Invalid paths (outside home directory)
    EXPECT_FALSE(user->canAccessPath("/var/ftp/otheruser"));
    EXPECT_FALSE(user->canAccessPath("/etc/passwd"));
    EXPECT_FALSE(user->canAccessPath("/var/ftp/../etc/passwd"));
}

TEST_F(FTPUserTest, SessionManagement) {
    // Test session timeout
    auto timeout = std::chrono::seconds(3600);
    user->setSessionTimeout(timeout);
    EXPECT_EQ(user->getSessionTimeout(), timeout);
    
    // Test last activity tracking
    auto now = std::chrono::steady_clock::now();
    user->updateLastActivity();
    EXPECT_GE(user->getLastActivity(), now);
}

TEST_F(FTPUserTest, Statistics) {
    // Test user statistics
    EXPECT_EQ(user->getLoginCount(), 0);
    EXPECT_EQ(user->getTotalBytesTransferred(), 0);
    
    // Simulate some activity
    user->incrementLoginCount();
    user->addBytesTransferred(1024);
    
    EXPECT_EQ(user->getLoginCount(), 1);
    EXPECT_EQ(user->getTotalBytesTransferred(), 1024);
}

TEST_F(FTPUserTest, MultiplePermissions) {
    // Test multiple permission operations
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

TEST_F(FTPUserTest, UserProperties) {
    // Test various user properties
    user->setEmail("test@example.com");
    user->setFullName("Test User");
    user->setDescription("Test user account");
    
    EXPECT_EQ(user->getEmail(), "test@example.com");
    EXPECT_EQ(user->getFullName(), "Test User");
    EXPECT_EQ(user->getDescription(), "Test user account");
}

TEST_F(FTPUserTest, AccountExpiration) {
    // Test account expiration
    auto expiration_date = std::chrono::system_clock::now() + std::chrono::hours(24);
    user->setExpirationDate(expiration_date);
    
    EXPECT_EQ(user->getExpirationDate(), expiration_date);
    EXPECT_FALSE(user->isExpired());
    
    // Test expired account
    auto past_date = std::chrono::system_clock::now() - std::chrono::hours(24);
    user->setExpirationDate(past_date);
    EXPECT_TRUE(user->isExpired());
}
