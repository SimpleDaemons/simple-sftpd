#ifndef SIMPLE_FTPD_TEST_HELPERS_HPP
#define SIMPLE_FTPD_TEST_HELPERS_HPP

#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <filesystem>

namespace ssftpd {
namespace test {

// Test data types
enum class TestDataType {
    RANDOM,     // Random bytes
    ZEROS,      // All zeros
    ONES,       // All ones
    PATTERN,    // Sequential pattern
    TEXT        // ASCII text
};

// Test data structure
struct TestData {
    std::vector<uint8_t> content;
    size_t size = 0;
};

// Test user structure
struct TestUser {
    std::string username;
    std::string password;
    std::string home_directory;
    std::string email;
    std::string full_name;
    std::string description;
};

// Test virtual host structure
struct TestVirtualHost {
    std::string hostname;
    std::string document_root;
    std::string welcome_message;
    std::string banner_message;
};

// String generation
std::string generateRandomString(size_t length);

// Binary data generation
std::vector<uint8_t> generateRandomBytes(size_t length);

// File operations
bool createTestFile(const std::string& filepath, size_t size);
bool createTestFile(const std::string& filepath, const std::string& content);
bool createTestDirectory(const std::string& dirpath);
bool createNestedDirectories(const std::string& base_path, 
                           const std::vector<std::string>& subdirs);

// Cleanup operations
bool removeTestFile(const std::string& filepath);
bool removeTestDirectory(const std::string& dirpath);
void cleanupTestEnvironment(const std::vector<std::string>& paths);

// File utilities
size_t getFileSize(const std::string& filepath);
bool fileExists(const std::string& filepath);
bool directoryExists(const std::string& dirpath);
std::filesystem::perms getFilePermissions(const std::string& filepath);
bool setFilePermissions(const std::string& filepath, std::filesystem::perms perms);

// File comparison
bool compareFiles(const std::string& file1, const std::string& file2);

// Test data generation
TestData generateTestData(TestDataType type, size_t size);

// Temporary file/directory creation
std::string createTempFile(const std::string& prefix = "test_", 
                          const std::string& suffix = ".tmp");
std::string createTempDirectory(const std::string& prefix = "test_");

// Utility functions
bool waitForCondition(std::function<bool()> condition, 
                     std::chrono::milliseconds timeout = std::chrono::seconds(30),
                     std::chrono::milliseconds interval = std::chrono::milliseconds(100));

std::chrono::microseconds measureExecutionTime(std::function<void()> func);

// Test data generation
TestUser generateTestUser(const std::string& username);
TestVirtualHost generateTestVirtualHost(const std::string& hostname);

} // namespace test
} // namespace ssftpd

#endif // SIMPLE_FTPD_TEST_HELPERS_HPP
