# Tests CMakeLists.txt for simple-sftpd
# Copyright 2024 SimpleDaemons

cmake_minimum_required(VERSION 3.16)

# Find Google Test - try find_package first, then FetchContent
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Create imported targets if they don't exist
    if(NOT TARGET gtest)
        add_library(gtest STATIC ${googletest_SOURCE_DIR}/googletest/src/gtest-all.cc)
        target_include_directories(gtest PUBLIC ${googletest_SOURCE_DIR}/googletest/include)
        target_include_directories(gtest PUBLIC ${googletest_SOURCE_DIR}/googletest)
    endif()
    
    if(NOT TARGET gtest_main)
        add_library(gtest_main STATIC ${googletest_SOURCE_DIR}/googletest/src/gtest_main.cc)
        target_include_directories(gtest_main PUBLIC ${googletest_SOURCE_DIR}/googletest/include)
        target_link_libraries(gtest_main gtest)
    endif()
    
    if(NOT TARGET gmock)
        add_library(gmock STATIC ${googletest_SOURCE_DIR}/googlemock/src/gmock-all.cc)
        target_include_directories(gmock PUBLIC ${googletest_SOURCE_DIR}/googlemock/include)
        target_include_directories(gmock PUBLIC ${googletest_SOURCE_DIR}/googlemock)
        target_link_libraries(gmock gtest)
    endif()
else()
    # Use found GTest
    message(STATUS "Using system GTest")
endif()

# Enable testing
enable_testing()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Test source files
set(TEST_SOURCES
    unit/test_ftp_user.cpp
    unit/test_ftp_user_manager.cpp
    unit/test_ftp_server_config.cpp
    unit/test_logger.cpp
    unit/test_ftp_rate_limiter.cpp
    unit/test_ftp_connection_manager.cpp
    integration/test_ftp_connection.cpp
    integration/test_ftp_server.cpp
    main.cpp
)

# Create test executable
add_executable(simple-sftpd-tests ${TEST_SOURCES})

# Link libraries
if(GTest_FOUND)
    # System GTest uses different target names
    target_link_libraries(simple-sftpd-tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        Threads::Threads
    )
else()
    # FetchContent GTest
    target_link_libraries(simple-sftpd-tests
        PRIVATE
        gtest
        gtest_main
        gmock
        Threads::Threads
    )
endif()

# Link OpenSSL if SSL is enabled
if(ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(simple-sftpd-tests PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(simple-sftpd-tests PRIVATE SIMPLE_SFTPD_SSL_ENABLED)
endif()

# Add source files for testing (without main.cpp)
target_sources(simple-sftpd-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_user.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_user_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_rate_limiter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_connection_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ftp_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/ftp_server_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/ssl_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/ip_access_control.cpp
)

# Compiler options
if(MSVC)
    target_compile_options(simple-sftpd-tests PRIVATE /W3)
else()
    target_compile_options(simple-sftpd-tests PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Add test to CTest
add_test(NAME simple-sftpd-tests COMMAND simple-sftpd-tests)

# Set test properties
set_tests_properties(simple-sftpd-tests PROPERTIES
    TIMEOUT 300
    LABELS "unit;integration"
)

