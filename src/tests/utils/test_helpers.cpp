#include "test_helpers.hpp"
#include <fstream>
#include <random>
#include <algorithm>
#include <cstring>

namespace ssftpd {
namespace test {

// Generate random string of specified length
std::string generateRandomString(size_t length) {
    static const char charset[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, sizeof(charset) - 2);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    
    return result;
}

// Generate random bytes
std::vector<uint8_t> generateRandomBytes(size_t length) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint8_t> distribution(0, 255);
    
    std::vector<uint8_t> result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result.push_back(distribution(generator));
    }
    
    return result;
}

// Create test file with random content
bool createTestFile(const std::string& filepath, size_t size) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate random content
    auto content = generateRandomBytes(size);
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
    
    return file.good();
}

// Create test file with specific content
bool createTestFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

// Create test directory
bool createTestDirectory(const std::string& dirpath) {
    // Create directory recursively
    std::string current_path;
    std::istringstream path_stream(dirpath);
    std::string path_component;
    
    while (std::getline(path_stream, path_component, '/')) {
        if (path_component.empty()) {
            continue;
        }
        
        current_path += "/" + path_component;
        
        if (!std::filesystem::exists(current_path)) {
            if (!std::filesystem::create_directory(current_path)) {
                return false;
            }
        }
    }
    
    return true;
}

// Create nested directory structure
bool createNestedDirectories(const std::string& base_path, 
                           const std::vector<std::string>& subdirs) {
    std::string current_path = base_path;
    
    for (const auto& subdir : subdirs) {
        current_path += "/" + subdir;
        if (!std::filesystem::exists(current_path)) {
            if (!std::filesystem::create_directory(current_path)) {
                return false;
            }
        }
    }
    
    return true;
}

// Remove test file
bool removeTestFile(const std::string& filepath) {
    if (std::filesystem::exists(filepath)) {
        return std::filesystem::remove(filepath);
    }
    return true;
}

// Remove test directory and contents
bool removeTestDirectory(const std::string& dirpath) {
    if (std::filesystem::exists(dirpath)) {
        return std::filesystem::remove_all(dirpath) > 0;
    }
    return true;
}

// Clean up test environment
void cleanupTestEnvironment(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) {
                std::filesystem::remove_all(path);
            } else {
                std::filesystem::remove(path);
            }
        }
    }
}

// Get file size
size_t getFileSize(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return 0;
    }
    
    return std::filesystem::file_size(filepath);
}

// Check if file exists
bool fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath) && 
           std::filesystem::is_regular_file(filepath);
}

// Check if directory exists
bool directoryExists(const std::string& dirpath) {
    return std::filesystem::exists(dirpath) && 
           std::filesystem::is_directory(dirpath);
}

// Get file permissions
std::filesystem::perms getFilePermissions(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return std::filesystem::perms::none;
    }
    
    return std::filesystem::status(filepath).permissions();
}

// Set file permissions
bool setFilePermissions(const std::string& filepath, std::filesystem::perms perms) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    try {
        std::filesystem::permissions(filepath, perms);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Compare file contents
bool compareFiles(const std::string& file1, const std::string& file2) {
    if (!fileExists(file1) || !fileExists(file2)) {
        return false;
    }
    
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);
    
    if (!f1.is_open() || !f2.is_open()) {
        return false;
    }
    
    // Compare file sizes first
    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    
    if (f1.tellg() != f2.tellg()) {
        return false;
    }
    
    // Reset to beginning
    f1.seekg(0);
    f2.seekg(0);
    
    // Compare content byte by byte
    char c1, c2;
    while (f1.get(c1) && f2.get(c2)) {
        if (c1 != c2) {
            return false;
        }
    }
    
    return f1.eof() && f2.eof();
}

// Generate test data for different scenarios
TestData generateTestData(TestDataType type, size_t size) {
    TestData data;
    data.size = size;
    
    switch (type) {
        case TestDataType::RANDOM:
            data.content = generateRandomBytes(size);
            break;
            
        case TestDataType::ZEROS:
            data.content.resize(size, 0);
            break;
            
        case TestDataType::ONES:
            data.content.resize(size, 1);
            break;
            
        case TestDataType::PATTERN:
            data.content.resize(size);
            for (size_t i = 0; i < size; ++i) {
                data.content[i] = static_cast<uint8_t>(i % 256);
            }
            break;
            
        case TestDataType::TEXT:
            data.content.resize(size);
            for (size_t i = 0; i < size; ++i) {
                data.content[i] = static_cast<uint8_t>('A' + (i % 26));
            }
            break;
    }
    
    return data;
}

// Create temporary file
std::string createTempFile(const std::string& prefix, const std::string& suffix) {
    std::string temp_dir = std::filesystem::temp_directory_path().string();
    std::string filename = prefix + generateRandomString(8) + suffix;
    std::string filepath = temp_dir + "/" + filename;
    
    // Create empty file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file.close();
        return filepath;
    }
    
    return "";
}

// Create temporary directory
std::string createTempDirectory(const std::string& prefix) {
    std::string temp_dir = std::filesystem::temp_directory_path().string();
    std::string dirname = prefix + generateRandomString(8);
    std::string dirpath = temp_dir + "/" + dirname;
    
    if (std::filesystem::create_directory(dirpath)) {
        return dirpath;
    }
    
    return "";
}

// Wait for condition with timeout
bool waitForCondition(std::function<bool()> condition, 
                     std::chrono::milliseconds timeout,
                     std::chrono::milliseconds interval) {
    auto start_time = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - start_time < timeout) {
        if (condition()) {
            return true;
        }
        std::this_thread::sleep_for(interval);
    }
    
    return false;
}

// Measure execution time
std::chrono::microseconds measureExecutionTime(std::function<void()> func) {
    auto start_time = std::chrono::high_resolution_clock::now();
    func();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
}

// Generate test user data
TestUser generateTestUser(const std::string& username) {
    TestUser user;
    user.username = username;
    user.password = generateRandomString(12);
    user.home_directory = "/var/ftp/" + username;
    user.email = username + "@example.com";
    user.full_name = "Test User " + username;
    user.description = "Test user account for " + username;
    
    return user;
}

// Generate test virtual host data
TestVirtualHost generateTestVirtualHost(const std::string& hostname) {
    TestVirtualHost vhost;
    vhost.hostname = hostname;
    vhost.document_root = "/var/ftp/" + hostname;
    vhost.welcome_message = "Welcome to " + hostname;
    vhost.banner_message = hostname + " FTP Server Ready";
    
    return vhost;
}

} // namespace test
} // namespace ssftpd
