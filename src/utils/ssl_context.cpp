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

#include "simple-sftpd/ssl_context.hpp"
#include "simple-sftpd/logger.hpp"
#include <fstream>
#include <cstring>

#ifdef SIMPLE_SFTPD_SSL_ENABLED
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#endif

namespace simple_sftpd {

SSLContext::SSLContext(std::shared_ptr<Logger> logger)
    : logger_(logger), enabled_(false), initialized_(false)
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    , ctx_(nullptr)
#endif
{
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    enabled_ = true;
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
#endif
}

SSLContext::~SSLContext() {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (ctx_) {
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
    }
    EVP_cleanup();
#endif
}

bool SSLContext::initialize(const std::string& cert_file, const std::string& key_file, 
                          const std::string& ca_file, bool require_client_cert,
                          const std::string& client_ca_file) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!enabled_) {
        logger_->error("SSL not enabled - OpenSSL not available");
        return false;
    }

    // Create SSL context
    ctx_ = SSL_CTX_new(TLS_server_method());
    if (!ctx_) {
        logSSLErrors();
        return false;
    }

    // Set minimum TLS version to 1.2
    SSL_CTX_set_min_proto_version(ctx_, TLS1_2_VERSION);

    // Load certificate
    if (SSL_CTX_use_certificate_file(ctx_, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        logger_->error("Failed to load certificate file: " + cert_file);
        logSSLErrors();
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
        return false;
    }

    // Load private key
    if (SSL_CTX_use_PrivateKey_file(ctx_, key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        logger_->error("Failed to load private key file: " + key_file);
        logSSLErrors();
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
        return false;
    }

    // Verify private key matches certificate
    if (!SSL_CTX_check_private_key(ctx_)) {
        logger_->error("Private key does not match certificate");
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
        return false;
    }

    // Load CA certificate if provided
    if (!ca_file.empty()) {
        if (SSL_CTX_load_verify_locations(ctx_, ca_file.c_str(), nullptr) <= 0) {
            logger_->warn("Failed to load CA certificate file: " + ca_file);
            logSSLErrors();
        }
    }
    
    // Client certificate authentication
    if (require_client_cert) {
        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
        
        if (!client_ca_file.empty()) {
            if (SSL_CTX_load_verify_locations(ctx_, client_ca_file.c_str(), nullptr) <= 0) {
                logger_->warn("Failed to load client CA certificate file: " + client_ca_file);
                logSSLErrors();
            }
        }
        
        // Set verify depth
        SSL_CTX_set_verify_depth(ctx_, 4);
    }

    // Set cipher list (prefer secure ciphers)
    SSL_CTX_set_cipher_list(ctx_, "HIGH:!aNULL:!MD5:!RC4");

    initialized_ = true;
    logger_->info("SSL context initialized successfully");
    logger_->info("Certificate: " + cert_file);
    logger_->info("Private key: " + key_file);
    return true;
#else
    logger_->error("SSL not enabled - OpenSSL not available");
    return false;
#endif
}

void* SSLContext::createSSL(int socket) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!initialized_ || !ctx_) {
        return nullptr;
    }

    SSL* ssl = SSL_new(ctx_);
    if (!ssl) {
        logSSLErrors();
        return nullptr;
    }

    if (SSL_set_fd(ssl, socket) <= 0) {
        logSSLErrors();
        SSL_free(ssl);
        return nullptr;
    }

    return ssl;
#else
    (void)socket;
    return nullptr;
#endif
}

bool SSLContext::acceptSSL(void* ssl) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!ssl) {
        return false;
    }

    int ret = SSL_accept(static_cast<SSL*>(ssl));
    if (ret <= 0) {
        int err = SSL_get_error(static_cast<SSL*>(ssl), ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            logSSLErrors();
        }
        return false;
    }

    return true;
#else
    (void)ssl;
    return false;
#endif
}

bool SSLContext::connectSSL(void* ssl) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!ssl) {
        return false;
    }

    int ret = SSL_connect(static_cast<SSL*>(ssl));
    if (ret <= 0) {
        int err = SSL_get_error(static_cast<SSL*>(ssl), ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            logSSLErrors();
        }
        return false;
    }

    return true;
#else
    (void)ssl;
    return false;
#endif
}

int SSLContext::readSSL(void* ssl, void* buf, int len) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!ssl) {
        return -1;
    }

    return SSL_read(static_cast<SSL*>(ssl), buf, len);
#else
    (void)ssl;
    (void)buf;
    (void)len;
    return -1;
#endif
}

int SSLContext::writeSSL(void* ssl, const void* buf, int len) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!ssl) {
        return -1;
    }

    return SSL_write(static_cast<SSL*>(ssl), buf, len);
#else
    (void)ssl;
    (void)buf;
    (void)len;
    return -1;
#endif
}

void SSLContext::shutdownSSL(void* ssl) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (ssl) {
        SSL_shutdown(static_cast<SSL*>(ssl));
    }
#else
    (void)ssl;
#endif
}

void SSLContext::freeSSL(void* ssl) {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (ssl) {
        SSL_free(static_cast<SSL*>(ssl));
    }
#else
    (void)ssl;
#endif
}

bool SSLContext::isEnabled() const {
    return enabled_ && initialized_;
}

std::string SSLContext::getLastError() const {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    char buf[256];
    unsigned long err = ERR_get_error();
    if (err) {
        ERR_error_string_n(err, buf, sizeof(buf));
        return std::string(buf);
    }
    return "No SSL error";
#else
    return "SSL not enabled";
#endif
}

void SSLContext::logSSLErrors() {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
        char buf[256];
        ERR_error_string_n(err, buf, sizeof(buf));
        logger_->error("SSL Error: " + std::string(buf));
    }
#endif
}

std::string SSLContext::getClientCertificate(void* ssl) const {
#ifdef SIMPLE_SFTPD_SSL_ENABLED
    if (!ssl) {
        return "";
    }
    
    X509* cert = SSL_get_peer_certificate(static_cast<SSL*>(ssl));
    if (!cert) {
        return "";
    }
    
    // Get certificate subject
    char* subject = X509_NAME_oneline(X509_get_subject_name(cert), nullptr, 0);
    std::string result = subject ? std::string(subject) : "";
    OPENSSL_free(subject);
    X509_free(cert);
    
    return result;
#else
    (void)ssl;
    return "";
#endif
}

} // namespace simple_sftpd

