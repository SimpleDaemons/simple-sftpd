#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char** argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Set up test environment
    std::cout << "Running Simple FTP Daemon Unit Tests" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    // Clean up
    std::cout << "\nUnit Tests completed with exit code: " << result << std::endl;
    
    return result;
}
