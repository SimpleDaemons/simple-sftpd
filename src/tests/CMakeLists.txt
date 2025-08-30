# Test configuration for Simple FTP Daemon

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # Try to find GTest manually
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        $ENV{GTEST_ROOT}/include
    )
    
    find_library(GTEST_LIBRARY
        NAMES gtest gtest_main
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        $ENV{GTEST_ROOT}/lib
    )
    
    if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY)
        set(GTEST_FOUND TRUE)
        set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
        set(GTEST_LIBRARIES ${GTEST_LIBRARY})
    endif()
endif()

if(GTEST_FOUND)
    message(STATUS "Google Test found: ${GTEST_LIBRARIES}")
    
    # Include directories
    include_directories(${GTEST_INCLUDE_DIRS})
    
    # Unit tests
    add_executable(unit_tests
        unit/test_main.cpp
        unit/test_ftp_user.cpp
        unit/test_ftp_virtual_host.cpp
        unit/test_ftp_server_config.cpp
        unit/test_logger.cpp
        unit/test_platform.cpp
    )
    
    target_link_libraries(unit_tests
        simple-ftpd-lib
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    # Integration tests
    add_executable(integration_tests
        integration/test_main.cpp
        integration/test_ftp_server.cpp
        integration/test_ftp_connection.cpp
    )
    
    target_link_libraries(integration_tests
        simple-ftpd-lib
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    # Performance tests
    add_executable(performance_tests
        performance/test_main.cpp
        performance/test_transfer_performance.cpp
        performance/test_connection_performance.cpp
    )
    
    target_link_libraries(performance_tests
        simple-ftpd-lib
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    # Add tests to CTest
    add_test(NAME UnitTests COMMAND unit_tests)
    add_test(NAME IntegrationTests COMMAND integration_tests)
    add_test(NAME PerformanceTests COMMAND performance_tests)
    
    # Set test properties
    set_tests_properties(UnitTests PROPERTIES
        TIMEOUT 300
        LABELS "unit"
    )
    
    set_tests_properties(IntegrationTests PROPERTIES
        TIMEOUT 600
        LABELS "integration"
    )
    
    set_tests_properties(PerformanceTests PROPERTIES
        TIMEOUT 900
        LABELS "performance"
    )
    
    # Install test executables
    install(TARGETS unit_tests integration_tests performance_tests
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
    )
    
else()
    message(WARNING "Google Test not found. Tests will not be built.")
    message(STATUS "To install Google Test:")
    message(STATUS "  Ubuntu/Debian: sudo apt-get install libgtest-dev")
    message(STATUS "  CentOS/RHEL: sudo yum install gtest-devel")
    message(STATUS "  macOS: brew install googletest")
    message(STATUS "  Or build from source: https://github.com/google/googletest")
endif()

# Mock objects for testing
add_library(test_mocks STATIC
    mocks/mock_ftp_server.cpp
    mocks/mock_ftp_connection.cpp
    mocks/mock_ftp_user.cpp
    mocks/mock_logger.cpp
)

target_link_libraries(test_mocks
    simple-ftpd-lib
)

# Test utilities
add_library(test_utils STATIC
    utils/test_helpers.cpp
    utils/test_data.cpp
    utils/test_config.cpp
)

target_link_libraries(test_utils
    simple-ftpd-lib
)

# Test configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)

# Test data directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
