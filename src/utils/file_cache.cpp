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

#include "simple-sftpd/file_cache.hpp"
#include "simple-sftpd/logger.hpp"
#include <algorithm>
#include <vector>
#include <chrono>

namespace simple_sftpd {

FileCache::FileCache(std::shared_ptr<Logger> logger, size_t max_entries, 
                     std::chrono::seconds ttl)
    : logger_(logger), max_entries_(max_entries), ttl_(ttl),
      cache_hits_(0), cache_misses_(0) {
}

std::shared_ptr<FileCache::FileMetadata> FileCache::get(const std::string& path) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        auto metadata = it->second;
        if (!isExpired(*metadata)) {
            cache_hits_++;
            return metadata;
        } else {
            // Expired, remove it
            cache_.erase(it);
        }
    }
    
    cache_misses_++;
    return nullptr;
}

void FileCache::put(const std::string& path, const FileMetadata& metadata) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    
    // Evict old entries if cache is full
    if (cache_.size() >= max_entries_) {
        evictOldEntries();
    }
    
    auto cached = std::make_shared<FileMetadata>(metadata);
    cached->cache_time = std::chrono::system_clock::now();
    cache_[path] = cached;
}

void FileCache::invalidate(const std::string& path) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_.erase(path);
}

void FileCache::clear() {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_.clear();
}

size_t FileCache::getSize() const {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    return cache_.size();
}

void FileCache::evictOldEntries() {
    // Remove expired entries first
    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (isExpired(*it->second)) {
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
    
    // If still full, remove oldest entries
    if (cache_.size() >= max_entries_) {
        std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> entries;
        for (const auto& entry : cache_) {
            entries.push_back({entry.first, entry.second->cache_time});
        }
        
        std::sort(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
        
        size_t to_remove = cache_.size() - max_entries_ + 1;
        for (size_t i = 0; i < to_remove && i < entries.size(); ++i) {
            cache_.erase(entries[i].first);
        }
    }
}

bool FileCache::isExpired(const FileMetadata& metadata) const {
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - metadata.cache_time);
    return age > ttl_;
}

} // namespace simple_sftpd

