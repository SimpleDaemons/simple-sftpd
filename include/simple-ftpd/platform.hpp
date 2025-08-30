#pragma once

#include <string>
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <iphlpapi.h>
    #include <crypt32.h>
    #include <advapi32.h>
    #include <userenv.h>
    #include <shell32.h>
    
    // Windows type definitions
    using socket_t = SOCKET;
    using ssize_t = SSIZE_T;
    
    // Windows error handling
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define SOCKET_ERROR_MSG std::string("Windows Socket Error: ") + std::to_string(WSAGetLastError())
    
    // Windows socket initialization
    inline bool initializeWinsock() {
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
    }
    
    inline void cleanupWinsock() {
        WSACleanup();
    }
    
    // Windows socket close
    inline int closeSocket(socket_t sock) {
        return closesocket(sock);
    }
    
    // Windows socket operations
    inline int sendData(socket_t sock, const char* data, size_t length) {
        return send(sock, data, static_cast<int>(length), 0);
    }
    
    inline int receiveData(socket_t sock, char* buffer, size_t max_length) {
        return recv(sock, buffer, static_cast<int>(max_length), 0);
    }
    
    // Windows file operations
    inline std::string getConfigDir() {
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appDataPath))) {
            return std::string(appDataPath) + "\\simple-ftpd";
        }
        return "C:\\Program Files\\simple-ftpd\\config";
    }
    
    inline std::string getLogDir() {
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appDataPath))) {
            return std::string(appDataPath) + "\\simple-ftpd\\logs";
        }
        return "C:\\Program Files\\simple-ftpd\\logs";
    }
    
    inline std::string getSSLDir() {
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appDataPath))) {
            return std::string(appDataPath) + "\\simple-ftpd\\ssl";
        }
        return "C:\\Program Files\\simple-ftpd\\ssl";
    }
    
#else
    // POSIX systems (Linux, macOS)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <cstring>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <grp.h>
    
    // POSIX type definitions
    using socket_t = int;
    
    // POSIX error handling
    #define SOCKET_ERROR_CODE errno
    #define SOCKET_ERROR_MSG std::string("POSIX Error: ") + std::strerror(errno)
    
    // POSIX socket initialization (no-op)
    inline bool initializeWinsock() { return true; }
    inline void cleanupWinsock() {}
    
    // POSIX socket close
    inline int closeSocket(socket_t sock) {
        return close(sock);
    }
    
    // POSIX socket operations
    inline int sendData(socket_t sock, const char* data, size_t length) {
        return send(sock, data, length, 0);
    }
    
    inline int receiveData(socket_t sock, char* buffer, size_t max_length) {
        return recv(sock, buffer, max_length, 0);
    }
    
    // POSIX file operations
    inline std::string getConfigDir() {
        #ifdef PLATFORM_MACOS
            return "/usr/local/etc/simple-ftpd";
        #else
            return "/etc/simple-ftpd";
        #endif
    }
    
    inline std::string getLogDir() {
        #ifdef PLATFORM_MACOS
            return "/usr/local/var/log/simple-ftpd";
        #else
            return "/var/log/simple-ftpd";
        #endif
    }
    
    inline std::string getSSLDir() {
        #ifdef PLATFORM_MACOS
            return "/usr/local/etc/simple-ftpd/ssl";
        #else
            return "/etc/simple-ftpd/ssl";
        #endif
    }
    
    // POSIX user/group operations
    inline bool changeToUser(const std::string& username) {
        struct passwd* pw = getpwnam(username.c_str());
        if (!pw) return false;
        
        if (setgid(pw->pw_gid) != 0) return false;
        if (setuid(pw->pw_uid) != 0) return false;
        
        return true;
    }
    
    inline bool changeToGroup(const std::string& groupname) {
        struct group* grp = getgrnam(groupname.c_str());
        if (!grp) return false;
        
        if (setgid(grp->gr_gid) != 0) return false;
        return true;
    }
    
    inline std::string getCurrentUser() {
        struct passwd* pw = getpwuid(getuid());
        return pw ? pw->pw_name : "";
    }
    
    inline std::string getCurrentGroup() {
        struct group* grp = getgrgid(getgid());
        return grp ? grp->gr_name : "";
    }
    
    inline bool isRoot() {
        return getuid() == 0;
    }
    
    inline bool dropPrivileges() {
        if (!isRoot()) return true;
        
        // Try to change to a non-privileged user
        const char* users[] = {"ftp", "nobody", "ftpuser", nullptr};
        for (int i = 0; users[i]; ++i) {
            if (changeToUser(users[i])) {
                return true;
            }
        }
        return false;
    }
    
    inline bool createDirectory(const std::string& path, mode_t mode = 0755) {
        return mkdir(path.c_str(), mode) == 0;
    }
    
    inline bool directoryExists(const std::string& path) {
        struct stat st;
        return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
    }
    
    inline bool fileExists(const std::string& path) {
        struct stat st;
        return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
    }
    
    inline bool setFilePermissions(const std::string& path, mode_t mode) {
        return chmod(path.c_str(), mode) == 0;
    }
    
    inline bool setFileOwner(const std::string& path, const std::string& user, const std::string& group) {
        struct passwd* pw = getpwnam(user.c_str());
        struct group* grp = getgrnam(group.c_str());
        
        if (!pw || !grp) return false;
        
        return chown(path.c_str(), pw->pw_uid, grp->gr_gid) == 0;
    }
#endif

// Common platform-independent functions
namespace simple_ftpd {

/**
 * @brief Platform-independent path separator
 */
#ifdef _WIN32
    const char PATH_SEPARATOR = '\\';
#else
    const char PATH_SEPARATOR = '/';
#endif

/**
 * @brief Get platform-specific path separator
 * @return Path separator character for current platform
 */
inline char getPathSeparator() {
    return PATH_SEPARATOR;
}

/**
 * @brief Normalize path for current platform
 * @param path Input path
 * @return Normalized path
 */
inline std::string normalizePath(const std::string& path) {
    std::string result = path;
    char separator = getPathSeparator();
    char other_separator = (separator == '/') ? '\\' : '/';
    
    // Replace other platform's separators with current platform's
    std::replace(result.begin(), result.end(), other_separator, separator);
    
    // Remove duplicate separators
    std::string::iterator new_end = std::unique(result.begin(), result.end(),
        [separator](char a, char b) { return a == separator && b == separator; });
    result.erase(new_end, result.end());
    
    return result;
}

/**
 * @brief Join path components
 * @param components Vector of path components
 * @return Joined path
 */
inline std::string joinPath(const std::vector<std::string>& components) {
    if (components.empty()) return "";
    
    std::string result = components[0];
    char separator = getPathSeparator();
    
    for (size_t i = 1; i < components.size(); ++i) {
        if (!result.empty() && result.back() != separator) {
            result += separator;
        }
        result += components[i];
    }
    
    return result;
}

/**
 * @brief Split path into components
 * @param path Input path
 * @return Vector of path components
 */
inline std::vector<std::string> splitPath(const std::string& path) {
    std::vector<std::string> components;
    std::string component;
    char separator = getPathSeparator();
    
    for (char c : path) {
        if (c == separator) {
            if (!component.empty()) {
                components.push_back(component);
                component.clear();
            }
        } else {
            component += c;
        }
    }
    
    if (!component.empty()) {
        components.push_back(component);
    }
    
    return components;
}

/**
 * @brief Get directory name from path
 * @param path Input path
 * @return Directory name
 */
inline std::string getDirName(const std::string& path) {
    std::vector<std::string> components = splitPath(path);
    if (components.empty()) return "";
    
    components.pop_back(); // Remove filename
    return joinPath(components);
}

/**
 * @brief Get base name from path
 * @param path Input path
 * @return Base name
 */
inline std::string getBaseName(const std::string& path) {
    std::vector<std::string> components = splitPath(path);
    if (components.empty()) return "";
    
    return components.back();
}

/**
 * @brief Check if path is absolute
 * @param path Input path
 * @return true if absolute, false otherwise
 */
inline bool isAbsolutePath(const std::string& path) {
    if (path.empty()) return false;
    
    #ifdef _WIN32
        // Windows: check for drive letter or UNC path
        if (path.length() >= 2 && path[1] == ':') return true;
        if (path.length() >= 2 && path[0] == '\\' && path[1] == '\\') return true;
        return false;
    #else
        // Unix: check if starts with /
        return path[0] == '/';
    #endif
}

/**
 * @brief Resolve relative path to absolute
 * @param base_path Base directory
 * @param relative_path Relative path
 * @return Absolute path
 */
inline std::string resolvePath(const std::string& base_path, const std::string& relative_path) {
    if (isAbsolutePath(relative_path)) {
        return relative_path;
    }
    
    std::vector<std::string> base_components = splitPath(base_path);
    std::vector<std::string> relative_components = splitPath(relative_path);
    
    for (const std::string& comp : relative_components) {
        if (comp == ".") {
            // Current directory, do nothing
        } else if (comp == "..") {
            // Parent directory
            if (!base_components.empty()) {
                base_components.pop_back();
            }
        } else {
            // Regular component
            base_components.push_back(comp);
        }
    }
    
    return joinPath(base_components);
}

} // namespace simple_ftpd
