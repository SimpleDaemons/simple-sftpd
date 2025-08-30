# Development Guide

This guide is for developers who want to understand the Simple-Secure FTP Daemon codebase, build from source, and contribute to the project.

## 🎯 What You'll Learn

By the end of this guide, you'll be able to:
- ✅ Understand the codebase architecture and design patterns
- ✅ Set up a development environment
- ✅ Build the project from source
- ✅ Run tests and quality assurance checks
- ✅ Contribute code and documentation
- ✅ Debug and troubleshoot issues

## 🏗️ Architecture Overview

### High-Level Architecture

Simple-Secure FTP Daemon follows a modular, event-driven architecture designed for high performance and extensibility:

```
┌─────────────────────────────────────────────────────────────┐
│                    Main Application                        │
│                     (main.cpp)                            │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    FTP Server                              │
│                (ftp_server.hpp)                            │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Network   │   Virtual   │    User    │   SSL/TLS   │  │
│  │   Layer     │   Hosts     │ Management │   Layer     │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                 Connection Pool                            │
│            (ftp_connection.hpp)                            │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Command   │   File      │   Session   │   Transfer │  │
│  │  Parser     │  System     │  Management │   Control  │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                 Platform Layer                             │
│                (platform.hpp)                              │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Socket    │   File      │   Process   │   Security │  │
│  │  Operations │  Operations │  Management │   Features │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Core Components

#### 1. FTP Server (`ftp_server.hpp`)
- **Purpose**: Main server orchestrator and connection manager
- **Responsibilities**: 
  - Network binding and listening
  - Connection acceptance and management
  - Virtual host routing
  - SSL/TLS context management
  - Signal handling and graceful shutdown

#### 2. FTP Connection (`ftp_connection.hpp`)
- **Purpose**: Individual connection handler and FTP command processor
- **Responsibilities**:
  - FTP command parsing and execution
  - File transfer operations
  - Session state management
  - Data connection handling
  - Command logging and auditing

#### 3. User Management (`ftp_user.hpp`)
- **Purpose**: User authentication, authorization, and management
- **Responsibilities**:
  - User credential validation
  - Permission checking
  - Access control enforcement
  - User statistics and monitoring
  - Account lifecycle management

#### 4. Virtual Host (`ftp_virtual_host.hpp`)
- **Purpose**: Multi-domain support and isolation
- **Responsibilities**:
  - Domain-specific configuration
  - Document root management
  - SSL certificate binding
  - Access control per domain
  - Domain statistics

#### 5. Configuration (`ftp_server_config.hpp`)
- **Purpose**: Configuration management and validation
- **Responsibilities**:
  - Configuration file parsing
  - Setting validation
  - Default value management
  - Configuration inheritance
  - Hot-reload support

#### 6. Platform Abstraction (`platform.hpp`)
- **Purpose**: Cross-platform compatibility layer
- **Responsibilities**:
  - Socket operations abstraction
  - File system operations
  - Process and user management
  - Security features (chroot, privilege dropping)
  - Path manipulation utilities

#### 7. Logging (`logger.hpp`)
- **Purpose**: Comprehensive logging and monitoring
- **Responsibilities**:
  - Multi-level logging
  - Log rotation and management
  - Performance monitoring
  - Asynchronous logging
  - Multiple output formats

### Design Patterns

#### 1. **Observer Pattern**
- Used for connection events and user activity
- Allows loose coupling between components
- Enables event-driven architecture

#### 2. **Factory Pattern**
- Used for creating connection objects
- Allows different connection types (FTP, FTPS)
- Enables easy extension of connection types

#### 3. **Strategy Pattern**
- Used for authentication methods
- Allows pluggable authentication backends
- Enables easy addition of new auth methods

#### 4. **Command Pattern**
- Used for FTP command processing
- Allows easy addition of new commands
- Enables command logging and auditing

#### 5. **Singleton Pattern**
- Used for configuration and logger instances
- Ensures single point of access
- Enables global state management

## 🔧 Development Environment Setup

### Prerequisites

#### Required Software
- **C++17 Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake 3.16+**: Build system
- **Git**: Version control
- **Make**: Build automation (optional, CMake can be used directly)

#### Required Libraries
- **OpenSSL**: SSL/TLS support
- **jsoncpp**: JSON configuration parsing
- **pkg-config**: Package discovery (Linux/macOS)

#### Optional Libraries
- **libcurl**: HTTP client (for future features)
- **zlib**: Compression support
- **libpam**: PAM authentication (Linux)

### Platform-Specific Setup

#### Linux (Ubuntu/Debian)

```bash
# Install development tools
sudo apt update
sudo apt install build-essential cmake git pkg-config

# Install required libraries
sudo apt install libssl-dev libjsoncpp-dev

# Install optional libraries
sudo apt install libcurl4-openssl-dev libpam0g-dev zlib1g-dev

# Install development tools
sudo apt install valgrind gdb cppcheck clang-tidy
```

#### Linux (CentOS/RHEL)

```bash
# Install development tools
sudo yum groupinstall "Development Tools"
sudo yum install cmake git pkgconfig

# Install required libraries
sudo yum install openssl-devel jsoncpp-devel

# Install optional libraries
sudo yum install libcurl-devel pam-devel zlib-devel

# Install development tools
sudo yum install valgrind gdb cppcheck
```

#### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install development tools
brew install cmake git pkg-config

# Install required libraries
brew install openssl jsoncpp

# Install optional libraries
brew install curl zlib

# Install development tools
brew install valgrind gdb cppcheck
```

#### Windows

```cmd
# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/

# Install CMake
# Download from: https://cmake.org/download/

# Install Git
# Download from: https://git-scm.com/download/win

# Install vcpkg for package management
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install required packages
.\vcpkg install openssl jsoncpp
```

### IDE Setup

#### Visual Studio Code

```json
// .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/include",
                "/usr/include",
                "/usr/local/include"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```

#### CLion

1. Open the project directory
2. CLion will automatically detect CMakeLists.txt
3. Configure build settings in `File > Settings > Build, Execution, Deployment > CMake`
4. Set build type to `Debug` for development

#### Visual Studio

1. Open the project directory
2. Visual Studio will detect CMakeLists.txt
3. Select build configuration (Debug/Release)
4. Configure CMake settings in `Project > CMake Settings`

## 🏗️ Building from Source

### Basic Build Process

#### 1. Clone the Repository

```bash
git clone https://github.com/ssftpd/ssftpd.git
cd ssftpd
```

#### 2. Install Development Dependencies

```bash
# Using the provided script
make install-dev

# Or manually install required packages
# (See platform-specific setup above)
```

#### 3. Configure the Build

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DENABLE_LOGGING=ON \
    -DENABLE_SSL=ON

# Or use the provided Makefile
cd ..
make configure
```

#### 4. Build the Project

```bash
# Using CMake directly
cd build
make -j$(nproc)

# Using the provided Makefile
make build

# Build specific targets
make ssftpd
make tests
make examples
```

#### 5. Install (Optional)

```bash
# Install system-wide
sudo make install

# Install to custom directory
make install DESTDIR=/path/to/install
```

### Build Variants

#### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Release Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### RelWithDebInfo Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)
```

#### MinSizeRel Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j$(nproc)
```

### Build Options

#### Core Options

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_STATIC_LIBS=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_DOCS=OFF
```

#### Feature Options

```bash
cmake .. \
    -DENABLE_SSL=ON \
    -DENABLE_LOGGING=ON \
    -DENABLE_VIRTUAL_HOSTS=ON \
    -DENABLE_USER_MANAGEMENT=ON \
    -DENABLE_RATE_LIMITING=ON \
    -DENABLE_MONITORING=OFF
```

#### Performance Options

```bash
cmake .. \
    -DENABLE_OPTIMIZATION=ON \
    -DENABLE_LTO=OFF \
    -DENABLE_SANITIZERS=OFF \
    -DENABLE_COVERAGE=OFF
```

#### Platform Options

```bash
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DUSE_SYSTEM_LIBS=ON
```

### Cross-Compilation

#### Linux to Windows

```bash
# Install MinGW
sudo apt install mingw-w64

# Configure for cross-compilation
cmake .. \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32
```

#### Linux to macOS

```bash
# Install osxcross
git clone https://github.com/tpoechtrager/osxcross.git
cd osxcross
./build.sh

# Configure for cross-compilation
cmake .. \
    -DCMAKE_SYSTEM_NAME=Darwin \
    -DCMAKE_C_COMPILER=o64-clang \
    -DCMAKE_CXX_COMPILER=o64-clang++ \
    -DCMAKE_FIND_ROOT_PATH=/path/to/osxcross/target
```

## 🧪 Testing and Quality Assurance

### Running Tests

#### Basic Test Execution

```bash
# Run all tests
make test

# Run tests with CMake
cd build
ctest

# Run tests with verbose output
ctest --verbose

# Run specific test suite
ctest -R "unit_tests"
ctest -R "integration_tests"
```

#### Test Configuration

```bash
# Run tests with specific configuration
ctest -C Debug
ctest -C Release

# Run tests in parallel
ctest -j$(nproc)

# Run tests with timeout
ctest --timeout 300

# Run tests with output on failure
ctest --output-on-failure
```

#### Individual Test Execution

```bash
# Run specific test executable
cd build
./tests/unit_tests
./tests/integration_tests

# Run with command line arguments
./tests/unit_tests --gtest_filter="*FTP*"
./tests/unit_tests --gtest_repeat=3
```

### Test Types

#### Unit Tests
- **Location**: `src/tests/unit/`
- **Framework**: Google Test
- **Coverage**: Individual class and method testing
- **Execution**: Fast, isolated tests

#### Integration Tests
- **Location**: `src/tests/integration/`
- **Framework**: Google Test
- **Coverage**: Component interaction testing
- **Execution**: Medium speed, component-level tests

#### Performance Tests
- **Location**: `src/tests/performance/`
- **Framework**: Google Benchmark
- **Coverage**: Performance regression testing
- **Execution**: Slow, performance measurement

#### End-to-End Tests
- **Location**: `src/tests/e2e/`
- **Framework**: Custom test framework
- **Coverage**: Full system testing
- **Execution**: Very slow, system-level tests

### Code Coverage

#### Enabling Coverage

```bash
# Configure with coverage
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_COVERAGE=ON \
    -DCMAKE_CXX_FLAGS="--coverage"

# Build and run tests
make
make test

# Generate coverage report
make coverage
```

#### Coverage Analysis

```bash
# View coverage report
make coverage-view

# Generate HTML report
make coverage-html

# Generate XML report
make coverage-xml

# Check coverage thresholds
make coverage-check
```

### Static Analysis

#### Cppcheck

```bash
# Run cppcheck
make cppcheck

# Run with specific options
cppcheck --enable=all --xml --xml-version=2 src/ 2> cppcheck-report.xml

# Run on specific files
cppcheck --enable=all src/core/ src/utils/
```

#### Clang-Tidy

```bash
# Run clang-tidy
make clang-tidy

# Run with specific checks
clang-tidy -checks=*,-fuchsia-*,-google-*,-zircon-*,-abseil-*,-modernize-use-trailing-return-type src/**/*.cpp

# Fix issues automatically
clang-tidy -checks=*,-fuchsia-*,-google-*,-zircon-*,-abseil-*,-modernize-use-trailing-return-type -fix src/**/*.cpp
```

#### Valgrind (Memory Checking)

```bash
# Run tests with valgrind
make memcheck

# Run specific test with valgrind
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./tests/unit_tests

# Generate valgrind report
valgrind --tool=memcheck --leak-check=full --xml=yes --xml-file=valgrind-report.xml ./tests/unit_tests
```

## 🔍 Debugging

### Debug Build Configuration

```bash
# Configure debug build
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-g -O0 -fno-omit-frame-pointer" \
    -DENABLE_LOGGING=ON \
    -DLOG_LEVEL=DEBUG

# Build debug version
make -j$(nproc)
```

### Debugging with GDB

```bash
# Start debugging
gdb ./ssftpd

# Set breakpoints
(gdb) break main
(gdb) break FTPServer::start
(gdb) break FTPConnection::handleCommand

# Run with arguments
(gdb) run --config /path/to/config.conf

# Step through code
(gdb) next
(gdb) step
(gdb) continue

# Inspect variables
(gdb) print config
(gdb) print server
(gdb) print connection
```

### Debugging with LLDB (macOS)

```bash
# Start debugging
lldb ./ssftpd

# Set breakpoints
(lldb) breakpoint set --name main
(lldb) breakpoint set --name FTPServer::start
(lldb) breakpoint set --name FTPConnection::handleCommand

# Run with arguments
(lldb) run --config /path/to/config.conf

# Step through code
(lldb) next
(lldb) step
(lldb) continue

# Inspect variables
(lldb) print config
(lldb) print server
(lldb) print connection
```

### Debugging with Visual Studio

1. Open the project in Visual Studio
2. Set breakpoints by clicking in the left margin
3. Press F5 to start debugging
4. Use F10 (step over), F11 (step into), F5 (continue)
5. Inspect variables in the Watch window

### Logging for Debugging

```cpp
// Enable debug logging
logger->setLogLevel(LogLevel::DEBUG);
logger->setLogFormat(LogFormat::EXTENDED);

// Add debug statements
logger->debug("Entering function: " + __FUNCTION__);
logger->debug("Variable value: " + std::to_string(value));
logger->debug("File: " + __FILE__ + " Line: " + std::to_string(__LINE__));
```

## 📝 Contributing Guidelines

### Code Style

#### C++ Style Guide

- **Standard**: C++17
- **Naming**: 
  - Classes: `PascalCase` (e.g., `FTPServer`)
  - Functions: `camelCase` (e.g., `startServer`)
  - Variables: `snake_case` (e.g., `server_config`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_CONNECTIONS`)
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 120 characters maximum
- **Braces**: Allman style (opening brace on new line)

#### Example Code Style

```cpp
class FTPServer
{
public:
    explicit FTPServer(std::shared_ptr<FTPServerConfig> config);
    
    bool start();
    void stop();
    
private:
    void serverLoop();
    bool setupSSL();
    
    std::shared_ptr<FTPServerConfig> config_;
    std::atomic<bool> running_;
    std::thread server_thread_;
};
```

### Documentation Standards

#### Code Documentation

```cpp
/**
 * @brief FTP Server main class
 * 
 * This class manages the main FTP server functionality including:
 * - Listening for incoming connections
 * - Managing multiple FTP connections
 * - User authentication and authorization
 * - Virtual host support
 * - SSL/TLS support
 */
class FTPServer
{
public:
    /**
     * @brief Start the FTP server
     * @return true if started successfully, false otherwise
     * @throws std::runtime_error if server cannot be started
     */
    bool start();
};
```

#### Commit Message Format

```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test changes
- `chore`: Build/tooling changes

**Examples:**
```
feat(server): add virtual host support

fix(connection): resolve memory leak in data transfer

docs(api): add comprehensive class documentation

refactor(config): simplify configuration parsing logic
```

### Development Workflow

#### 1. Fork and Clone

```bash
# Fork the repository on GitHub
# Clone your fork
git clone https://github.com/yourusername/ssftpd.git
cd ssftpd

# Add upstream remote
git remote add upstream https://github.com/ssftpd/ssftpd.git
```

#### 2. Create Feature Branch

```bash
# Update main branch
git checkout main
git pull upstream main

# Create feature branch
git checkout -b feature/your-feature-name
```

#### 3. Make Changes

```bash
# Make your changes
# Follow code style guidelines
# Add tests for new functionality
# Update documentation

# Stage changes
git add .

# Commit changes
git commit -m "feat(component): add new feature description"
```

#### 4. Test Your Changes

```bash
# Build the project
make build

# Run tests
make test

# Run static analysis
make cppcheck
make clang-tidy

# Check code coverage
make coverage
```

#### 5. Push and Create Pull Request

```bash
# Push your branch
git push origin feature/your-feature-name

# Create pull request on GitHub
# Include:
# - Clear description of changes
# - Related issue number
# - Test results
# - Screenshots (if applicable)
```

### Review Process

#### Pull Request Requirements

1. **Code Quality**: Follows style guidelines
2. **Tests**: Includes appropriate tests
3. **Documentation**: Updates relevant documentation
4. **Performance**: No performance regressions
5. **Security**: No security vulnerabilities

#### Review Checklist

- [ ] Code follows style guidelines
- [ ] Tests pass and coverage is adequate
- [ ] Documentation is updated
- [ ] No performance regressions
- [ ] No security issues
- [ ] Changes are minimal and focused
- [ ] Commit messages are clear and descriptive

## 🚀 Performance Optimization

### Profiling

#### Using gprof

```bash
# Configure with profiling
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-pg"

# Build and run
make
./ssftpd --config config.conf

# Generate profile data
gprof ./ssftpd gmon.out > profile.txt
```

#### Using perf (Linux)

```bash
# Profile CPU usage
perf record -g ./ssftpd --config config.conf

# Analyze results
perf report

# Profile specific events
perf record -e cache-misses ./ssftpd --config config.conf
```

#### Using Instruments (macOS)

1. Open Xcode
2. Product > Profile
3. Select Time Profiler
4. Run your application
5. Analyze performance data

### Optimization Techniques

#### Compiler Optimizations

```bash
# Enable all optimizations
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"

# Enable Link Time Optimization
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_LTO=ON
```

#### Code Optimizations

```cpp
// Use move semantics
std::string processData(std::string&& data)
{
    return std::move(data);
}

// Avoid unnecessary copies
const auto& config = server.getConfig();

// Use references where possible
void processUser(const FTPUser& user);

// Use smart pointers
std::unique_ptr<FTPConnection> connection;
```

## 🔒 Security Considerations

### Security Best Practices

#### Input Validation

```cpp
// Validate all user input
bool validatePath(const std::string& path)
{
    // Check for path traversal attempts
    if (path.find("..") != std::string::npos)
        return false;
    
    // Check for absolute paths
    if (path[0] == '/')
        return false;
    
    return true;
}
```

#### Memory Safety

```cpp
// Use smart pointers
std::unique_ptr<FTPConnection> connection = 
    std::make_unique<FTPConnection>(socket, address, virtual_host);

// Avoid raw pointers
// Bad: FTPConnection* connection = new FTPConnection();
// Good: auto connection = std::make_unique<FTPConnection>();
```

#### Authentication Security

```cpp
// Use secure password hashing
std::string hashPassword(const std::string& password)
{
    // Use bcrypt or similar
    return bcrypt_hash(password, 12);
}

// Implement rate limiting
bool checkRateLimit(const std::string& ip)
{
    auto& attempts = rate_limit_map_[ip];
    auto now = std::chrono::steady_clock::now();
    
    // Clean old attempts
    attempts.erase(std::remove_if(attempts.begin(), attempts.end(),
        [now](const auto& attempt) {
            return now - attempt > std::chrono::minutes(15);
        }), attempts.end());
    
    if (attempts.size() >= 5)
        return false;
    
    attempts.push_back(now);
    return true;
}
```

## 📚 Additional Resources

### Documentation

- **API Reference**: [API Documentation](../api/README.md)
- **Configuration Guide**: [Configuration Documentation](../configuration/README.md)
- **User Guide**: [User Documentation](../user-guide/README.md)

### External Resources

- **C++ Core Guidelines**: https://isocpp.github.io/CppCoreGuidelines/
- **Google C++ Style Guide**: https://google.github.io/styleguide/cppguide.html
- **CMake Documentation**: https://cmake.org/documentation/
- **Google Test**: https://github.com/google/googletest

### Community

- **GitHub Issues**: Report bugs and request features
- **GitHub Discussions**: Ask questions and share solutions
- **Contributing Guide**: See CONTRIBUTING.md in the repository
- **Code of Conduct**: See CODE_OF_CONDUCT.md in the repository

---

**Next Steps**: Now that you understand the development process:
1. **Set up your development environment**
2. **Build the project from source**
3. **Run tests and verify everything works**
4. **Pick an issue to work on**
5. **Submit your first contribution**

Welcome to the Simple-Secure FTP Daemon development community! 🎉
