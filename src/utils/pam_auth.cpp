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

#include "simple-sftpd/pam_auth.hpp"
#include "simple-sftpd/logger.hpp"

#ifndef _WIN32
#ifdef __linux__
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#endif
#include <unistd.h>
#endif

namespace simple_sftpd {

struct pam_data {
    const char* password;
};

#ifndef _WIN32
#ifdef __linux__
static int pam_conv_func(int num_msg, const struct pam_message** msg,
                         struct pam_response** resp, void* appdata_ptr) {
    struct pam_data* data = static_cast<struct pam_data*>(appdata_ptr);
    *resp = static_cast<struct pam_response*>(calloc(num_msg, sizeof(struct pam_response)));
    
    for (int i = 0; i < num_msg; ++i) {
        switch (msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_OFF:
            case PAM_PROMPT_ECHO_ON:
                (*resp)[i].resp = strdup(data->password);
                (*resp)[i].resp_retcode = PAM_SUCCESS;
                break;
            default:
                free((*resp)[i].resp);
                (*resp)[i].resp = nullptr;
                break;
        }
    }
    
    return PAM_SUCCESS;
}

static struct pam_conv conv = {
    pam_conv_func,
    nullptr
};
#endif
#endif

PAMAuth::PAMAuth(std::shared_ptr<Logger> logger)
    : logger_(logger), pam_available_(false), pam_handle_(nullptr) {
#ifndef _WIN32
#ifdef __linux__
    // Check if PAM is available
    pam_available_ = true; // Assume available on Linux
    logger_->info("PAM authentication available");
#else
    pam_available_ = false;
    logger_->warn("PAM not available on this platform");
#endif
#else
    pam_available_ = false;
    logger_->warn("PAM not available on Windows");
#endif
}

PAMAuth::~PAMAuth() {
#ifndef _WIN32
#ifdef __linux__
    if (pam_handle_) {
        pam_end(static_cast<pam_handle_t*>(pam_handle_), 0);
        pam_handle_ = nullptr;
    }
#endif
#endif
}

bool PAMAuth::authenticate(const std::string& username, const std::string& password) {
#ifndef _WIN32
#ifdef __linux__
    if (!pam_available_) {
        return false;
    }
    
    struct pam_data data;
    data.password = password.c_str();
    conv.appdata_ptr = &data;
    
    pam_handle_t* handle = nullptr;
    int ret = pam_start("simple-sftpd", username.c_str(), &conv, &handle);
    if (ret != PAM_SUCCESS) {
        logger_->error("PAM start failed: " + std::string(pam_strerror(handle, ret)));
        return false;
    }
    
    pam_handle_ = handle;
    
    ret = pam_authenticate(handle, 0);
    if (ret != PAM_SUCCESS) {
        logger_->warn("PAM authentication failed for user: " + username);
        pam_end(handle, ret);
        pam_handle_ = nullptr;
        return false;
    }
    
    ret = pam_acct_mgmt(handle, 0);
    if (ret != PAM_SUCCESS) {
        logger_->warn("PAM account management failed for user: " + username);
        pam_end(handle, ret);
        pam_handle_ = nullptr;
        return false;
    }
    
    logger_->info("PAM authentication successful for user: " + username);
    pam_end(handle, PAM_SUCCESS);
    pam_handle_ = nullptr;
    return true;
#else
    (void)username;
    (void)password;
    logger_->warn("PAM not available on this platform");
    return false;
#endif
#else
    (void)username;
    (void)password;
    return false;
#endif
}

} // namespace simple_sftpd

