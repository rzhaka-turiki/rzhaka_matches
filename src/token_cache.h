#pragma once
#include <chrono>
#include <mutex>
#include <vector>

#include "database.h"

class TokenCache {
public:
    TokenCache(Database& db, int ttl_seconds = 60) : db_(db), ttl_(ttl_seconds) {}

    std::vector<ActiveToken> get_tokens() {
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (!cache_valid_ || std::chrono::duration_cast<std::chrono::seconds>(now - last_update_).count() >= ttl_) {
                cache_ = db_.fetch_active_tokens();
                last_update_ = now;
                cache_valid_ = true;
            }
        }
        return cache_;
    }

    void invalidate() {
        std::lock_guard<std::mutex> lock(mtx_);
        cache_valid_ = false;
    }

private:
    Database& db_;
    int ttl_;
    std::vector<ActiveToken> cache_;
    std::chrono::steady_clock::time_point last_update_;
    bool cache_valid_ = false;
    std::mutex mtx_;
};