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
    )
    
    target_link_libraries(unit_tests
        ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    
    # Add tests to CTest
    add_test(NAME UnitTests COMMAND unit_tests)
    
    # Set test properties
    set_tests_properties(UnitTests PROPERTIES
        TIMEOUT 300
        LABELS "unit"
    )
    
    # Install test executables
    install(TARGETS unit_tests
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

# Test utilities (only build what exists)
add_library(test_utils STATIC
    utils/test_helpers.cpp
)

# Test configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)

# Test data directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
