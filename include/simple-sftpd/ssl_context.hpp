/*
 * Copyright 2024 SimpleDaemons
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <memory>
#include <string>

#ifdef SIMPLE_SFTPD_SSL_ENABLED
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#endif

namespace simple_sftpd {

class Logger;

/**
 * @brief SSL Context wrapper for OpenSSL
 * 
 * Manages SSL/TLS context, certificates, and SSL connections
 */
class SSLContext {
public:
    SSLContext(std::shared_ptr<Logger> logger);
    ~SSLContext();

    /**
     * @brief Initialize SSL context
     * @param cert_file Path to certificate file
     * @param key_file Path to private key file
     * @param ca_file Optional path to CA certificate file
     * @return true if successful, false otherwise
     */
    bool initialize(const std::string& cert_file, const std::string& key_file, const std::string& ca_file = "");

    /**
     * @brief Create SSL connection from socket
     * @param socket File descriptor for socket
     * @return SSL pointer or nullptr on error
     */
    void* createSSL(int socket);

    /**
     * @brief Accept SSL handshake (server side)
     * @param ssl SSL connection
     * @return true if successful, false otherwise
     */
    bool acceptSSL(void* ssl);

    /**
     * @brief Connect SSL handshake (client side)
     * @param ssl SSL connection
     * @return true if successful, false otherwise
     */
    bool connectSSL(void* ssl);

    /**
     * @brief Read data from SSL connection
     * @param ssl SSL connection
     * @param buf Buffer to read into
     * @param len Maximum bytes to read
     * @return Number of bytes read, or -1 on error
     */
    int readSSL(void* ssl, void* buf, int len);

    /**
     * @brief Write data to SSL connection
     * @param ssl SSL connection
     * @param buf Buffer to write
     * @param len Number of bytes to write
     * @return Number of bytes written, or -1 on error
     */
    int writeSSL(void* ssl, const void* buf, int len);

    /**
     * @brief Shutdown SSL connection
     * @param ssl SSL connection
     */
    void shutdownSSL(void* ssl);

    /**
     * @brief Free SSL connection
     * @param ssl SSL connection
     */
    void freeSSL(void* ssl);

    /**
     * @brief Check if SSL is enabled
     * @return true if SSL is available and initialized
     */
    bool isEnabled() const;

    /**
     * @brief Get SSL error string
     * @return Error description
     */
    std::string getLastError() const;

private:
    std::shared_ptr<Logger> logger_;
    bool enabled_;
    bool initialized_;

#ifdef SIMPLE_SFTPD_SSL_ENABLED
    SSL_CTX* ctx_;
#endif

    void logSSLErrors();
};

} // namespace simple_sftpd

