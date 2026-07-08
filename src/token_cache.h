#pragma once
#include "database.h"
#include <pqxx/pqxx>
#include <vector>
#include <chrono>
#include <mutex>

class TokenCache {
private:
	pqxx::connection& conn_;
	int ttl_; // time to live
	std::vector<ActiveToken> cache_;
	std::chrono::steady_clock::time_point last_update_;
	bool cache_valid_ = false;
	std::mutex mtx_;
public:
	TokenCache(pqxx::connection& conn, int ttl_seconds = 60)
		: conn_(conn), ttl_(ttl_seconds) {}
	std::vector<ActiveToken> get_tokens() {
		auto now = std::chrono::steady_clock::now();
		{
			std::lock_guard<std::mutex> lock(mtx_);
			if (!cache_valid_ ||
				std::chrono::duration_cast<std::chrono::seconds>(now - last_update_).count() = > ttl_) {
				cache_ = fecth_active_tokens(conn_);
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
};